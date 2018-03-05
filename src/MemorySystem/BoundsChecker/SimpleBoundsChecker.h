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
			bool ValidateCanary(void* memory) const;

		private:
			const uint32_t m_canary = 0xCA;
		};
	}
}
