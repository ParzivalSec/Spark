#pragma once
#include <utility>

namespace sp
{
	namespace container
	{
		template <typename Item, size_t Capacity>
		class RingBuffer
		{
		public:
			RingBuffer();

			void Write(const Item& element);
			void Write(const Item&& element);

			Item* Read(void);
			Item* Peek(void);
			void  Reset(void);

			bool IsEmpty(void) const { return m_isEmpty; }
			bool IsFull(void)  const { return !IsEmpty(); }

			size_t Size(void) const;

		private:
			bool m_isEmpty;
			size_t m_writeIndex;
			size_t m_readIndex;
			Item m_items[Capacity];
		};

		template <typename Item, size_t Capacity>
		RingBuffer<Item, Capacity>::RingBuffer()
			: m_isEmpty(true)
			, m_writeIndex(0u)
			, m_readIndex(0u)
		{}

		template <typename Item, size_t Capacity>
		void RingBuffer<Item, Capacity>::Write(const Item& element)
		{
			// The RingBuffer cannot be full, if we reach the read pointer while writing
			// we push it forward by one element, overwriting the one previously pointed
			// to in the next Write() invocation
			if (m_writeIndex == m_readIndex && !m_isEmpty)
			{
				++m_readIndex;
			}

			m_isEmpty = false;

			m_items[m_writeIndex] = element;
			m_writeIndex = (m_writeIndex + 1) % Capacity;
		}

		template <typename Item, size_t Capacity>
		void RingBuffer<Item, Capacity>::Write(const Item&& element)
		{
			// The RingBuffer cannot be full, if we reach the read pointer while writing
			// we push it forward by one element, overwriting the one previously pointed
			// to in the next Write() invocation
			if (m_writeIndex == m_readIndex && !m_isEmpty)
			{
				++m_readIndex;
			}

			m_isEmpty = false;

			m_items[m_writeIndex] = std::move(element);
			m_writeIndex = (m_writeIndex + 1) % Capacity;
		}

		template <typename Item, size_t Capacity>
		Item* RingBuffer<Item, Capacity>::Read()
		{
			if (IsEmpty())
			{
				return nullptr;
			}

			Item* readItem = &m_items[m_readIndex];
			m_readIndex = (m_readIndex + 1) % Capacity;
			m_isEmpty = m_readIndex == m_writeIndex;
			return readItem;
		}

		template <typename Item, size_t Capacity>
		Item* RingBuffer<Item, Capacity>::Peek()
		{
			if (IsEmpty())
			{
				return nullptr;
			}

			return &m_items[m_readIndex];
		}

		template <typename Item, size_t Capacity>
		void RingBuffer<Item, Capacity>::Reset()
		{
			m_writeIndex = 0u;
			m_readIndex = 0u;
			m_isEmpty = true;
		}

		template <typename Item, size_t Capacity>
		size_t RingBuffer<Item, Capacity>::Size() const
		{
			size_t size = 0u;

			if (m_readIndex < m_writeIndex)
			{
				size = m_writeIndex - m_readIndex;
			}
			else if (m_readIndex > m_writeIndex)
			{
				const size_t sizeReadToEnd = Capacity - m_readIndex;
				size = sizeReadToEnd + m_writeIndex;
			}

			return size;
		}
	}
}