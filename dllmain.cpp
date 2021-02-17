#include <codecvt>

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
#include "lib/tinydir.h"
//#include "lib/cinnamon.h"

namespace stdfs = std::filesystem;
namespace fs = cinnamon::fs;

/// WARNING: If you don't know what you're doing, please avoid everything below here. ///

std::wstring s2ws(const std::string& str)
{
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.from_bytes(str);
}

std::string ws2s(const std::wstring& wstr)
{
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.to_bytes(wstr);
}

int str_ends_with(const char* str, const char* suffix) {

    if (str == NULL || suffix == NULL)
        return 0;

    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);

    if (suffix_len > str_len)
        return 0;

    return 0 == strncmp(str + str_len - suffix_len, suffix, suffix_len);
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

void init_mods()
{
	// this is all to get the directory path properly
	// can someone please find a way to make this look not ugly
    auto e = stdfs::current_path().c_str();
    std::wstring path = std::wstring(stdfs::current_path().c_str()) + s2ws(std::string("/cinnamon/mods"));
    println("DEBUG", "Loading Mods");
    tinydir_dir dir;
    tinydir_open(&dir,path.c_str());

	// iterate.
    while (dir.has_next)
    {
    	// read file
        tinydir_file file;
        if (tinydir_readfile(&dir, &file) == -1)
        {
            println("ERROR", "Failed to load a specific file.");
            goto bail;
        }

    	if (file.is_dir)
    	{
    		// pain
            std::wstring pain(file.path);
            std::string fpath(pain.begin(), pain.end());

    		// this
    		if (str_ends_with(fpath.c_str(), ".") == 0) // ignore the .'s
    		{
                tinydir_dir moddir;
                tinydir_file mainmodfile;

    			// file name
                std::wstring wname(file.name);
                std::string name(wname.begin(), wname.end());
    			
    			
                tinydir_open(&moddir, s2ws(fpath).c_str());
                println(fmt::format("{}/INFO", name), fmt::format("Loading {}...", name));

                if (tinydir_file_open(&mainmodfile, s2ws(fpath + std::string("/mod.py")).c_str()) == -1)
                {
                    println(fmt::format("{}/ERROR", name), fpath + std::string("/mod.py"));
                    println(fmt::format("{}/ERROR", name), fmt::format("{} does not contain a mod.py file.", name));
                } else
                {
	                
                }
    		}
    	}

    	if (tinydir_next(&dir) == -1)
        {
  
            println("DEBUG", "Finished loading mods.");
            goto bail;
        }
    }

bail:
    tinydir_close(&dir);
	// done
}

void init_python()
{
    println("DEBUG", "Loading Python API...");

	// set program name really quickly
    wchar_t* program = Py_DecodeLocale("cinnamon-python", NULL);
    Py_SetProgramName(program);

    // initialize python
    Py_Initialize();

    // load mods (or try to)
    init_mods();
}

void init()
{
	// create the main directory if it doesn't exist
    fs::Directory::create(stdfs::current_path().string() + "/cinnamon");
	
    // create the mods directory if it doesn't exist
    fs::Directory::create(stdfs::current_path().string() + "/cinnamon/mods");

	// init pythonic api
    init_python();

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

	
	// init everything
    init();

    return 0;
}

_declspec(dllexport) BOOL APIENTRY DllMain(HMODULE hModule,
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
        }
    }
    }
	
    return TRUE;
}