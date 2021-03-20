package de.gigadroid.flutter_udid

import androidx.annotation.NonNull
import android.annotation.SuppressLint
import android.provider.Settings
import android.content.Context

import io.flutter.plugin.common.MethodChannel
import io.flutter.plugin.common.MethodChannel.MethodCallHandler
import io.flutter.plugin.common.MethodChannel.Result
import io.flutter.plugin.common.MethodCall
import io.flutter.plugin.common.PluginRegistry.Registrar
import io.flutter.plugin.common.PluginRegistry
import io.flutter.embedding.engine.plugins.FlutterPlugin
import io.flutter.plugin.common.EventChannel
import io.flutter.plugin.common.BinaryMessenger

class FlutterUdidPlugin() : MethodCallHandler, FlutterPlugin {

  private lateinit var channel : MethodChannel
  private var applicationContext: Context? = null

  companion object {
    @JvmStatic
    fun registerWith(registrar: Registrar): Unit {
      val instance = FlutterUdidPlugin()
      instance.onAttachedToEngine(registrar.context(), registrar.messenger())
    }
  }

  override fun onAttachedToEngine(@NonNull flutterPluginBinding: FlutterPlugin.FlutterPluginBinding) {
    onAttachedToEngine(flutterPluginBinding.getApplicationContext(), flutterPluginBinding.getBinaryMessenger());
  }

  private fun onAttachedToEngine(applicationContext : Context, messenger: BinaryMessenger) {
    this.applicationContext = applicationContext;
    channel = MethodChannel(messenger, "flutter_udid")
    channel.setMethodCallHandler(this)
  }

  override fun onMethodCall(@NonNull call: MethodCall, @NonNull result: Result) {
    if(call.method == "getUDID"){
      val udid = getUDID()
      if(udid == null || udid == ""){
        result.error("UNAVAILABLE", "UDID not available.", null)
      }else{
        result.success(udid)
      }
    }else{
      result.notImplemented()
    }
  }

  override fun onDetachedFromEngine(@NonNull binding: FlutterPlugin.FlutterPluginBinding) {
    applicationContext = null;
    channel.setMethodCallHandler(null)
  }

  private fun getUDID() : String{
    return Settings.Secure.getString(applicationContext?.contentResolver, Settings.Secure.ANDROID_ID)
  }
}

