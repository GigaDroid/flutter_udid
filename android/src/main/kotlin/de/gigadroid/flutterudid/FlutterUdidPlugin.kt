package de.gigadroid.flutterudid

import android.annotation.SuppressLint
import android.provider.Settings
import io.flutter.plugin.common.MethodChannel
import io.flutter.plugin.common.MethodChannel.MethodCallHandler
import io.flutter.plugin.common.MethodChannel.Result
import io.flutter.plugin.common.MethodCall
import io.flutter.plugin.common.PluginRegistry.Registrar
import io.flutter.plugin.common.PluginRegistry



class FlutterUdidPlugin(private val registrar: PluginRegistry.Registrar) : MethodCallHandler {
  companion object {
    @JvmStatic
    fun registerWith(registrar: Registrar): Unit {
      val channel = MethodChannel(registrar.messenger(), "flutter_udid")
      val instance = FlutterUdidPlugin(registrar)
      channel.setMethodCallHandler(instance)
    }
  }

  override fun onMethodCall(call: MethodCall, result: Result): Unit {
    if(call.method == "getUDID"){
      val udid = getUDID()
      if(udid == ""){
        result.error("UNAVAILABLE", "UDID not available.", null)
      }else{
        result.success(udid)
      }
    }else{
      result.notImplemented()
    }
  }

  private fun getUDID() : String{
    return Settings.Secure.getString(registrar.context().contentResolver, Settings.Secure.ANDROID_ID)
  }
}

