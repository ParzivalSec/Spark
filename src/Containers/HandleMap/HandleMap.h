#pragma once

#include <cstdint>
#include <cassert>
#include <utility>

#include "FreeList/FreeList.h"
#include "Pointers/PointerUtil.h"

//
// A HandleMap ensures internally contiguous storage of elements of type T while
// beeing able to hand out Handles (IDs) to these resources while upholding the
// validity of these handles even though the internal resource was moved to uphold
// contiguous storage
//
// This implementation of a HandleMap is not dynamic and has an upper bound of elements
// that can be stored & memory for these alements will be allocated too at construction
// (cap is provided by the user upon construction)
//
// In the future process of this projects all containers will receive an API to manage
// their storage inside a memory block provided by the more low-level MemorySystem
// (Specifying an Allocator that provides all memory for neccessary allocations)
//

namespace
{
	typedef uint32_t SparseArrayIndex;

	struct HandleData
	{
		size_t   denseArrayIndex;
		uint32_t sparseArrayIndex;
		uint32_t generation;
	};

	struct LookupMeta
	{
		uint32_t denseToSparseIndex;
	};

	struct InternalId
	{
		uint32_t sparseArrayIndex;
		uint32_t generation;
	};
}

namespace sp
{
	namespace container
	{
		typedef size_t Handle;

		template <typename Item>
		class HandleMap
		{
		public:
			HandleMap(uint32_t maximumCapacity);

			Item& At(Handle handle);
			const Item& At(Handle handle) const;

			Item& operator[](Handle handle);
			const Item& operator[](Handle handle) const;

			Item* GetItems(void) { return m_items; }
			const Item* GetItems(void) const { return m_items; }

			Handle Insert(Item&& item);
			Handle Insert(const Item& item);

			void Erase(Handle handle);
			void Clear(void);

			bool IsValid(Handle handle) const;

			size_t Size(void)		const { return m_itemCount; }
			size_t Capacity(void)	const { return m_maxItems;	}

			~HandleMap();

		private:

			uint32_t m_maxItems;
			uint32_t m_itemCount;

			HandleData* m_handles;
			LookupMeta* m_meta;
			Item* m_items;

			core::FreeList m_freeList;
		};

#pragma region Implementation
		template <typename Item>
		HandleMap<Item>::HandleMap(uint32_t maximumCapacity)
			: m_maxItems(maximumCapacity)
			, m_itemCount(0u)
			, m_handles(new HandleData[m_maxItems])
			, m_meta(new LookupMeta[m_maxItems])
			, m_items(new Item[m_maxItems])
		{
			for (uint32_t idx = 0u; idx < m_maxItems; ++idx)
			{
				m_handles[idx].sparseArrayIndex = idx;
			}

			{
				const bool pointerDoesNotOverrideData = sizeof(void*) <= sizeof(size_t);
				assert(pointerDoesNotOverrideData && "FreeList ptr was bigger than sizeof(size_t)");
			}

			m_freeList.~FreeList();
			new (&m_freeList) core::FreeList(m_handles, m_handles + m_maxItems, sizeof(HandleData));
		}

		template <typename Item>
		Item& HandleMap<Item>::At(Handle handle)
		{
			const InternalId internalId = pointerUtil::pseudo_cast<InternalId>(handle, 0);
			const HandleData& handleData = m_handles[internalId.sparseArrayIndex];

			{
				const bool userIndexInRange = internalId.sparseArrayIndex < m_itemCount;
				assert(userIndexInRange && "Index of handle was out of range of the handle array");
				const bool validGeneration = internalId.generation == handleData.generation;
				assert(validGeneration && "Generation is invalid or outdated");
			}

			return m_items[handleData.denseArrayIndex];
		}

		template <typename Item>
		const Item& HandleMap<Item>::At(Handle handle) const
		{
			return At(handle);
		}

		template <typename Item>
		Item& HandleMap<Item>::operator[](Handle handle)
		{
			return At(handle);
		}

		template <typename Item>
		const Item& HandleMap<Item>::operator[](Handle handle) const
		{
			return At(handle);
		}

