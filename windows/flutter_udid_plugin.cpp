#include "flutter_udid_plugin.h"

#include <windows.h>
#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <sstream>

namespace flutter_udid
{
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
      // Inspired by https://github.com/BestBurning/platform_device_id
      char buf[1024] = {0};
      FILE *rstream = _popen("wmic csproduct get UUID", "r");

      if (rstream == NULL)
      {
        result->Error("UNABLE_TO_OPEN_STREAM", "Unable to open stream for wmic command");
        return;
      }

      size_t bytesRead = fread(buf, sizeof(char), sizeof(buf), rstream);
      _pclose(rstream);

      if (bytesRead == 0)
      {
        result->Error("NO_DATA_READ", "No data read from wmic command");
        return;
      }

      std::string deviceIdResult = buf;
      std::size_t pos = deviceIdResult.find("\n");

      if (pos == std::string::npos)
      {
        result->Error("INVALID_UUID_FORMAT", "UUID format is invalid");
        return;
      }

      std::string deviceId = deviceIdResult.substr(pos + 1);
      result->Success(flutter::EncodableValue(deviceId));
    }
    else
    {
      result->NotImplemented();
    }
  }

} // namespace flutter_udid
