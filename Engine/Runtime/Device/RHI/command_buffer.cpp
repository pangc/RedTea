#include "command_buffer.h"
#include "common.h"
#include "utils/memory.h"
#include <algorithm>

namespace redtea {
namespace device {

inline void CommandBase::execute(CommandBuffer * buffer)
{
	size_t next;
	mExecute(this, buffer, next);
	buffer->ReleaseBuffer(next);	// move reader to next command offset
}

NoopCommand::NoopCommand(uint8_t * chunk)
: CommandBase(SwitchChunk), nextChunk(chunk)
{
}

void NoopCommand::SwitchChunk(CommandBase* self, CommandBuffer* buffer, size_t& offset) noexcept
{
	NoopCommand* cmd = static_cast<NoopCommand*>(self);
	buffer->SwitchReadingChunk(cmd->nextChunk);
	offset = 0;
}


CustomCommand::CustomCommand(std::function<void()> cmd)
: CommandBase(Invoke), mCommand(std::move(cmd))
{
}

void CustomCommand::Invoke(CommandBase * self, CommandBuffer * buffer, size_t & offset) noexcept
{
	CustomCommand* cmd = static_cast<CustomCommand*>(self);
	cmd->mCommand();
	offset = sizeof(CustomCommand);
}

CommandBuffer::CommandBuffer(size_t bufferSize)
{
	ASSERT(bufferSize <= kDefaultChunkSize);
	bufferSize = std::max<size_t>(1, bufferSize);
	// malloc a big chunk and split it 
	uint8_t* ptr = nullptr;
	
	for (size_t i = 0; i < bufferSize; i++)
	{
		ptr = common::AllocateMemory<uint8_t>(kDefaultMemChunkSize);
		mFreeChunk.push(ptr);
		ptr = ptr + kDefaultMemChunkSize;
	}

	// init reader and writer
	ptr = AllocateChunk();
	mReader.chunk = ptr;
	mReader.offset = 0;
	mWriter.chunk = ptr;
	mWriter.offset = 0;
}

CommandBuffer::~CommandBuffer()
{
	// free current used chunk;
	uint8_t* chunk = mWriter.chunk;
	if (chunk != nullptr)
	{
		common::ReleaseMemory<uint8_t>(chunk);
	}
	// free chunk pool
	while (mFreeChunk.pop(chunk))
	{
		common::ReleaseMemory<uint8_t>(chunk);
	}
}

uint8_t* CommandBuffer::AllocateBuffer(size_t size)
{
	const size_t align_size = Align(size);
	uint8_t* ptr = nullptr;
	if (kDefaultMemChunkSize - mWriter.offset - noopCmdSize < align_size)
	{
		ASSERT(kDefaultMemChunkSize - mWriter.offset >= noopCmdSize);
		ptr = AllocateChunk();
		uint8_t* addr = mWriter.chunk + mWriter.offset;
		QueueCommand<NoopCommand>(addr, ptr);
		SwitchWritingChunk(ptr);
	}

	// update writer context
	ptr = mWriter.chunk + mWriter.offset;
	mWriter.offset += align_size;
	return ptr;
}



uint8_t* CommandBuffer::AllocateChunk()
{
	// Check used chunk is not full
	if (UNLIKELY(mUsedChunkNum.load() >= kDefaultChunkSize))
	{
		LOGE("CommandBuffer used chunk is full!");
		ASSERT(false);
		std::unique_lock<std::mutex> lock(mLock);
		while (mUsedChunkNum.load() >= kDefaultChunkSize || mRequestExit)
		{
			mCondition.wait(lock);
		}
	}

	uint8_t* ptr = nullptr;
	if (!mFreeChunk.pop(ptr))
	{
		ptr = common::AllocateMemory<uint8_t>(kDefaultMemChunkSize);
	}

	mUsedChunkNum.fetch_add(1);
	return ptr;
}

void CommandBuffer::RecycleChunk(uint8_t * chunk)
{
	ASSERT(chunk != nullptr);
	if (UNLIKELY(mFreeChunk.size() > kDefaultChunkSize))
	{
		common::ReleaseMemory<uint8_t>(chunk);
	}
	else
	{
		mFreeChunk.push(chunk);
	}
	mUsedChunkNum.fetch_sub(1);
	// notify writer
	mCondition.notify_one();
}

void CommandBuffer::SwitchReadingChunk(uint8_t * chunk)
{
	RecycleChunk(mReader.chunk);
	mReader.chunk = chunk;
	mReader.offset = 0;
}

void CommandBuffer::SwitchWritingChunk(uint8_t * chunk)
{
	mWriter.chunk = chunk;
	mWriter.offset = 0;
}

CommandBase * CommandBuffer::FetchCommand()
{
	CommandBase* command = nullptr;
	command = (CommandBase*)(mReader.chunk + mReader.offset);
	return command;
}

void CommandBuffer::ProcessOneCommand()
{
	CommandBase* command = FetchCommand();
	command->execute(this);
	mCommandNum.fetch_sub(1);
}

void CommandBuffer::Flush()
{
	while (mCommandNum.load() > 0 && (!mRequestExit))
	{
		ProcessOneCommand();
	}
}

void CommandBuffer::WaitAndFlush()
{
#if USE_MULTTRHEAD
	// check command can be fetch
	while (WaitForCommand())
	{
		Flush();
	}
#else
	Flush();
#endif

}

bool CommandBuffer::WaitForCommand()
{
	std::unique_lock<std::mutex> lock(mLock);
	while (mCommandNum.load() <= 0 && (!mRequestExit))
	{
		mCondition.wait(lock);
	}
	return mCommandNum.load() > 0;
}

void CommandBuffer::KickOff()
{
	mCondition.notify_one();
}

}
}