#include "include/flutter_udid/flutter_udid_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>


#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <map>
#include <memory>
#include <sstream>

#define _WIN32_DCOM
#include <iostream>
#include <comdef.h>
#include <Wbemidl.h>

#pragma comment(lib, "wbemuuid.lib")

namespace {

	class FlutterUdidPlugin : public flutter::Plugin {
	public:
		static void RegisterWithRegistrar(flutter::PluginRegistrarWindows* registrar);

		FlutterUdidPlugin();

		virtual ~FlutterUdidPlugin();

	private:
		// Called when a method is called on this plugin's channel from Dart.
		void HandleMethodCall(
			const flutter::MethodCall<flutter::EncodableValue>& method_call,
			std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
	};

	// static
	void FlutterUdidPlugin::RegisterWithRegistrar(
		flutter::PluginRegistrarWindows* registrar) {
		auto channel =
			std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
				registrar->messenger(), "flutter_udid",
				&flutter::StandardMethodCodec::GetInstance());

		auto plugin = std::make_unique<FlutterUdidPlugin>();

		channel->SetMethodCallHandler(
			[plugin_pointer = plugin.get()](const auto& call, auto result) {
			plugin_pointer->HandleMethodCall(call, std::move(result));
		});

		registrar->AddPlugin(std::move(plugin));
	}

	FlutterUdidPlugin::FlutterUdidPlugin() {}

	FlutterUdidPlugin::~FlutterUdidPlugin() {}

	void FlutterUdidPlugin::HandleMethodCall(
		const flutter::MethodCall<flutter::EncodableValue>& method_call,
		std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
		if (method_call.method_name().compare("getUDID") == 0) {
			std::ostringstream version_stream;
			HRESULT hres;

			// Initialize COM.

			hres = CoInitializeEx(0, COINIT_MULTITHREADED);

			// Set general COM security levels

			hres = CoInitializeSecurity(
				NULL,
				-1,                          // COM authentication
				NULL,                        // Authentication services
				NULL,                        // Reserved
				RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication
				RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation
				NULL,                        // Authentication info
				EOAC_NONE,                   // Additional capabilities
				NULL                         // Reserved
			);


			if (FAILED(hres))
			{
				CoUninitialize();
				result->Error("0", "Failed to initialize security.");
				return; // Program has failed.
			}

			// Obtain the initial locator to WMI

			IWbemLocator* pLoc = NULL;

			hres = CoCreateInstance(
				CLSID_WbemLocator,
				0,
				CLSCTX_INPROC_SERVER,
				IID_IWbemLocator, (LPVOID*)&pLoc);

			if (FAILED(hres))
			{
				CoUninitialize();              // Program has failed.
				result->Error("0", "Failed to create IWbemLocator object.");
				return; // Program has failed.
			}


			// Connect to WMI through the IWbemLocator::ConnectServer method

			IWbemServices* pSvc = NULL;

			// Connect to the root\cimv2 namespace with
			// the current user and obtain pointer pSvc
			// to make IWbemServices calls.
			hres = pLoc->ConnectServer(
				_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
				NULL,                    // User name. NULL = current user
				NULL,                    // User password. NULL = current
				0,                       // Locale. NULL indicates current
				NULL,                    // Security flags.
				0,                       // Authority (for example, Kerberos)
				0,                       // Context object
				&pSvc                    // pointer to IWbemServices proxy
			);

			if (FAILED(hres))
			{
				pLoc->Release();
				CoUninitialize();
				result->Error("0", "Could not connect.");
				return; // Program has failed.
			}


			// Set security levels on the proxy

			hres = CoSetProxyBlanket(
				pSvc,                        // Indicates the proxy to set
				RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
				RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
				NULL,                        // Server principal name
				RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx
				RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
				NULL,                        // client identity
				EOAC_NONE                    // proxy capabilities
			);

			if (FAILED(hres))
			{
				pSvc->Release();
				pLoc->Release();
				CoUninitialize();
				result->Error("0", "Could not set proxy blanket.");
				return; // Program has failed.
			}

			// Step 6:
			// Use the IWbemServices pointer to make requests of WMI


			IEnumWbemClassObject* pEnumerator = NULL;
			hres = pSvc->ExecQuery(
				bstr_t("WQL"),
				bstr_t("SELECT * FROM Win32_ComputerSystemProduct"),
				WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
				NULL,
				&pEnumerator);

			if (FAILED(hres))
			{
				pSvc->Release();
				pLoc->Release();
				CoUninitialize();
				//
				result->Error("0", "Query for UUID failed.");
				return; // Program has failed.
			}

			// Step 7:
			// Get the data from the query in step 6

			IWbemClassObject* pclsObj = NULL;
			ULONG uReturn = 0;

			while (pEnumerator)
			{
				HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
					&pclsObj, &uReturn);

				if (0 == uReturn)
				{
					break;
				}

				VARIANT vtProp;
				int cp = CP_UTF8;
				std::string dst;
				// Get the value of the UUID property
				hr = pclsObj->Get(L"UUID", 0, &vtProp, 0, 0);
				// request content length in single-chars through a terminating
				//  nullchar in the BSTR. note: BSTR's support imbedded nullchars,
				//  so this will only convert through the first nullchar.
				int res = WideCharToMultiByte(cp, 0, vtProp.bstrVal, -1, NULL, 0, NULL, NULL);
				if (res > 0)
				{
					dst.resize(res);
					WideCharToMultiByte(cp, 0, vtProp.bstrVal, -1, &dst[0], res, NULL, NULL);
				}
				else
				{    // no content. clear target
					dst.clear();
				}
				version_stream << dst;
				VariantClear(&vtProp);
				pclsObj->Release();
			}

			// Cleanup
			// ========

			pSvc->Release();
			pLoc->Release();
			pEnumerator->Release();
			CoUninitialize();
			result->Success(flutter::EncodableValue(version_stream.str()));
		}
		else {
			result->NotImplemented();
		}
	}
}  // namespace

void FlutterUdidPluginRegisterWithRegistrar(
	FlutterDesktopPluginRegistrarRef registrar) {
	FlutterUdidPlugin::RegisterWithRegistrar(
		flutter::PluginRegistrarManager::GetInstance()
		->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