		template <typename Item>
		Handle HandleMap<Item>::Insert(Item&& item)
		{
			{
				const bool enoughCapacityForInsert = m_itemCount < m_maxItems;
				assert(enoughCapacityForInsert && "Item count reached maximum, cannot insert anymore. Maybe alter the max item cap?");
			}

			if (!m_freeList.IsEmpty())
			{
				HandleData* handleData = pointerUtil::pseudo_cast<HandleData*>(m_freeList.GetChunk(), 0);
				handleData->denseArrayIndex = m_itemCount;

				InternalId newInternalId;
				newInternalId.sparseArrayIndex = handleData->sparseArrayIndex;
				newInternalId.generation = handleData->generation;

				m_meta[m_itemCount].denseToSparseIndex = newInternalId.sparseArrayIndex;

				new (&m_items[m_itemCount]) Item(std::forward<Item>(item));
				++m_itemCount;

				return pointerUtil::pseudo_cast<Handle>(newInternalId, 0);
			}

			return SIZE_MAX;
		}

		template <typename Item>
		Handle HandleMap<Item>::Insert(const Item& item)
		{
			{
				const bool enoughCapacityForInsert = m_itemCount < m_maxItems;
				assert(enoughCapacityForInsert && "Item count reached maximum, cannot insert anymore. Maybe alter the max item cap?");
			}

			if (!m_freeList.IsEmpty())
			{
				HandleData* handleData = pointerUtil::pseudo_cast<HandleData*>(m_freeList.GetChunk(), 0);
				handleData->denseArrayIndex = m_itemCount;

				InternalId newInternalId;
				newInternalId.sparseArrayIndex = handleData->sparseArrayIndex;
				newInternalId.generation = handleData->generation;

				m_meta[m_itemCount].denseToSparseIndex = newInternalId.sparseArrayIndex;

				new (&m_items[m_itemCount]) Item(item);
				++m_itemCount;

				return pointerUtil::pseudo_cast<Handle>(newInternalId, 0);
			}

			return SIZE_MAX;
		}

		template <typename Item>
		void HandleMap<Item>::Erase(Handle handle)
		{
			const InternalId internalId = pointerUtil::pseudo_cast<InternalId>(handle, 0);
			HandleData& handleData = m_handles[internalId.sparseArrayIndex];

			{
				const bool userIndexInRange = internalId.sparseArrayIndex < m_itemCount;
				assert(userIndexInRange && "Index of handle was out of range of the handle array");
			}

			const bool validGeneration = internalId.generation == handleData.generation;
			if (!validGeneration)
			{
				return;
			}

			const uint32_t lastMeshIndex = m_itemCount - 1;
			m_items[handleData.denseArrayIndex] = m_items[lastMeshIndex];
			m_items[lastMeshIndex].~Item();

			const uint32_t indexOfMovedItem = m_meta[lastMeshIndex].denseToSparseIndex;
			m_handles[indexOfMovedItem].denseArrayIndex = handleData.denseArrayIndex;
			m_meta[handleData.denseArrayIndex].denseToSparseIndex = indexOfMovedItem;

			m_freeList.ReturnChunk(&m_handles[internalId.sparseArrayIndex]);

			++handleData.generation;
			m_itemCount = lastMeshIndex;
		}

		template <typename Item>
		void HandleMap<Item>::Clear()
		{
			for (size_t idx = 0u; idx < m_itemCount; ++idx)
			{
				m_items[idx].~Item();
				m_handles[idx].generation += 1u;
			}

			m_freeList.~FreeList();
			new (&m_freeList) core::FreeList(m_handles, m_handles + m_maxItems, sizeof(Item));

			m_itemCount = 0u;
		}

		template <typename Item>
		bool HandleMap<Item>::IsValid(Handle handle) const
		{
			const InternalId internalId = pointerUtil::pseudo_cast<InternalId>(handle, 0);

			if (handle == SIZE_MAX || internalId.sparseArrayIndex >= m_itemCount)
			{
				return false;
			}

			const HandleData& handleData = m_handles[internalId.sparseArrayIndex];

			return handleData.denseArrayIndex < m_itemCount && handleData.generation == internalId.generation;
		}

		template <typename Item>
		HandleMap<Item>::~HandleMap()
		{
			// Delete all elements we hold
			for (size_t idx = 0u; idx < m_itemCount; ++idx)
			{
				m_items[idx].~Item();
			}

			delete[] m_items;
			delete[] m_meta;
			delete[] m_handles;
		}
#pragma endregion 
	}
}