#pragma once

namespace sp
{
	namespace core
	{
		/**
		 * The FreeList is an intrusive linked-list. It can be constructed in a
		 * provided memory space. It then stores a pointer to the next chunk, which
		 * is offsetted by chunkSize bytes. If it is used in the storage of a collection
		 * it has to be ensured that every element is at least as big as a pointer on
		 * the target architecture.
		 */
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