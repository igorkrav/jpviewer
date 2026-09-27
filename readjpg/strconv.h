#pragma once
#include <string>
#include <locale>
#include <codecvt>

std::string wstring_to_string(const std::wstring& wstr);
std::string wstring_to_string(const wchar_t* wstr);
std::wstring string_to_wstring(const std::string& str, const std::locale& loc = std::locale());
