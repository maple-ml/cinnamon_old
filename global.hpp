#include <cstdint>

#ifndef _GLOBAL_HPP
#define _GLOBAL_HPP

// handles
inline uintptr_t _MODULE_HANDLE = reinterpret_cast<uintptr_t>(GetModuleHandle(0));

// game manager
inline uintptr_t _GAME_MANAGER = _MODULE_HANDLE + 0x3222D0;

// menu layer
inline uintptr_t _MENU_LAYER = _MODULE_HANDLE + 0x1907B0;

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
