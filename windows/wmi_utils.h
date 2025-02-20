// wmi_utils.h
#ifndef WMI_UTILS_H
#define WMI_UTILS_H

#define _WIN32_DCOM

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <Wbemidl.h>
#include <comutil.h>
#include <windows.h>

#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "comsuppw.lib")

namespace WmiUtils
{

// globally initialized COM objects
extern IWbemServices *pSvc;
extern IWbemLocator *pLoc;

// Function prototypes
bool wmiInit(std::string serveName);
void wmiRelease();
std::wstring wmiValue(std::string tableName, std::string fieldName,
                      std::string condition = "", std::string delimiter = ",");
std::wstring wmiValues(std::string tableName, std::string fieldName,
                       std::string condition = "", std::string delimiter = ",");

std::string wstring_to_string(const std::wstring &wstr);
std::wstring string_to_wstring(const std::string &str);

std::vector<std::wstring> _split(const std::wstring &s, wchar_t delimiter);

HRESULT _init(IWbemServices **pSvc, IWbemLocator **pLoc, std::string serveName);
std::vector<std::wstring> _exec(IWbemServices *pSvc, std::string tableName,
                                std::string fieldName,
                                std::string condition = "");

} // namespace WmiUtils

#endif // WMI_UTILS_H