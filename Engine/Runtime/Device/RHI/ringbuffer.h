#pragma once

namespace redtea {
namespace device {

class RingBuffer
{
public:
	explicit RingBuffer(size_t bufferSize);

	// can't be moved or copy-constructed
	RingBuffer(RingBuffer const& rhs) = delete;
	RingBuffer(RingBuffer&& rhs) noexcept = delete;
	RingBuffer& operator=(RingBuffer const& rhs) = delete;
	RingBuffer& operator=(RingBuffer&& rhs) noexcept = delete;

	void* Allocate(size_t size);

	bool Empty() const noexcept { return mTail == mHead; }

	void* GetHead() const noexcept { return mHead; }

	void* GetTail() const noexcept { return mTail; }

private:
	// ringbuffer memory pointer
	void* mData = nullptr;

	// ringbuffer size
	size_t mSize = 0;

	// pointer to the beginning of recorded data
	void* mTail = nullptr;

	// pointer to the next available command
	void* mHead = nullptr;
};

}
}