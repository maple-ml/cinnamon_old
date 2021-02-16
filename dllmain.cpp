#include <iostream>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <string>
#include <windows.h>

#include "incl/fmt/core.h"
#include "incl/python/Python.h"
#include "incl/MinHook.h"

#include "global.hpp"
#include "lib/cinnamon.h"
#include "lib/process.h"
#include "modules/fs.h"

namespace stdfs = std::filesystem;
namespace fs = cinnamon::fs;

/// WARNING: If you don't know what you're doing, please avoid everything below here. ///
void println(std::string title, std::string msg)
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream sstr;

    sstr << std::put_time(&tm, "%d/%m/%Y %H:%M:%S");
	
    std::cout << fmt::format("[{}] {}: {}", sstr.str().c_str(), title.c_str(), msg.c_str()) << std::endl;
}

void init_hook()
{
    println("DEBUG", "Loading hooks...");
    MH_Initialize();
    println("DEBUG", "Hooking: MenuLayer");
    MenuLayer::hook();
    MH_EnableHook(MH_ALL_HOOKS);
}

void init()
{
	// create the main directory if it doesn't exist
    fs::FDirectory::create(stdfs::current_path().string() + "/cinnamon");
	
    // create the mods directory if it doesn't exist
    fs::FDirectory::create(stdfs::current_path().string() + "/cinnamon/mods");

    // init hooks
    init_hook();
}
DWORD WINAPI thread(void* hModule)
{
    // create console
    AllocConsole();
	
    std::string title = fmt::format("Cinnamon {} - running Geometry Dash {}", cinnamon_version(), gd_version());
    SetConsoleTitle(LPCWSTR(title.c_str()));

    // open stdout
    freopen("CONOUT$", "w", stdout);
	
    // print startup
    println("INFO", fmt::format("Starting Cinnamon v{} for Geometry Dash {}", cinnamon_version(), gd_version()));
    println("INFO", fmt::format("Running on: x{}", architecture()));
    println("DEBUG", fmt::format("Module Address: {}", _MODULE_HANDLE));
    println("DEBUG", fmt::format("Executable Path: {}", "d"));

    // initialize the script backend
    Py_Initialize();
	
	// init everything
    init();

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    //wchar_t *program = Py_DecodeLocale(argv[0], NULL);
    switch (ul_reason_for_call)
    {
    // attached to gd
    case DLL_PROCESS_ATTACH: {
        CreateThread(0, 0x1000, thread, hModule, 0, 0);
    }

    // detatched from GD
    case DLL_PROCESS_DETACH: {
        if (Py_FinalizeEx() < 0) {
            FreeConsole(); // free console
            exit(120);
        }
    }
    }
	
    return TRUE;
}