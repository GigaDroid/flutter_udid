#include "include/flutter_udid/flutter_udid_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "flutter_udid_plugin.h"

void FlutterUdidPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  flutter_udid::FlutterUdidPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
