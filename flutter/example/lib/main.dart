import 'package:flutter/material.dart';
import 'dart:async';

import 'package:flutter/services.dart';
import 'package:altid_mobile/altid_mobile.dart';

void main() => runApp(MyApp());

class MyApp extends StatefulWidget {
  @override
  _MyAppState createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  String _titleData = 'Unknown';
  String _document = 'Unknown';

  @override
  void initState() {
    super.initState();
    initConnection();
    initDocument();
    initTitle();
  }

  Future<void> initConnection() async {
    try {
      await AltidMobile.altidStart("192.168.0.116");
    } on PlatformException {
    }

    try {
      await AltidMobile.altidAttach;
    } on PlatformException {}
  }

  Future<void> initDocument() async {
    String document;
    try {
      document = await AltidMobile.altidDocument;
    } on PlatformException {
      document = 'Failed to read document.';
    }

    setState(() {
      _document = document;
    });
  }

  Future<void> initTitle() async {
    String title;
    try {
      title = await AltidMobile.altidTitle;
    } on PlatformException {
      title = 'Failed to read title.';
    }

    setState(() {
      _titleData = title;
    });
  }


  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: Text(_titleData),
        ),
        body: Center(
          child: Text(_document),
        ),
      ),
    );
  }
}
