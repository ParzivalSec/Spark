#pragma once

#include <new>

#include "VirtualMemory/VirtualMemory.h"
#include "Pointers/PointerUtil.h"
#include "Math/MathUtil.h"

namespace sp
{
	namespace container
	{
		template<typename T>
		class Vector
		{
		public:
			Vector(void);
			Vector(const Vector<T>& other);
			Vector<T>& operator=(const Vector<T>& other);

			size_t size(void) const;
			size_t capacity(void) const;
			static size_t max_size(void);
			bool empty(void) const;

			void push_back(const T& object);

			void reserve(size_t newCapacity);

			void resize(size_t newSize);
			void resize(size_t newSize, const T& object);

			void erase(size_t index);
			void erase(size_t rangeBegin, size_t rangeEnd);
			void erase_by_swap(size_t index);
			void clear(void);

			void shrink_to_fit(void);

			T& operator[] (size_t index);
			const T& operator[](size_t index) const;

			~Vector(void);

		private:

			void GrowByBytes(size_t bytes);
			size_t GetGrowSizeInElements(void) const;

			size_t m_size;
			size_t m_capacity;

			char* m_virtual_mem_begin;
			char* m_virtual_mem_end;
			char* m_physical_mem_begin;
			char* m_physical_mem_end;

			static const size_t MAX_VECTOR_CAPACITY = 1024 * 1024 * 1024;
			static const uint32_t MAX_ELEMENTS = MAX_VECTOR_CAPACITY / sizeof(T);
		};
	
#pragma region Implementation

		template <typename T>
		Vector<T>::Vector()
			: m_size(0u)
			, m_capacity(0u)
			, m_virtual_mem_begin(static_cast<char*>(sp::memory::ReserveAddressSpace(MAX_VECTOR_CAPACITY)))
			, m_virtual_mem_end(m_virtual_mem_begin + MAX_VECTOR_CAPACITY)
			, m_physical_mem_begin(m_virtual_mem_begin)
			, m_physical_mem_end(m_physical_mem_begin)
		{}

		template <typename T>
		Vector<T>::Vector(const Vector<T>& other)
			: m_size(0u)
			, m_capacity(0u)
			, m_virtual_mem_begin(static_cast<char*>(sp::memory::ReserveAddressSpace(MAX_VECTOR_CAPACITY)))
			, m_virtual_mem_end(m_virtual_mem_begin + MAX_VECTOR_CAPACITY)
			, m_physical_mem_begin(m_virtual_mem_begin)
			, m_physical_mem_end(m_physical_mem_begin)
		{
			reserve(other.m_capacity);
			for (size_t idx = 0u; idx < other.m_size; ++idx)
			{
				push_back(other[idx]);
			}
		}

		template <typename T>
		Vector<T>& Vector<T>::operator=(const Vector<T>& other)
		{
			if (this != &other)
			{
				T* internalArray = pointerUtil::pseudo_cast<T*>(m_physical_mem_begin, 0);
				// destruct elements of this vector
				for (size_t idx = 0u; idx < m_size; ++idx)
				{
					internalArray[idx].~T();
				}

				// adjust capacity to match other vector only if the others capacity is larger than ours
				// if it is lesser or equal we go with the current capacity and just copy in the others content
				if (other.m_capacity > m_capacity)
				{
					reserve(other.m_capacity);
				}

				// need to set size to 0, so push_back will work correctly
				m_size = 0u;

				// copy everything from the other vector
				for (size_t idx = 0u; idx < other.m_size; ++idx)
				{
					push_back(other[idx]);
				}
			}

			return *this;
		}

		template <typename T>
		Vector<T>::~Vector()
		{
			T* internalArray = pointerUtil::pseudo_cast<T*>(m_physical_mem_begin, 0);
			for (size_t idx = 0u; idx < m_size; ++idx)
			{
				internalArray[idx].~T();
			}
			memory::FreeAddressSpace(m_virtual_mem_begin);
		}

		template <typename T>
		size_t Vector<T>::size() const
		{
			return m_size;
		}

		template <typename T>
		size_t Vector<T>::capacity() const
		{
			return m_capacity;
		}

		template <typename T>
		size_t Vector<T>::max_size()
		{
			return MAX_ELEMENTS;
		}

		template <typename T>
		bool Vector<T>::empty() const
		{
			return m_size == 0u;
		}

		template <typename T>
		void Vector<T>::push_back(const T& object)
		{
			if (m_capacity == m_size)
			{
				GrowByBytes(GetGrowSizeInElements() * sizeof(T));
			}

			// if we were not able to grow anymore, the placement new will try to write memory that we don't have
			// and therefore propably crash. This only happens in release mode, because of the missing assert in GrowByBytes().
			// we decided to do this anyways, because it is propably better to let the user crash, than to just do nothing and
			// let the user think everything worked fine when it actually did not.

			T* internalArray = pointerUtil::pseudo_cast<T*>(m_physical_mem_begin, 0);
			new (internalArray + m_size) T(object);

			++m_size;
		}

