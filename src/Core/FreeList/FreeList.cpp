#include "FreeList.h"
#include <cstddef>
#include "../Pointers/PointerUtil.h"

sp::core::FreeList::FreeList() 
	: m_nextChunk(nullptr)
{}

sp::core::FreeList::FreeList(void* memoryBegin, void* memoryEnd, size_t chunkSize)
	: m_nextChunk(nullptr)
{
	const ptrdiff_t memoryBlockLength = pointerUtil::pseudo_cast<char*>(memoryEnd, 0) - pointerUtil::pseudo_cast<char*>(memoryBegin, 0);
	const size_t elementCount = memoryBlockLength / chunkSize;

	char* memory = pointerUtil::pseudo_cast<char*>(memoryBegin, 0);
	m_nextChunk = pointerUtil::pseudo_cast<FreeList*>(memory, 0);
	memory += chunkSize;

	FreeList* current = m_nextChunk;
	for (size_t i = 0; i < elementCount - 1; ++i)
	{
		current->m_nextChunk = pointerUtil::pseudo_cast<FreeList*>(memory, 0);
		current = current->m_nextChunk;
		memory += chunkSize;
	}
	current->m_nextChunk = nullptr;
}

void* sp::core::FreeList::GetChunk(void)
{
	if (!m_nextChunk)
	{
		return nullptr;
	}

	FreeList* chunk = m_nextChunk;
	m_nextChunk = chunk->m_nextChunk;
	return chunk;
}

void sp::core::FreeList::ReturnChunk(void* chunk)
{
	{
		const bool isNotNullptr = chunk != nullptr;
		assert(isNotNullptr && "Cannot return a nullptr into the FreeList");
	}
	FreeList* newChunk = pointerUtil::pseudo_cast<FreeList*>(chunk, 0);
	newChunk->m_nextChunk = m_nextChunk;
	m_nextChunk = newChunk;
}