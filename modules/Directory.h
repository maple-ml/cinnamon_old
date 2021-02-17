#pragma once
#include <filesystem>
#include <string>

namespace cinnamon::fs {
    class __declspec(dllexport) Directory
    {
    private:
        std::string path;

    public:
        /// constructors
        Directory();
        Directory(std::string path);
        static Directory* create(std::string path);

        // parameter declarations
        void set_path(std::string path);
        std::string get_path();

        // actual functions
        std::filesystem::directory_iterator files();
        int size();
    };
}