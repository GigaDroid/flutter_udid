#include "flutter_udid_plugin.h"

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <string>

#include "wmi_utils.h"

namespace flutter_udid
{

  // global cache
  std::string deviceId = "";
  
  // static
  void FlutterUdidPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarWindows *registrar)
  {
    auto channel =
        std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
            registrar->messenger(), "flutter_udid",
            &flutter::StandardMethodCodec::GetInstance());

    auto plugin = std::make_unique<FlutterUdidPlugin>();

    channel->SetMethodCallHandler(
        [plugin_pointer = plugin.get()](const auto &call, auto result)
        {
          plugin_pointer->HandleMethodCall(call, std::move(result));
        });

    registrar->AddPlugin(std::move(plugin));
  }

  FlutterUdidPlugin::FlutterUdidPlugin() {}

  FlutterUdidPlugin::~FlutterUdidPlugin() {}

  void FlutterUdidPlugin::HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result)
  {
    if (method_call.method_name().compare("getUDID") == 0)
    {
      if(! deviceId.empty()){ // return cached deviceId if available
        return result->Success(flutter::EncodableValue(deviceId));
      }
      // Query WMI directly because the deprecated wmic.exe command-line tool
      // is optional or unavailable on newer Windows installations.
      const std::string serveName = "ROOT\\CIMV2";
      const std::string tableName = "Win32_ComputerSystemProduct";
      const std::string fieldName = "UUID";

      const bool initialized = WmiUtils::wmiInit(serveName);
      if (!initialized)
      {
        result->Error("UNABLE_TO_INIT_WMI", "Unable to initialize WMI");
        return;
      }

      std::wstring uuid = WmiUtils::wmiValue(tableName, fieldName);
      WmiUtils::wmiRelease();
      if (uuid.empty())
      {
        result->Error("NO_DATA_READ", "No data read from wmi instance");
        return;
      }
      deviceId = WmiUtils::wstring_to_string(uuid);
      result->Success(flutter::EncodableValue(deviceId));
    }
    else
    {
      result->NotImplemented();
    }
  }

  

} // namespace flutter_udid
