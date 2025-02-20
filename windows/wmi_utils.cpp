// wmi_utils.cpp
#include "wmi_utils.h"

/*
// for example, get the UUID of the system
int main() {

    std::string serveName = "ROOT\\CIMV2";
    bool initialized = WmiUtils::wmiInit(serveName);

    std::string tableName = "Win32_ComputerSystemProduct";;
    std::string fieldName = "UUID";
    std::wstring result = WmiUtils::wmiValue(tableName, fieldName); // single field
    std::cout << "tableName:" + tableName + ", fieldName:" + fieldName << std::endl;
    std::cout << WmiUtils::wstring_to_string(result) << std::endl;

    tableName = "Win32_OperatingSystem";
    fieldName = "SerialNumber";
    result = WmiUtils::wmiValue(tableName, fieldName);
    std::cout << "tableName:" + tableName + ", fieldName:" + fieldName << std::endl;
    std::cout << WmiUtils::wstring_to_string(result) << std::endl;

    tableName = "Win32_OperatingSystem";
    fieldName = "RegisteredUser,SerialNumber,Name";
    result = WmiUtils::wmiValues(tableName, fieldName); // multiple fields
    std::cout << "tableName:" + tableName + ", fieldName:" + fieldName << std::endl;
    std::cout << WmiUtils::wstring_to_string(result) << std::endl;

    WmiUtils::wmiRelease();

    return 0;
}
*/

// Define the namespace
namespace WmiUtils
{
IWbemServices *pSvc = NULL;
IWbemLocator *pLoc = NULL;

std::string wstring_to_string(const std::wstring &wstr)
{
    if (wstr.empty())
    {
        return "";
    }
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &str[0], size_needed, NULL, NULL);
    return str;
}

std::wstring string_to_wstring(const std::string &str)
{
    int size_needed =
        MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstr[0],
                        size_needed);
    return wstr;
}

// Helper function to split a string by delimiter
std::vector<std::wstring> _split(const std::wstring &s, wchar_t delimiter)
{
    std::wstringstream ss(s);
    std::wstring item;
    std::vector<std::wstring> elems;
    while (std::getline(ss, item, delimiter))
    {
        elems.push_back(item);
    }
    return elems;
}

// init COM & WMISERVER
HRESULT _init(IWbemServices **pSvc1, IWbemLocator **pLoc1, std::string serveName)
{

    _bstr_t bstrServeName(serveName.c_str());
    HRESULT hres;

    // Step 1: --------------------------------------------------
    // Initialize COM. ------------------------------------------
    hres = CoInitializeEx(0, COINIT_APARTMENTTHREADED); // COINIT_MULTITHREADED
    if (FAILED(hres))
    {
        std::cerr << "Failed to initialize COM library. Error code = 0x" << std::hex << hres << std::endl;
        return hres;
    }

    // Step 2: --------------------------------------------------
    // Set general COM security levels --------------------------
    hres = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT,
                                RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);

    if (SUCCEEDED(hres) || RPC_E_TOO_LATE == hres)
    {
    }
    else
    {
        std::cerr << "Failed to initialize security. Error code = 0x" << std::hex << hres << std::endl;
        CoUninitialize();
        return hres;
    }

    // Step 3: ---------------------------------------------------
    // Obtain the initial locator to WMI -------------------------
    hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
                            IID_IWbemLocator, (LPVOID *)pLoc1);
    if (FAILED(hres))
    {
        std::cerr << "Failed to create IWbemLocator object. Error code = 0x" << std::hex << hres << std::endl;
        CoUninitialize();
        return hres;
    }

    // Step 4: -----------------------------------------------------
    // Connect to WMI through the IWbemLocator::ConnectServer method
    // Connect to the root\cimv2 namespace with the current user and obtain pointer pSvc to make IWbemServices calls.
    hres = (*pLoc1)->ConnectServer(bstrServeName, NULL, NULL, 0, NULL, 0, 0, pSvc1);
    if (FAILED(hres))
    {
        std::cerr << "Failed to connect to WMI. Error code = 0x" << std::hex << hres << std::endl;
        (*pLoc1)->Release();
        CoUninitialize();
        return hres;
    }

    // Step 5: --------------------------------------------------
    // Set security levels on the proxy -------------------------
    hres = CoSetProxyBlanket(*pSvc1, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
                             RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
    if (FAILED(hres))
    {
        std::cerr << "Failed to set proxy blanket. Error code = 0x" << std::hex << hres << std::endl;
        (*pSvc1)->Release();
        (*pLoc1)->Release();
        CoUninitialize();
        return hres;
    }

    return S_OK;
}

