#include "gtest/gtest.h"

#include "RingBuffer/RingBuffer.h"

struct Task
{
	Task() : taskId(0u) {}
	Task(size_t id) : taskId(id) {}

	size_t operator()() const { return kernel(data); }

	size_t taskId;
	void* data;
	size_t (*kernel) (void*);
};

struct HelloWorldData
{
	size_t taskId;
};

size_t AnyKernel(void* data)
{
	HelloWorldData* typedData = sp::pointerUtil::pseudo_cast<HelloWorldData*>(data, 0);
	return typedData->taskId;
}

TEST(RingBuffer, Construction)
{
	const sp::container::RingBuffer<Task, 100> taskBuffer;
	ASSERT_TRUE(taskBuffer.IsEmpty()) << "Freshly constructed RingBuffer was not empty";
}

TEST(RingBuffer, Null_On_Empty_Read)
{
	sp::container::RingBuffer<Task, 100> taskBuffer;
	ASSERT_EQ(taskBuffer.Read(), nullptr) << "Read() on an empty RingBuffer returned a non-null value";
}

TEST(RingBuffer, Null_On_Empty_Peek)
{
	sp::container::RingBuffer<Task, 100> taskBuffer;
	ASSERT_EQ(taskBuffer.Peek(), nullptr) << "Read() on an empty RingBuffer returned a non-null value";
}

TEST(RingBuffer, Write_Tasks)
{
	sp::container::RingBuffer<Task, 100> taskBuffer;

	for (size_t id = 0; id < 10; ++id)
	{
		Task task(id);
		task.data = &task.taskId;
		task.kernel = &AnyKernel;

		taskBuffer.Write(task);
	}

	ASSERT_EQ(taskBuffer.Size(), 10u) << "RingBuffer does not contain 10 elements to read";

	for (size_t id = 0; id < 10; ++id)
	{
		Task* task = taskBuffer.Read();
		ASSERT_EQ(task->taskId, id) << "Task ID did not match";
	}

	ASSERT_TRUE(taskBuffer.IsEmpty()) << "Read loop did not consume all written values";
}

TEST(RingBuffer, Peek_Does_Not_Alter_Size)
{
	sp::container::RingBuffer<Task, 100> taskBuffer;

	for (size_t id = 0; id < 2; ++id)
	{
		Task task(id);
		task.data = &task.taskId;
		task.kernel = &AnyKernel;

		taskBuffer.Write(task);
	}

	Task* task_1 = taskBuffer.Peek();
	Task* task_2 = taskBuffer.Peek();
	ASSERT_EQ(task_1, task_2) << "Peek() returned another element the second time it was called";

	ASSERT_FALSE(taskBuffer.IsEmpty()) << "Peek() calls consumed the elements";
	ASSERT_EQ(taskBuffer.Size(), 2u) << "RingBuffer does not contain 2 elements after Peek()";
}

TEST(RingBuffer, Reset)
{
	sp::container::RingBuffer<Task, 100> taskBuffer;

	for (size_t id = 0; id < 10; ++id)
	{
		Task task(id);
		task.data = &task.taskId;
		task.kernel = &AnyKernel;

		taskBuffer.Write(task);
	}

	ASSERT_EQ(taskBuffer.Size(), 10u) << "RingBuffer does not contain 10 elements to read";
	
	taskBuffer.Reset();

	ASSERT_TRUE(taskBuffer.IsEmpty()) << "Reset() did not reset the RingBuffer";
}