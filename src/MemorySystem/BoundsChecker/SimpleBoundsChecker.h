#pragma once
#include <cstdint>

namespace sp
{
	namespace memory
	{
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
