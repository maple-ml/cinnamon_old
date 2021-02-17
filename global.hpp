#include <cstdint>

#ifndef _GLOBAL_HPP
#define _GLOBAL_HPP

const char* cinnamon_version()
{
	return "0.1.1";
};

const char* gd_version()
{
	return "2.11";
};

const char* architecture()
{
	if (sizeof(void*) == 8)
		return "64";

	return "32";
}

uintptr_t read(uintptr_t address) {
	return *(reinterpret_cast<uintptr_t*>(address));
}

template <typename T>
T read(uintptr_t address)
{
	return *reinterpret_cast<T*>(read(address));
}

#endif // !GLOBAL_HPP