// get rows by one filed
std::vector<std::wstring> _exec(IWbemServices *pSvc1, std::string tableName, std::string fieldName, std::string condition)
{
    std::vector<std::wstring> vecValue; // store values

    _bstr_t bstrFieldName(fieldName.c_str());
    _bstr_t bstrTableName(tableName.c_str());
    _bstr_t bstrCondition(condition.c_str());

    _bstr_t bstrStmt = L"SELECT " + bstrFieldName + " FROM " + bstrTableName + " " + bstrCondition;

    // Use the IWbemServices pointer to make requests of WMI ----
    IEnumWbemClassObject *pEnumerator = NULL;
    HRESULT hres = pSvc1->ExecQuery(
        bstr_t("WQL"),
        bstrStmt,
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL, &pEnumerator);
    if (FAILED(hres))
    {
        std::cerr << "Query for " + tableName + " failed. Error code = 0x" << std::hex << hres << std::endl;
        return vecValue;
    }

    IWbemClassObject *pObj = NULL;
    ULONG uReturn = 0;

    // Step 7: -------------------------------------------------
    // Get the data from the query in step 6 -------------------
    while (pEnumerator)
    {
        hres = pEnumerator->Next(WBEM_INFINITE, 1, &pObj, &uReturn);
        if (0 == uReturn)
        {
            break;
        }

        VARIANT vtProp;
        VariantInit(&vtProp);

        // get value from bstrFieldName
        hres = pObj->Get(bstrFieldName, 0, &vtProp, 0, 0);
        if (FAILED(hres))
        {
            std::cerr << "Failed to get " + bstrFieldName << ", Error code = 0x" << std::hex << hres << std::endl;
        }
        else
        {
            if (vtProp.vt != VT_NULL)
            {
                vecValue.push_back(vtProp.bstrVal); // store value
            }
        }

        VariantClear(&vtProp);
        pObj->Release();
    }

    pEnumerator->Release();
    return vecValue;
}

