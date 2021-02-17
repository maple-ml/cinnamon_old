#include "Directory.h"

#include <iostream>
#include <utility>
#include <filesystem>

namespace stdfs = std::filesystem;

namespace cinnamon::fs {
    // constructors
    Directory::Directory()
        = default;

    Directory::Directory(std::string path)
    {
        stdfs::create_directory(path);
    }

    // constructor functions
    std::string Directory::get_path()
    {
        return path;
    }

    void Directory::set_path(std::string path)
    {
        this->path = std::move(path);
    }

    Directory* Directory::create(std::string path)
    {
        return new Directory(path);
    }

    // methods
    int Directory::size()
    {
        int size = 0;

        for (auto& p : stdfs::directory_iterator(path))
            size++;

        return size;
    }

    std::filesystem::directory_iterator Directory::files()
    {
        return stdfs::directory_iterator(path);
    }
}