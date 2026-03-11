
#include "LeCore/LeCore.h"
#include "LeCore/LeUtils/LeHash.h"

#include "fnv64.hpp"


uint64 lehash64(const LeChar* str)
{
	return fnv64::fnv1_hash_cstr(str);
}

uint64 lehash64_bin(const void* p, size_t size)
{
	return fnv64::fnv1_hash(p, size);
}