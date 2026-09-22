/*
 * Copyright(C) 2026 Igor Kravchenko <igorkrav@gmail.com>
 * 
 * Licensed under the MIT License. 
 * See the LICENSE file in the project root for full license information.
 */

#pragma once
#include <vector>
#include <string>
#include <filesystem>

typedef std::vector<std::wstring > StringList;

class file_list
{
public:
    file_list() : current_index(0) {}
    virtual ~file_list() {
        //if (files) {
        //    for (int i = 0; i < count; ++i) {
        //        delete[] files[i];
        //    }
        //    delete[] files;
        //}
    }
    bool load_from_directory(const wchar_t* directory);

    bool is_last() const { return current_index >= files.size() - 1; }
    bool is_first() const { return current_index <= 0; }

    const wchar_t* get_file(int idx) const {
        if (idx < 0 || idx >= files.size()) return nullptr;
        return files[idx].c_str();
    }
    const wchar_t* get_current() const {
        return get_file(current_index);
    }
    const std::wstring get_file_name(int idx) const {
        if (idx < 0 || idx >= files.size()) return std::wstring();
        std::filesystem::path filePath = files[idx].c_str();
        return filePath.filename().wstring();
    }
    const std::wstring get_current_name() const {
        return get_file_name(current_index);
    }
    int get_count() const { return static_cast<int>( files.size()); }
    const int get_current_index() const { return current_index; }
    const int get_total_files() const { return static_cast<int>(files.size()); }

    bool next() {
        if (current_index < files.size() - 1)
        {
            current_index++;
            return true;
        }
        return false;
    }
    bool prev() {
        if (current_index > 0) {
            current_index--;
            return true;
        }
        return false;
    }

protected:
    std::wstring m_path;
    StringList files;
    int current_index = 0;
};
