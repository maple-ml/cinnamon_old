#include "incl/fmt/core.h"
#include "incl/MinHook.h"
#include "lib/process.h"

#include <iostream>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <string>

#include "modules/fs.h"
#include "incl/python/Python.h"

#include <windows.h>

#include "addresses.h"
#include "global.hpp"
//#include "lib/cinnamon.h"

namespace stdfs = std::filesystem;
namespace fs = cinnamon::fs;

/// WARNING: If you don't know what you're doing, please avoid everything below here. ///

std::wstring s2ws(const std::string& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}

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
    //MenuLayer::hook();
    MH_EnableHook(MH_ALL_HOOKS);
    println("DEBUG", "Hooks loaded!");
}

void init_python()
{
    println("DEBUG", "Loading Python API...");
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

	// painfully set the title
    std::wstring title = s2ws(std::string("Cinnamon ")
						+ std::string(cinnamon_version())
						+ std::string(" - running Geometry Dash ")
                        + std::string(gd_version()));
	SetConsoleTitle(title.c_str());

    // open stdout
    freopen("CONOUT$", "w", stdout);
	
    // print startup
    println("INFO", fmt::format("Starting Cinnamon v{} for Geometry Dash {}", cinnamon_version(), gd_version()));
    println("INFO", fmt::format("Running on: x{}", architecture()));
    println("DEBUG", fmt::format("Module Address: {}", _MODULE_HANDLE));
    println("DEBUG", fmt::format("Executable Path: {}", cinnamon::process::current_directory()));

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