		template <typename T>
		void Vector<T>::resize(size_t newSize)
		{
			{
				const bool resizeRequestExceedsAvailableRange = newSize > MAX_ELEMENTS;
				assert("Resize requested more elements then the max capacity possible" && !resizeRequestExceedsAvailableRange);
			}

			if (newSize == m_size)
			{
				return;
			}

			T* internalArray = pointerUtil::pseudo_cast<T*>(m_physical_mem_begin, 0);
			if (newSize > m_size) // Add n elements until m_size equals newSize
			{
				if (newSize > m_capacity) // If the capacity is not sufficient, we need to grow
				{
					const size_t growSizeInBytes = (newSize - m_capacity) * sizeof(T);
					GrowByBytes(growSizeInBytes);
				}

				for (size_t idx = m_size; idx < newSize; ++idx)
				{
					// Small optimization here for built-in types. Before we called T() here what we discovered zero-initializes built-in types
					// introducing a very small overhead to default-initialization but it can be measured and therefore gained us some performace
					new (internalArray + idx) T;
				}
			}
			else
			{
				//Destruct redundant elements
				for (size_t idx = m_size; idx < newSize; ++idx)
				{
					internalArray[idx].~T();
				}
			}

			m_size = newSize;
		}

		/*
		* This resize overload works just like the resize(size_t) function but with the difference of constructing
		* the new elements using the copy ctor of the T type and call it with the provided template object
		*/
		template <typename T>
		void Vector<T>::resize(size_t newSize, const T& object)
		{
			{
				const bool resizeRequestExceedsAvailableRange = newSize > MAX_ELEMENTS;
				assert("Resize requested more elements then the max capacity possible" && !resizeRequestExceedsAvailableRange);
			}

			if (newSize == m_size)
			{
				return;
			}

			T* internalArray = pointerUtil::pseudo_cast<T*>(m_physical_mem_begin, 0);
			if (newSize > m_size) // Add n elements until m_size equals newSize
			{
				if (newSize > m_capacity) // If the capacity is not sufficient, we need to grow
				{
					const size_t growSizeInBytes = (newSize - m_capacity) * sizeof(T);
					GrowByBytes(growSizeInBytes);
				}

				for (size_t idx = m_size; idx < newSize; ++idx)
				{
					// Here we call T`s CCTOR with the template object from the parameters
					new (internalArray + idx) T(object);
				}
			}
			else
			{
				//Destruct redundant elements
				for (size_t idx = m_size; idx < newSize; ++idx)
				{
					internalArray[idx].~T();
				}
			}

			m_size = newSize;
		}

		template <typename T>
		void Vector<T>::reserve(size_t newCapacity)
		{
			{
				const bool capacityRequestExceedsAvailableRange = newCapacity > MAX_ELEMENTS;
				assert("Reserve requested more capacity then the max capacity possible" && !capacityRequestExceedsAvailableRange);
			}

			//If already big enough, do nothing
			if (newCapacity <= m_capacity)
			{
				return;
			}

			const size_t growSizeInBytes = (newCapacity - m_capacity) * sizeof(T);
			GrowByBytes(growSizeInBytes);
		}

		template <typename T>
		void Vector<T>::erase(size_t index)
		{
			{
				//Check if index is in Range, no negative check needed because size_t is unsigned
				const bool isIndexInRange = index < m_size;
				assert("Index out of Range!" && isIndexInRange);
			}

			T* internalArray = pointerUtil::pseudo_cast<T*>(m_physical_mem_begin, 0);
			T* current = nullptr;
			T* next = nullptr;
			for (size_t idx = index; idx < m_size - 1; ++idx)
			{
				current = internalArray + idx;
				next = internalArray + idx + 1u;
				// INFO: Requires user to properly implement the assignment operator of T
				*current = *next;
			}

			// At the end call the dtor for the last element to free its resources
			internalArray[m_size - 1].~T();
			--m_size;
		}

		template <typename T>
		void Vector<T>::erase(size_t rangeBegin, size_t rangeEnd)
		{
			{
				const bool isEndBiggerThanOrEqualToStart = rangeEnd >= rangeBegin;
				assert("EndIndex needs to be larger than or equal to StartIndex!" && isEndBiggerThanOrEqualToStart);
				const bool isEndInVector = rangeEnd <= m_size - 1;
				assert("EndIndex is out of vector range" && isEndInVector);
			}

			// Quote: The iterator first does not need to be dereferenceable if first==last: erasing an empty range is a no-op.
			// Comes from erasing ranges with iterator begin() and end()
			// If begin == end means begin is not dereferencable and can not be deleted -> no-op
			if (rangeBegin != rangeEnd)
			{
				// Erasing a range needs to bubble up a group of holes
				// To do so we check how many elements shall be deleted and offset the index of the loop by this
				// to assign a still valid object to an invalid hole.
				const size_t elementsToDelete = (rangeEnd - rangeBegin) + 1;
				T* internalArray = pointerUtil::pseudo_cast<T*>(m_physical_mem_begin, 0);
				T* current = nullptr;
				T* next = nullptr;

				for (size_t idx = rangeBegin; idx < m_size - elementsToDelete; ++idx)
				{

					current = internalArray + idx;
					next = internalArray + (idx + elementsToDelete);
					// INFO: Requires user to properly implement the assignment operator of T
					*current = *next;
				}

				// Now delete the bubbled up elements that would leak resources if the dtor was not called
				for (size_t idx = m_size - elementsToDelete; idx < m_size; ++idx)
				{
					internalArray[idx].~T();
				}

				m_size -= elementsToDelete;
			}
		}

