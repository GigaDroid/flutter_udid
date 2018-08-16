# flutter_udid

[![pub package](https://img.shields.io/pub/v/flutter_udid.svg)](https://pub.dartlang.org/packages/flutter_udid)

Plugin for getting a persistent UDID accross app reinstalls on iOS and Android.

## Getting Started

```
import 'package:flutter_udid/flutter_udid.dart';
String udid = await FlutterUdid.udid;
```

This provides a UDID for both iOS and Android using the format corresponding platform.

| Platform | Format |
| ------------- | ------------- |
| iOS     | `7946DA4E-8429-423C-B405-B3FC77914E3E` | 
| Android | `8af8770a27cfd182` |

To get a consistent formatting on both platforms use:

```
import 'package:flutter_udid/flutter_udid.dart';
String udid = await FlutterUdid.consistentUdid;
```

This will result in a UDID in the following format:     
`25FD859320F9C05F85243B18EB1D31D4FA88A591023F84AC26CE4F8170A3BBFC`


The UDID can change after a factory reset!
Additionally if a device has been updated to Android 8.0 through an OTA and the app is reinstalled the UDID may change as well.

For help getting started with Flutter, view the online
[documentation](https://flutter.io/).

For help on editing plugin code, view the [documentation](https://flutter.io/developing-packages/#edit-plugin-package).
