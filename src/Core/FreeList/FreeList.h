#pragma once

namespace sp
{
	namespace core
	{
		class FreeList
		{
		public:
			FreeList();
			FreeList(void* memoryBegin, void* memoryEnd, size_t chunkSize);

			void* GetChunk(void);
			void ReturnChunk(void* chunk);

			bool IsEmpty(void) const { return m_nextChunk == nullptr; }
		private:
			FreeList* m_nextChunk;
		};
	}
}