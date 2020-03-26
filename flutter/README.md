# altid_mobile

Flutter library to connect to Altid's 9p servers

## Getting Started

You must call functions in the correct order:

```dart
AltidMobile.altidStart(ipaddress);
// Optionally, you can call altidAuth() but it has not been implemented
AltidMobile.altidAttach();
[...]
```