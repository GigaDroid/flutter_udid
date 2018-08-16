#import "FlutterUdidPlugin.h"
#import <flutter_udid/flutter_udid-Swift.h>

@implementation FlutterUdidPlugin
+ (void)registerWithRegistrar:(NSObject<FlutterPluginRegistrar>*)registrar {
  [SwiftFlutterUdidPlugin registerWithRegistrar:registrar];
}
@end
