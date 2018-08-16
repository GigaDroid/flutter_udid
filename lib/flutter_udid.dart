import 'dart:async';

import 'package:flutter/services.dart';

class FlutterUdid {
  static const MethodChannel _channel =
      const MethodChannel('flutter_udid');

  static Future<String> get udid async {
    final String udid = await _channel.invokeMethod('getUDID');
    return udid;
  }
}
