# flutter_udid

[![pub package](https://img.shields.io/pub/v/flutter_udid.svg)](https://pub.dartlang.org/packages/flutter_udid)

Plugin to retrieve a persistent UDID across app reinstalls on iOS, Android, Mac, Windows & Linux.

## Getting Started

```
import 'package:flutter_udid/flutter_udid.dart';
String udid = await FlutterUdid.udid;
```

This provides an UDID using the format of the corresponding platform.

| Platform | Format                                 | Source                                                                                                                                          |
|----------|----------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------|
| iOS      | `7946DA4E-8429-423C-B405-B3FC77914E3E` | [identifierForVendor (saved to Keychain for persistence)](https://developer.apple.com/documentation/uikit/uidevice/1620059-identifierforvendor) |
| Android  | `8af8770a27cfd182`                     | [Settings.Secure.ANDROID_ID](https://developer.android.com/reference/android/provider/Settings.Secure#ANDROID_ID)                               |
| Mac      | `707E990C-D002-520B-ABA6-4216C6D514BF` | [kIOPlatformUUIDKey](https://developer.apple.com/documentation/iokit/kioplatformuuidkey)                                                        |
| Windows  | `99A4D301-53F5-11CB-8CA0-9CA39A9E1F01` | BIOS UUID                                                                                                                                       |
| Linux    | `1124435e065241dcae2241e9caab9a6c` | Machine ID                                                                                                                                      |

To get a consistent formatting on all platforms use:

```
import 'package:flutter_udid/flutter_udid.dart';
String udid = await FlutterUdid.consistentUdid;
```

This will result in an UDID of the following format:  
`984725b6c4f55963cc52fca0f943f9a8060b1c71900d542c79669b6dc718a64b`

The UDID can change after a factory reset!
Additionally if a device has been updated to Android 8.0 through an OTA and the app is reinstalled the UDID may change as well due to security changes in Android 8.0.
On rooted and jailbroken devices the ID can be changed, so please take this into account. However, it should not be possible to identify as a different device through random guessing because of the complexity of the ID.
Furthermore, the UDID may also change if there is a change in the app's signing signature, for both iOS and Android. Ensure that you always use the same digital signature to sign your app.

For help getting started with Flutter, view the online
[documentation](https://flutter.io/).

For help on editing plugin code, view the [documentation](https://flutter.io/developing-packages/#edit-plugin-package).
