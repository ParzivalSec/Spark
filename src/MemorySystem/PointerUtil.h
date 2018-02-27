#pragma once

#include <cstdint>
#include <cassert>

namespace sp
{
	namespace pointerUtil
	{
		bool IsPowerOfTwo(size_t number);

		template<typename T>
		bool IsAlignedTo(T* pointer, size_t alignment)
		{
			union
			{
				uintptr_t as_uintptr;
				T* as_T_ptr;
			};

			as_T_ptr = pointer;
			return alignment == 0 || (as_uintptr & (alignment - 1)) == 0;
		}

		///
		/// Align a pointer to the next higher alignment boundary (only POTs)
		///
		template<typename T>
		T* AlignTop(T* pointer, size_t alignment)
		{
			assert(IsPowerOfTwo(alignment) && "Alignment was no power-of-two");

			union
			{
				uintptr_t as_uintptr;
				T* as_T_ptr;
			};

			as_T_ptr = pointer;
			as_uintptr += (alignment - 1);
			as_uintptr &= ~(alignment - 1);

			return as_T_ptr;
		}

		///
		/// Align the pointer to the next smaller alignment boundary (only POTs)
		///
		template<typename T>
		void* AlignBottom(void* pointer, size_t alignment)
		{
			assert(IsPowerOfTwo(alignment) && "Alignment was no power-of-two");

			union
			{
				uintptr_t as_uintptr;
				T* as_T_ptr;
			};

			as_T_ptr = pointer;
			as_uintptr &= ~(alignment - 1);

			return as_T_ptr;
		}

		///
		/// PointerAs
		///
		template<typename T>
		T PointerAs(void* pointer, size_t offset)
		{
			union
			{
				void* as_void;
				uintptr_t as_ptr;
				T as_type;
			};

			as_void = pointer;
			as_ptr += offset;
			return as_type;
		}
	}
}
