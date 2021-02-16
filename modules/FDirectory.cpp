#include "FDirectory.h"

#include <iostream>
#include <utility>
#include <filesystem>

namespace stdfs = std::filesystem;

namespace cinnamon::fs {
    // constructors
    FDirectory::FDirectory()
        = default;

    FDirectory::FDirectory(std::string path)
    {
        stdfs::create_directory(path);
    }

    // constructor functions
    std::string FDirectory::get_path()
    {
        return path;
    }

    void FDirectory::set_path(std::string path)
    {
        this->path = std::move(path);
    }

    FDirectory* FDirectory::create(std::string path)
    {
        return new FDirectory(path);
    }

    // methods
    int FDirectory::size()
    {
        int size = 0;

        for (auto& p : stdfs::directory_iterator(path))
            size++;

        return size;
    }
}