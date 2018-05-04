#include "Ringbuffer_Benchmarks.h"
#include "../CommonStruct.h"
#include "RingBuffer/RingBuffer.h"

static const size_t RINGBUFFER_COUNT = 10000;
static const size_t WRAPPING_OFFSET = 5000;

void ringbuffe_10000_write()
{
	sp::container::RingBuffer<AllocationData, RINGBUFFER_COUNT> ringbuffer;

	for (size_t idx = 0; idx < RINGBUFFER_COUNT; ++idx)
	{
		ringbuffer.Write(AllocationData());
	}
}

void ringbuffer_10000_read()
{
	sp::container::RingBuffer<AllocationData, RINGBUFFER_COUNT> ringbuffer;

	for (size_t idx = 0; idx < RINGBUFFER_COUNT; ++idx)
	{
		ringbuffer.Write(AllocationData());
	}

	for (size_t idx = 0; idx < RINGBUFFER_COUNT; ++idx)
	{
		AllocationData* data = ringbuffer.Read();
	}
}

void ringbuffer_15000_write_wrapping()
{
	sp::container::RingBuffer<AllocationData, RINGBUFFER_COUNT> ringbuffer;

	for (size_t idx = 0; idx < RINGBUFFER_COUNT + WRAPPING_OFFSET; ++idx)
	{
		ringbuffer.Write(AllocationData());
	}
}
