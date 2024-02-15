#ifndef FLUTTER_PLUGIN_FLUTTER_UDID_PLUGIN_H_
#define FLUTTER_PLUGIN_FLUTTER_UDID_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>

#include <memory>

namespace flutter_udid {

class FlutterUdidPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  FlutterUdidPlugin();

  virtual ~FlutterUdidPlugin();

  // Disallow copy and assign.
  FlutterUdidPlugin(const FlutterUdidPlugin&) = delete;
  FlutterUdidPlugin& operator=(const FlutterUdidPlugin&) = delete;

  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
};

}  // namespace flutter_udid

#endif  // FLUTTER_PLUGIN_FLUTTER_UDID_PLUGIN_H_
