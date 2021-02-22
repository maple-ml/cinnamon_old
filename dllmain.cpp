#include <codecvt>
#include <atlstr.h>

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
#include <stdlib.h> 
#include <winnt.h>
#include <stdio.h>

#include "addresses.h"
#include "global.hpp"
#include "lib/tinydir.h"
//#include "lib/cinnamon.h"

using namespace std;

namespace stdfs = filesystem;
namespace fs = cinnamon::fs;


/// WARNING: If you don't know what you're doing, please avoid everything below here. ///

wstring s2ws(const string& str)
{
    using convert_typeX = codecvt_utf8<wchar_t>;
    wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.from_bytes(str);
}

string ws2s(const wstring& wstr)
{
    using convert_typeX = codecvt_utf8<wchar_t>;
    wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.to_bytes(wstr);
}

string int_to_string(int num)
{
    stringstream stream;
    stream << hex << num;
    return stream.str();
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

vector<string> split(const string& str, const string& delim)
{
    vector<string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == string::npos) pos = str.length();
        string token = str.substr(prev, pos - prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());
    return tokens;
}

int setenv(const char* name, const char* value, int overwrite)
{
    int errcode = 0;
    if (!overwrite) {
        size_t envsize = 0;
        errcode = getenv_s(&envsize, NULL, 0, name);
        if (errcode || envsize) return errcode;
    }
    return _putenv_s(name, value);
}

void println(string title, string msg)
{
    auto t = time(nullptr);
    auto tm = *localtime(&t);
    ostringstream sstr;

    sstr << put_time(&tm, "%d/%m/%Y %H:%M:%S");

    cout << fmt::format("[{}] [{}]: {}", sstr.str().c_str(), title.c_str(), msg.c_str()) << endl;
}

int find_path()
{
    char* path = getenv("PATH");
    auto paths = split(string(path), string(";"));

    bool found = false;

    println("DEBUG", path);
	

	for (const auto &strpath : paths)
	{
		if (strpath.find("Python39") != string::npos)
        {
            println("DEBUG", "A");
            // found path
            if (strpath.find("32") != string::npos)
            {
                println("DEBUG", "B");
                // it's 32 bit
                if (strpath.find("Scripts") == string::npos)
                {
                    println("DEBUG", "C");
                    // ignore Scripts path
                    println("DEBUG", strpath);
                    setenv("PYPATH", strpath.c_str(), 0);

                    found = true;
                    break;
                }
            }
		}
	}

	if (found)
	{
        const char* pypath = getenv("PYPATH");
        println("DEBUG", "Found Python Executable in PATH");
        Py_SetPythonHome(Py_DecodeLocale(pypath, NULL));
        return 1;
	} else
	{
        println("DEBUG", "Couldn't find Python Executable in PATH.");
        return 0;
	}
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
    wstring path = wstring(stdfs::current_path().c_str()) + s2ws(string("/cinnamon/mods"));
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
            wstring pain(file.path);
            string fpath(pain.begin(), pain.end());

    		// check if the file path ends with . (ignore recursive and previous dir)
    		if (str_ends_with(fpath.c_str(), ".") == 0) // ignore the .'s
    		{
                tinydir_dir moddir;
                tinydir_file mainmodfile;
                PyObject *modpy, *modpyname;

    			// file name
                wstring wname(file.name);
                string name(wname.begin(), wname.end());

    			// file path
                string mpypath = fpath + string("/mod.py");
                wstring wmpypath = s2ws(mpypath);
    			
                tinydir_open(&moddir, s2ws(fpath).c_str());
                println(fmt::format("{}/INFO", name), fmt::format("Loading {}...", name));

                if (tinydir_file_open(&mainmodfile, CA2T(mpypath.c_str())) == -1)
                {
                    println(fmt::format("{}/ERROR", name), fmt::format("{} does not contain a valid mod.py file.", name));
                    goto bail;
                } else
                {
                	// set cwd to mod folder
                    PyRun_SimpleString((string("import os\nos.chdir('") + fpath + string("')")).c_str());
                    PyRun_SimpleString("import os\nprint(os.getcwd())");
                    Py_Finalize();

                	
                    Py_Initialize();
                    modpyname = PyUnicode_DecodeFSDefault(mpypath.c_str());
                    modpy = PyImport_Import(modpyname);
                    Py_DECREF(modpyname);

                    if (modpy != NULL) {

                    } else
                    {
                        println(fmt::format("{}/ERROR", name), fmt::format("{} does not contain a valid mod.py file.", name));
                        goto bail;
                    }
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

    // find path
    int res = find_path();

	if (res == 1)
	{
        // initialize python
        Py_Initialize();

        // load mods (or try to)
        init_mods();
    }
    else
    {
    	// display warning
        /*system("cls");

    	// change title
        wstring title = s2ws(string("Cinnamon ")
            + string(cinnamon_version())
            + string(" - NOT running Geometry Dash ")
            + string(gd_version()));
        SetConsoleTitle(title.c_str());

    	// display warning
        cout << " " << endl;
        cout << "  WARNING: " << endl;
        cout << "   If you are seeing this message and screen, then that means Cinnamon failed to" << endl;
        cout << "   load entirely. While this means your installation itself is fine, as of right" << endl;
        cout << "   now you do not have Python 3.9.1 (32-bit) installed." << endl;
        cout << " " << endl;
        cout << "   Before continuing to run Cinnamon, please install it before doing such." << endl;
        cout << "   NOTE: Make sure to add it to the environment variables, as well!" << endl;
        cout << " " << endl;
        cout << " " << endl;
        cout << "             Thank you!" << endl;
        cout << "              - The Cinnamon Dev Team" << endl;
        cout << " " << endl;
        cout << "            PS: We appreciate your usage of our modloader!" << endl;*/
    }
}

void init()
{
	// create the main directory if it doesn't exist
    fs::Directory::create(stdfs::current_path().string() + "/cinnamon");
	
    // create the mods directory if it doesn't exist
    fs::Directory::create(stdfs::current_path().string() + "/cinnamon/mods");

    // init hooks
    init_hook();

    // init pythonic api
    init_python();
}

DWORD WINAPI thread(void* hModule)
{
    // create console
    AllocConsole();

	// painfully set the title
    wstring title = s2ws(string("Cinnamon ")
						+ string(cinnamon_version())
						+ string(" - running Geometry Dash ")
                        + string(gd_version()));
	SetConsoleTitle(title.c_str());

	// console stuff
    HWND hwnd = GetConsoleWindow();
    HMENU hmen = GetSystemMenu(hwnd, FALSE);

	// disable closing the console
    //EnableMenuItem(hmen, SC_CLOSE, MF_GRAYED);

    // open stdout
    freopen("CONOUT$", "w", stdout);
	
    // print startup
    println("INFO", fmt::format("Starting Cinnamon v{} for Geometry Dash {}", cinnamon_version(), gd_version()));
    println("INFO", fmt::format("Running on: x{}", architecture()));
    println("DEBUG", fmt::format("Module Address: 0x{}", int_to_string((int)_MODULE_HANDLE)));
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
	
    switch (ul_reason_for_call)
    {
    // attached to gd
    case DLL_PROCESS_ATTACH: {
    		// safecheck
    		try
    		{
                CreateThread(0, 0x1000, thread, hModule, 0, 0);
    		} catch (exception e)
    		{
                MessageBox(0, (LPCWSTR)"Cinnamon: Hi, I'm an Error!", (LPCWSTR)e.what(), MB_OK);

    		}
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