#pragma once

#include <cstdint>

namespace sp
{
	namespace pointerUtil
	{
		///
		/// PointerType union used for type punning
		/// This union is an abstraction to allow a void pointer to be interpreted as a
		/// uintptr_t without using a reinterpret cast and therefore without breaking the
		/// pointer-aliasing rule. On should not, that using unions for type-punning is 
		/// implemenation defined behaviour.
		///
		union PointerType
		{
			void* as_void;
			uintptr_t as_ptr;
		};

		///
		/// Align a pointer to the next higher alignment boundary (only POWs)
		///
		void* AlignTop(void* pointer, size_t alignment);

		///
		/// Align the pointer to the next smaller alignment boundary (only POWs)
		///
		void* AlignBottom(void* pointer, size_t alignment);
	}
}
