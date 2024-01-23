package de.gigadroid.flutter_udid

import android.content.Context
import android.provider.Settings
import io.flutter.embedding.engine.plugins.FlutterPlugin
import io.flutter.plugin.common.BinaryMessenger
import io.flutter.plugin.common.MethodCall
import io.flutter.plugin.common.MethodChannel
import io.flutter.plugin.common.MethodChannel.MethodCallHandler
import io.flutter.plugin.common.MethodChannel.Result

class FlutterUdidPlugin : MethodCallHandler, FlutterPlugin {

    private lateinit var channel: MethodChannel
    private var applicationContext: Context? = null

    override fun onAttachedToEngine(flutterPluginBinding: FlutterPlugin.FlutterPluginBinding) {
        onAttachedToEngine(
            flutterPluginBinding.getApplicationContext(),
            flutterPluginBinding.getBinaryMessenger()
        );
    }

    private fun onAttachedToEngine(applicationContext: Context, messenger: BinaryMessenger) {
        this.applicationContext = applicationContext;
        channel = MethodChannel(messenger, "flutter_udid")
        channel.setMethodCallHandler(this)
    }

    override fun onMethodCall(call: MethodCall, result: Result) {
        if (call.method == "getUDID") {
            val udid = getUDID()
            if (udid.isNullOrEmpty()) {
                result.error("UNAVAILABLE", "UDID not available.", null)
            } else {
                result.success(udid)
            }
        } else {
            result.notImplemented()
        }
    }

    override fun onDetachedFromEngine(binding: FlutterPlugin.FlutterPluginBinding) {
        applicationContext = null;
        channel.setMethodCallHandler(null)
    }

    private fun getUDID(): String? {
        return Settings.Secure.getString(
            applicationContext?.contentResolver,
            Settings.Secure.ANDROID_ID
        )
    }
}

