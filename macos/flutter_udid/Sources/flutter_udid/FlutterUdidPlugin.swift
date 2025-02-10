import Cocoa
import FlutterMacOS
import IOKit

public class FlutterUdidPlugin: NSObject, FlutterPlugin {
  public static func register(with registrar: FlutterPluginRegistrar) {
    let channel = FlutterMethodChannel(name: "flutter_udid", binaryMessenger: registrar.messenger)
    let instance = FlutterUdidPlugin()
    registrar.addMethodCallDelegate(instance, channel: channel)
  }

  public func handle(_ call: FlutterMethodCall, result: @escaping FlutterResult) {
    switch call.method {
    case "getUDID":
      self.getUniqueDeviceIdentifierAsString(result: result);
    default:
      result(FlutterMethodNotImplemented)
    }
  }

  private func getUniqueDeviceIdentifierAsString(result: FlutterResult) {
        let applicationUUID = self.hardwareUUID()
          if(applicationUUID==nil||applicationUUID==""){
              result(FlutterError.init(code: "UNAVAILABLE",
                                      message: "UDID not available",
                                      details: nil));
          }else{
              result(applicationUUID)
          }
  }

  private func hardwareUUID() -> String?
  {
      let matchingDict = IOServiceMatching("IOPlatformExpertDevice")
      let platformExpert = IOServiceGetMatchingService(kIOMasterPortDefault, matchingDict)
      defer{ IOObjectRelease(platformExpert) }

      guard platformExpert != 0 else { return nil }
      return IORegistryEntryCreateCFProperty(platformExpert, kIOPlatformUUIDKey as CFString, kCFAllocatorDefault, 0).takeRetainedValue() as? String
  }
}
