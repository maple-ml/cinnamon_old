#include <Windows.h>

// handles
inline uintptr_t _MODULE_HANDLE = reinterpret_cast<uintptr_t>(GetModuleHandle(0));

// game manager
inline uintptr_t _GAME_MANAGER = _MODULE_HANDLE + 0x3222D0;

// menu layer
inline uintptr_t _MENU_LAYER = _MODULE_HANDLE + 0x1907B0;