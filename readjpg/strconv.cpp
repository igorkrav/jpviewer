#include "strconv.h"

std::string wstring_to_string2(const std::wstring& wstr)
{
    if (wstr.empty()) return {};

    // Determine required buffer size
    std::setlocale(LC_ALL, ""); // Set to environment default (e.g., en_US.UTF-8)
    size_t len = wstr.length() * MB_CUR_MAX + 1;
    std::string str(len, '\0');

    size_t res = std::wcstombs(&str[0], wstr.c_str(), len);
    if (res == static_cast<size_t>(-1)) {
        return ""; // Conversion error
    }

    str.resize(res);

    return str;
}

std::string wstring_to_string(const std::wstring& wstr)
{
    return wstring_to_string(wstr.c_str());
}

std::string wstring_to_string(const wchar_t* wstr)
{
    if (std::wcslen(wstr) < 1) return {};

    // Determine required buffer size
    std::setlocale(LC_ALL, ""); // Set to environment default (e.g., en_US.UTF-8)
    size_t len = std::wcslen(wstr) * MB_CUR_MAX + 1;
    std::string str(len, '\0');

    size_t res = std::wcstombs(&str[0], wstr, len);
    if (res == static_cast<size_t>(-1)) {
        return ""; // Conversion error
    }

    str.resize(res);

    return str;
}

std::wstring string_to_wstring(const std::string& str, const std::locale& loc)
{
    if (str.empty()) return std::wstring();

    // Get the ctype facet for the specified locale
    const auto& facet = std::use_facet<std::ctype<wchar_t>>(loc);

    std::wstring result(str.size(), L'\0');
    facet.widen(str.data(), str.data() + str.size(), result.data());

    return result;
}
