

#pragma once

class LeHash
{
	static uint64 hash64(const LeChar* str);
	static uint64 hash64_bin(const void* p, size_t size);
};