		template <typename T>
		void Vector<T>::erase_by_swap(size_t index)
		{
			{
				//Check if index is in Range, no negative check needed because size_t is unsigned
				const bool isIndexInRange = index < m_size;
				assert("Index out of Range!" && isIndexInRange);
			}

			T* internalArray = pointerUtil::pseudo_cast<T*>(m_physical_mem_begin, 0);
			T* lastElement = internalArray + (m_size - 1);

			if (index < m_size - 1)
			{
				T* toDelete = nullptr;
				toDelete = internalArray + index;
				*toDelete = *lastElement;
			}

			lastElement->~T();
			--m_size;
		}

		template <typename T>
		void Vector<T>::clear()
		{
			T* internalArray = pointerUtil::pseudo_cast<T*>(m_physical_mem_begin, 0);
			for (size_t idx = 0u; idx < m_size; ++idx)
			{
				internalArray[idx].~T();
			}

			m_size = 0u;
		}

		template <typename T>
		void Vector<T>::shrink_to_fit()
		{
			// We fullfill the request to handle unused capacity memory back to the OS
			const size_t commitedBytes = math::RoundUp(m_size * sizeof(T), memory::GetPageSize());
			const size_t pinnedCapacity = commitedBytes / sizeof(T);
			if (m_capacity > pinnedCapacity)
			{
				// If we really have some committed pages that do not contain any used elements
				// we can decommit them
				char* unusedMemory = m_physical_mem_begin + commitedBytes;
				const size_t bytesToDecommit = m_physical_mem_end - unusedMemory;
				memory::DecommitPhysicalMemory(unusedMemory, bytesToDecommit);
				m_capacity = pinnedCapacity;
			}
		}

		template <typename T>
		T& Vector<T>::operator[](size_t index)
		{
			//No check for >= 0 needed because index is unsigned!
			assert("Subscript out of range!" && index < m_size);
			T* internalArray = pointerUtil::pseudo_cast<T*>(m_physical_mem_begin, 0);
			return internalArray[index];
		}

		template <typename T>
		const T& Vector<T>::operator[](size_t index) const
		{
			//No check for >= 0 needed because index is unsigned!
			assert("Subscript out of range!" && index < m_size);
			T* internalArray = pointerUtil::pseudo_cast<T*>(m_physical_mem_begin, 0);
			return internalArray[index];
		}

		//INTERNALS

		template <typename T>
		void Vector<T>::GrowByBytes(size_t bytes)
		{
			if (bytes == 0u) return; // Grows by 0 are just rejected

			// Round up to the next highest multiple of the current OS page size
			size_t roundedGrowSize = math::RoundUp(bytes, memory::GetPageSize());

			{
				// If the grow would exceed the available address space we cannot grow anymore
				// this happends if the m_physical_mem pointer is already at the m_virtual_end
				const bool addressSpaceEndReached = m_physical_mem_end == m_virtual_mem_end;
				assert("Grow would exceed maximum available address space - cannot grow further!" && !addressSpaceEndReached);
			}

			// We though about this and decided it makes sense that if a user
			// push_backs into the vector and the grow behaviour would exceed the range
			// then we allow growing to the maximum available address space and just fail to
			// grow if we really are out of memory
			if (m_physical_mem_end + roundedGrowSize > m_virtual_mem_end)
			{
				const size_t remainingGrowSpace = m_virtual_mem_end - m_physical_mem_end;
				roundedGrowSize = math::RoundDown(remainingGrowSpace, memory::GetPageSize());
			}

			char* allocation = static_cast<char*>(memory::CommitPhysicalMemory(m_physical_mem_end, roundedGrowSize));
			m_physical_mem_end = allocation + roundedGrowSize;
			// If the range is not equally divisable by the sizeof(T) this implicitely does a floor(...)
			// and we are good because we don't say that we have more capacity than we really have
			m_capacity = (m_physical_mem_end - m_physical_mem_begin) / sizeof(T);
		}

		template <typename T>
		size_t Vector<T>::GetGrowSizeInElements() const
		{
			// This is a small trick we found in a blog and thought about a bit
			// If we allocate one element it is very probable that we allocate a few more and 
			// it shows a small performance gain when allocating 8 slots at the beginning instead of going 1-2-4-8 for the first few push_backs
			// INFO: This is a better optimization for a non virtual mem based vector implementation but we leave it here as a reference to think
			// about this kind of micro-opts when virtual mem would not be a thing (thank `eternal thing` it is)
			return m_capacity ? m_capacity * 2 : 8;
		}

#pragma endregion Implementation
	
	}
}