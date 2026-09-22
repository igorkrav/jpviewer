/*
 * Copyright(C) 2026 Igor Kravchenko <igorkrav@gmail.com>
 * 
 * Licensed under the MIT License. 
 * See the LICENSE file in the project root for full license information.
 */

#include "file_list.h"
#include <string>
#include <iostream>
#include <cstring>
#include <algorithm>

#if __has_include(<filesystem>)
    #include <filesystem>
    namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
    #include <experimental/filesystem>
    namespace fs = std::experimental::filesystem;
#else
    #error "Missing the <filesystem> header."
#endif

static bool caseInsensitiveCompare(const std::wstring& str1, const std::wstring& str2)
{
    if (str1.length() != str2.length()) {
        return false;
    }
    return std::equal(str1.begin(), str1.end(), str2.begin(),
        [](wchar_t c1, wchar_t c2) {
            return std::tolower(c1) == std::tolower(c2);
        });
}

static bool caseInsensitiveCompare(const std::wstring& str1, const wchar_t* str2)
{
    return caseInsensitiveCompare(str1, std::wstring(str2));
}

static bool caseInsensitiveCompare(const wchar_t* str1, const wchar_t* str2)
{
    return caseInsensitiveCompare(std::wstring(str1), std::wstring(str2));
}

bool file_list::load_from_directory(const wchar_t* directory)
{
    std::filesystem::path dirPath;
    std::wstring name;

    if (fs::is_directory(directory)) {
        dirPath = directory;
    }
    else
    {
        if (fs::is_regular_file(directory)) {

            std::filesystem::path filePath = directory;
            dirPath = filePath.parent_path();
            name = directory;
            if (dirPath.empty()) {
                dirPath = L".";
            }
        }
        else {
            std::wcerr << L"Error: " << directory << L" path doesn't exist." << std::endl;
            return false;
        }
    }

    files.clear();
    for (const auto& entry : fs::directory_iterator(dirPath)) {
        if (entry.is_regular_file()) {
            std::wstring ext = entry.path().extension().wstring();
            if (
                caseInsensitiveCompare(ext, L".jxl") ||
                caseInsensitiveCompare(ext, L".jpg") ||
                caseInsensitiveCompare(ext, L".jpeg")
                )
            {
                try {
                    files.push_back(entry.path().wstring());
                }
                catch (const std::exception& e) {
                    // Handle the conversion failure
                    (void)e;
                }
            }
        }
        // std::wcout << entry.path().wstring().c_str() << std::endl;
    }

    std::sort(files.begin(), files.end());

    if (!name.empty()) {
        auto it = std::find(files.begin(), files.end(), name);

        // 2. Check if the element was actually found
        if (it != files.end()) {
            // 3. Subtract vec.begin() to get the 0-based index
            current_index = static_cast<int>( it - files.begin());
        }
    }
    else {
        current_index = 0;
    }

    return true;
}
