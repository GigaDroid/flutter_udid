import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:flutter_udid/flutter_udid.dart';

void main() {
  const MethodChannel channel = MethodChannel('flutter_udid');

  TestWidgetsFlutterBinding.ensureInitialized();

  setUp(() {
    channel.setMockMethodCallHandler((MethodCall methodCall) async {
      return '42';
    });
  });

  tearDown(() {
    channel.setMockMethodCallHandler(null);
  });

  test('getUDID', () async {
    expect(await FlutterUdid.udid, '42');
  });

  test('getConsistentUDID', () async {
    expect(await FlutterUdid.consistentUdid, '73475cb40a568e8da8a045ced110137e159f890ac4da883b6b17dc651b3a8049');
  });
}
