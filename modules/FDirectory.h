#pragma once
#include <string>

namespace cinnamon::fs {
    class __declspec(dllexport) FDirectory
    {
    private:
        std::string path;

    public:
        /// constructors
        FDirectory();
        FDirectory(std::string path);
        static FDirectory* create(std::string path);

        // parameter declarations
        void set_path(std::string path);
        std::string get_path();

        // actual functions
        int size();
    };
}