std::wstring _exec2(IWbemServices *pSvc1,
                    std::string tableName,
                    std::string fieldName,
                    std::string condition)
{
    _bstr_t bstrTableName(tableName.c_str());
    _bstr_t bstrFieldName(fieldName.c_str());
    _bstr_t bstrCondition(condition.c_str());

    _bstr_t bstrStmt = L"SELECT " + bstrFieldName + " FROM " + bstrTableName +
                       " " + bstrCondition;

    IEnumWbemClassObject *pEnumerator = NULL;
    HRESULT hres =
        pSvc1->ExecQuery(bstr_t("WQL"), bstrStmt,
                         WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                         NULL, &pEnumerator);
    if (FAILED(hres))
    {
        std::cerr << "Query for " + tableName + " failed, Error code = 0x" << std::hex << hres << std::endl;
        return L"";
    }

    IWbemClassObject *pObj = NULL;
    ULONG uReturn = 0;

    std::wostringstream result;
    while (pEnumerator)
    {
        hres = pEnumerator->Next(WBEM_INFINITE, 1, &pObj, &uReturn);
        if (0 == uReturn)
        {
            break;
        }

        if (fieldName == "*")
        {
            // Get property names
            SAFEARRAY *pNames = NULL;
            hres = pObj->GetNames(0, WBEM_FLAG_ALWAYS, NULL, &pNames);
            if (FAILED(hres))
            {
                std::cerr << "Failed to GetNames, Error code = 0x" << std::hex << hres << std::endl;
                pObj->Release();
                continue;
            }

            long lLBound = 0;
            long lUBound = 0;
            SafeArrayGetLBound(pNames, 1, &lLBound);
            SafeArrayGetUBound(pNames, 1, &lUBound);

            for (long i = lLBound; i <= lUBound; i++)
            {
                BSTR bstrName = NULL;
                VARIANT vtProp;
                VariantInit(&vtProp);

                hres = SafeArrayGetElement(pNames, &i, &bstrName);
                if (FAILED(hres))
                {
                    std::cerr << "Failed to SafeArrayGetElement, Error code = 0x" << std::hex << hres << std::endl;
                    continue;
                }

                // Get property value
                hres = pObj->Get(bstrName, 0, &vtProp, 0, 0);
                if (FAILED(hres))
                {
                    std::cerr << "Failed to Get, Error code = 0x" << std::hex << hres << std::endl;
                    SysFreeString(bstrName);
                    continue;
                }

                // Display property name and value
                result << bstrName << L": ";
                switch (vtProp.vt)
                {
                case VT_BSTR:
                    result << vtProp.bstrVal << std::endl;
                    break;
                case VT_I4:
                    result << vtProp.lVal << std::endl;
                    break;
                // Add more cases for other data types if needed
                default:
                    result << L"Unknown data type" << std::endl;
                    break;
                }

                SysFreeString(bstrName);
                VariantClear(&vtProp);
            }

            SafeArrayDestroy(pNames);
        }
        else
        {
            // Get specified property values
            std::wstring field =
                string_to_wstring(fieldName);
            std::vector<std::wstring> fieldNames = _split(field, L',');
            for (const auto &elemName : fieldNames)
            {
                _bstr_t bstrElemName(elemName.c_str());

                VARIANT vtProp;
                VariantInit(&vtProp);

                hres = pObj->Get(bstrElemName, 0, &vtProp, 0, 0);
                if (FAILED(hres))
                {
                    std::wcerr << "Failed to get property value for " << elemName
                               << std::endl;
                    result << elemName << L": " << L"" << std::endl;
                    continue;
                }

                // Display property name and value
                result << elemName << L": ";
                switch (vtProp.vt)
                {
                case VT_BSTR:
                    result << vtProp.bstrVal << std::endl;
                    break;
                case VT_I4:
                    result << vtProp.lVal << std::endl;
                    break;
                // Add more cases for other data types if needed
                default:
                    result << L"Unknown data type" << std::endl;
                    break;
                }

                VariantClear(&vtProp);
            }
        }

        pObj->Release();
    }

    pEnumerator->Release();

    return result.str();
}

bool wmiInit(std::string serveName)
{
    if (pSvc || pLoc)
    {
        return true;
    }
    HRESULT hres = WmiUtils::_init(&pSvc, &pLoc, serveName);
    if (FAILED(hres))
    {
        std::cerr << "Failed to initialize COM or WMI." << std::endl;
        return false;
    }
    return true;
}

void wmiRelease()
{
    if (pLoc)
    {
        pLoc->Release();
    }
    if (pSvc)
    {
        pSvc->Release();
        CoUninitialize();
    }

    pSvc = NULL;
    pLoc = NULL;
}

std::wstring wmiValue(std::string tableName, std::string fieldName, std::string condition, std::string delimiter)
{

    // get values
    std::vector<std::wstring> vecVals = _exec(pSvc, tableName, fieldName, condition);

    if (vecVals.empty())
    {
        return L"";
    }

    _bstr_t bstrDelimiter(delimiter.c_str());

    std::wostringstream result;
    for (size_t i = 0; i < vecVals.size(); ++i)
    {
        if (i != 0)
        {
            result << bstrDelimiter;
        }
        result << vecVals[i];
    }

    return result.str();
}

std::wstring wmiValues(std::string tableName, std::string fieldName,
                       std::string condition, std::string delimiter)
{
    // get values
    std::wstring result = _exec2(pSvc, tableName, fieldName, condition);
    return result;
}

} // namespace WmiUtils