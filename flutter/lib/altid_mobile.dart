import 'dart:async';

import 'package:flutter/services.dart';

class AltidMobile {
  static const MethodChannel _channel =
      const MethodChannel('altid_mobile');

  static Future<String> get platformVersion async {
    final String version = await _channel.invokeMethod('getPlatformVersion');
    return version;
  }

  static altidStart(String address) async => await _channel.invokeMethod('altidStart', address);
  static altidBuffer(String buffer) async => await _channel.invokeMethod('altidBuffer', buffer);
  static altidInput(String input) async => await _channel.invokeMethod('altidInput', input);  
  static altidClose(String close) async => await _channel.invokeMethod('altidClose', close); 
  static altidOpen(String open) async => await _channel.invokeMethod('altidOpen', open); 
  static altidLink(String link) async => await _channel.invokeMethod('altidLink', link); 

  static get altidAuth async => await _channel.invokeMethod('altidAuth');
  static get altidAttach async => await _channel.invokeMethod('altidAttach');
  static get altidFeed async => await _channel.invokeMethod('altidFeed');
 
  static Future<String> get altidTitle async {
    final String title = await _channel.invokeMethod('altidTitle');
    return title;
  }

  static Future<String> get altidTabs async {
    final String tabs = await _channel.invokeMethod('altidTabs');
    return tabs;
  }

  static Future<String> get altidStatus async {
    final String status = await _channel.invokeMethod('altidStatus');
    return status;
  }

  static Future<String> get altidDocument async {
    final String document = await _channel.invokeMethod('altidDocument');
    return document;
  }

  static Future<String> get altidAside async {
    final String aside = await _channel.invokeMethod('altidAside');
    return aside;
  }

  static Future<String> get altidNotify async {
    final String notify = await _channel.invokeMethod('altidNotify');
    return notify;
  }

  static Future<ByteData> get altidReadFeed async {
    final ByteData feed = await _channel.invokeMethod('altidReadFeed');
    return feed;
  }

}
