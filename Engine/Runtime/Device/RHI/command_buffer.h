#pragma once
#include <functional>
#include "utils/lockfree_queue.h"
#include <iostream>

namespace redtea {
namespace device {

class CommandBuffer;

class CommandBase
{
public:
	using Execute = void(*)(CommandBase* self, CommandBuffer* buffer, size_t &cmd_size);
	inline ~CommandBase() noexcept = default;
	inline void execute(CommandBuffer* buffer);
protected:
	explicit CommandBase(Execute execute) noexcept : mExecute(execute)
	{
	}
private:
	Execute mExecute;
};

// switch command buffer chunk
class NoopCommand : public CommandBase
{
public:
	explicit NoopCommand(uint8_t* chunk);

	static void SwitchChunk(CommandBase* self, CommandBuffer* buffer, size_t &offset) noexcept;
	uint8_t* nextChunk;
};

class CustomCommand : public CommandBase
{
public:
	explicit CustomCommand(std::function<void()> cmd);
	static void Invoke(CommandBase* self, CommandBuffer* buffer, size_t &offset) noexcept;
	std::function<void()> mCommand;
};

class CommandBuffer
{

struct ReaderContext
{
	uint8_t* chunk;
	int offset;
};

struct WriterContext
{
	uint8_t* chunk;
	int offset;
};
	
enum
{
	kDefaultAlignment = 16,
	// page size is 16 * 4k
	kDefaultMemChunkSize = 4096 * 16,
	// chunk size 64
	kDefaultChunkSize = 64
};

public:
	explicit CommandBuffer(size_t chunkSize);
	~CommandBuffer();
	// can't be moved or copy-constructed
	CommandBuffer(CommandBuffer const& rhs) = delete;
	CommandBuffer(CommandBuffer&& rhs) noexcept = delete;
	CommandBuffer& operator=(CommandBuffer const& rhs) = delete;
	CommandBuffer& operator=(CommandBuffer&& rhs) noexcept = delete;

	inline uint32_t GetCommandNum() { return mCommandNum.load(); }

	bool Empty() const noexcept { return mCommandNum.load() == 0; }

	void SwitchReadingChunk(uint8_t* chunk);
	void SwitchWritingChunk(uint8_t* chunk);
	// Allocate buffer and release buffer
	uint8_t* AllocateBuffer(size_t size);
	inline void ReleaseBuffer(size_t offset) { mReader.offset += Align(offset); };

	// Get command from buffer
	CommandBase* FetchCommand();
	// Write buffer and move wrirte
	template<typename T, typename ... ARGS>
	void WriteCommand(ARGS&& ... args);
	// Fetch and excute one command
	void ProcessOneCommand();
	// Excute all command and clear buffer
	void Flush();
private:
	inline size_t Align(size_t pos, size_t alignment = kDefaultAlignment) const { return (pos + alignment - 1)&~(alignment - 1); }
	
	uint8_t* AllocateChunk();
	void RecycleChunk(uint8_t* chunk);

	template<typename T, typename ... ARGS>
	void QueueCommand(uint8_t* addr, ARGS&& ... args)
	{
		new(addr) T(std::forward<ARGS>(args)...);
		mCommandNum.fetch_add(1);
	};

	// CommandBuffer size
	std::atomic<size_t> mUsedChunkNum{ 0 };
	std::atomic<uint32_t> mCommandNum{ 0 };

	// Reader and Writer
	ReaderContext mReader;
	WriterContext mWriter;

	static constexpr size_t noopCmdSize = sizeof(NoopCommand);
	// Memory chunk list
	common::LockFreeQueue<uint8_t*> mFreeChunk{ kDefaultChunkSize };
};

template<typename T, typename ...ARGS>
inline void CommandBuffer::WriteCommand(ARGS&& ... args)
{
	uint8_t* addr = AllocateBuffer(sizeof(T));
	QueueCommand<T>(addr, (args)...);
 }

}
}