import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:altid_mobile/altid_mobile.dart';

void main() {
  const MethodChannel channel = MethodChannel('altid_mobile');

  TestWidgetsFlutterBinding.ensureInitialized();

  setUp(() {
    channel.setMockMethodCallHandler((MethodCall methodCall) async {
      return '42';
    });
  });

  tearDown(() {
    channel.setMockMethodCallHandler(null);
  });

  test('getPlatformVersion', () async {
    expect(await AltidMobile.platformVersion, '42');
  });

  test('altidStart', () async {
    expect(await AltidMobile.altidStart("127.0.0.1"), null);
    expect(await AltidMobile.altidAttach, null);
  });
}
