/*
 * Copyright (C) 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ostream.h"
#include "../common.h"

#include <algorithm>
#include <string>

namespace redtea {

namespace common {

ostream::~ostream() = default;

// don't allocate any memory before we actually use the log because one of these is created
// per thread.
ostream::Buffer::Buffer() noexcept = default;

ostream::Buffer::~Buffer() noexcept {
    // note: on Android pre r14, thread_local destructors are not called
    free(buffer);
}

void ostream::Buffer::advance(size_t n) noexcept {
    if (n > 0) {
        size_t written = n < size ? size_t(n) : size;
        curr += written;
        size -= written;
    }
}

void ostream::Buffer::reserve(size_t newSize) noexcept {
    size_t offset = curr - buffer;
    if (buffer == nullptr) {
        buffer = (char*)malloc(newSize);
    } else {
        buffer = (char*)realloc(buffer, newSize);
    }
	ASSERT(buffer);
    capacity = newSize;
    curr = buffer + offset;
    size = capacity - offset;
}

void ostream::Buffer::reset() noexcept {
    // aggressively shrink the buffer
    if (capacity > 1024) {
        free(buffer);
        buffer = (char*)malloc(1024);
        capacity = 1024;
    }
    curr = buffer;
    size = capacity;
}

const char* ostream::getFormat(ostream::type t) const noexcept {
    switch (t) {
        case type::SHORT:       return mShowHex ? "0x%hx"  : "%hd";
        case type::USHORT:      return mShowHex ? "0x%hx"  : "%hu";
        case type::CHAR:        return "%c";
        case type::UCHAR:       return "%c";
        case type::INT:         return mShowHex ? "0x%x"   : "%d";
        case type::UINT:        return mShowHex ? "0x%x"   : "%u";
        case type::LONG:        return mShowHex ? "0x%lx"  : "%ld";
        case type::ULONG:       return mShowHex ? "0x%lx"  : "%lu";
        case type::LONG_LONG:   return mShowHex ? "0x%llx" : "%lld";
        case type::ULONG_LONG:  return mShowHex ? "0x%llx" : "%llu";
        case type::DOUBLE:      return "%f";
        case type::LONG_DOUBLE: return "%Lf";
		default: return "";
    }
}

void ostream::growBufferIfNeeded(size_t s) noexcept {
    Buffer& buf = getBuffer();
    if (buf.size < s) {
        size_t used = buf.curr - buf.buffer;
        size_t newCapacity = std::max(size_t(32), used + (s * 3 + 1) / 2); // 32 bytes minimum
        buf.reserve(newCapacity);
		ASSERT(buf.size >= s);
    }
}

ostream& ostream::operator<<(short value) noexcept {
    Buffer& buf = getBuffer();
    size_t s = snprintf(nullptr, 0, getFormat(type::SHORT), value);
    growBufferIfNeeded(s + 1); // +1 to include the null-terminator
    buf.advance(snprintf(buf.curr, buf.size, getFormat(type::SHORT), value));
    return *this;
}

ostream& ostream::operator<<(unsigned short value) noexcept {
    Buffer& buf = getBuffer();
    size_t s = snprintf(nullptr, 0, getFormat(type::USHORT), value);
    growBufferIfNeeded(s + 1); // +1 to include the null-terminator
    buf.advance(snprintf(buf.curr, buf.size, getFormat(type::USHORT), value));
    return *this;
}

ostream& ostream::operator<<(char value) noexcept {
    Buffer& buf = getBuffer();
    size_t s = snprintf(nullptr, 0, getFormat(type::CHAR), value);
    growBufferIfNeeded(s + 1); // +1 to include the null-terminator
    buf.advance(snprintf(buf.curr, buf.size, getFormat(type::CHAR), value));
    return *this;
}

ostream& ostream::operator<<(unsigned char value) noexcept {
    Buffer& buf = getBuffer();
    size_t s = snprintf(nullptr, 0, getFormat(type::UCHAR), value);
    growBufferIfNeeded(s + 1); // +1 to include the null-terminator
    buf.advance(snprintf(buf.curr, buf.size, getFormat(type::UCHAR), value));
    return *this;
}

ostream& ostream::operator<<(int value) noexcept {
    Buffer& buf = getBuffer();
    size_t s = snprintf(nullptr, 0, getFormat(type::INT), value);
    growBufferIfNeeded(s + 1); // +1 to include the null-terminator
    buf.advance(snprintf(buf.curr, buf.size, getFormat(type::INT), value));
    return *this;
}

ostream& ostream::operator<<(unsigned int value) noexcept {
    Buffer& buf = getBuffer();
    size_t s = snprintf(nullptr, 0, getFormat(type::UINT), value);
    growBufferIfNeeded(s + 1); // +1 to include the null-terminator
    buf.advance(snprintf(buf.curr, buf.size, getFormat(type::UINT), value));
    return *this;
}

ostream& ostream::operator<<(long value) noexcept {
    Buffer& buf = getBuffer();
    size_t s = snprintf(nullptr, 0, getFormat(type::LONG), value);
    growBufferIfNeeded(s + 1); // +1 to include the null-terminator
    buf.advance(snprintf(buf.curr, buf.size, getFormat(type::LONG), value));
    return *this;
}

ostream& ostream::operator<<(unsigned long value) noexcept {
    Buffer& buf = getBuffer();
    size_t s = snprintf(nullptr, 0, getFormat(type::ULONG), value);
    growBufferIfNeeded(s + 1); // +1 to include the null-terminator
    buf.advance(snprintf(buf.curr, buf.size, getFormat(type::ULONG), value));
    return *this;
}

ostream& ostream::operator<<(long long value) noexcept {
    Buffer& buf = getBuffer();
    size_t s = snprintf(nullptr, 0, getFormat(type::LONG_LONG), value);
    growBufferIfNeeded(s + 1); // +1 to include the null-terminator
    buf.advance(snprintf(buf.curr, buf.size, getFormat(type::LONG_LONG), value));
    return *this;
}

ostream& ostream::operator<<(unsigned long long value) noexcept {
    Buffer& buf = getBuffer();
    size_t s = snprintf(nullptr, 0, getFormat(type::ULONG_LONG), value);
    growBufferIfNeeded(s + 1); // +1 to include the null-terminator
    buf.advance(snprintf(buf.curr, buf.size, getFormat(type::ULONG_LONG), value));
    return *this;
}

ostream& ostream::operator<<(float value) noexcept {
    return operator<<((double)value);
}

ostream& ostream::operator<<(double value) noexcept {
    Buffer& buf = getBuffer();
    size_t s = snprintf(nullptr, 0, getFormat(type::DOUBLE), value);
    growBufferIfNeeded(s + 1); // +1 to include the null-terminator
    buf.advance(snprintf(buf.curr, buf.size, getFormat(type::DOUBLE), value));
    return *this;
}

ostream& ostream::operator<<(long double value) noexcept {
    Buffer& buf = getBuffer();
    size_t s = snprintf(nullptr, 0, getFormat(type::LONG_DOUBLE), value);
    growBufferIfNeeded(s + 1); // +1 to include the null-terminator
    buf.advance(snprintf(buf.curr, buf.size, getFormat(type::LONG_DOUBLE), value));
    return *this;
}

ostream& ostream::operator<<(bool value) noexcept {
    return operator<<(value ? "true" : "false");
}

ostream& ostream::operator<<(const char* string) noexcept {
    Buffer& buf = getBuffer();
    size_t s = snprintf(nullptr, 0, "%s", string);
    growBufferIfNeeded(s + 1); // +1 to include the null-terminator
    buf.advance(snprintf(buf.curr, buf.size, "%s", string));
    return *this;
}

ostream& ostream::operator<<(const unsigned char* string) noexcept {
    Buffer& buf = getBuffer();
    size_t s = snprintf(nullptr, 0, "%s", string);
    growBufferIfNeeded(s + 1); // +1 to include the null-terminator
    buf.advance(snprintf(buf.curr, buf.size, "%s", string));
    return *this;
}

ostream& ostream::operator<<(const void* value) noexcept {
    Buffer& buf = getBuffer();
    size_t s = snprintf(nullptr, 0, "%p", value);
    growBufferIfNeeded(s + 1); // +1 to include the null-terminator
    buf.advance(snprintf(buf.curr, buf.size, "%p", value));
    return *this;
}

ostream& ostream::hex() noexcept {
    mShowHex = true;
    return *this;
}

ostream& ostream::dec() noexcept {
    mShowHex = false;
    return *this;
}

} // namespace io

} // namespace utils
