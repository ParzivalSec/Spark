#pragma once
#include <cstdint>

namespace sp
{
	namespace memory
	{
		/**
		 * This bounds checking strategy adds a marker value
		 * before and after the allocation block. This value
		 * can later be checked for validity to discover
		 * memory stomps.
		 */
		class SimpleBoundsChecker
		{
		public:
			static const uint8_t CANARY_SIZE;

			void WriteCanary(void* memory) const;
			void ValidateFrontCanary(void* memory) const;
			void ValidateBackCanary(void* memory) const;

		private:
			const uint32_t m_canary = 0xCA;
		};
	}
}
