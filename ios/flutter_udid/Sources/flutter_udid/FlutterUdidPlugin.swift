import Flutter
import UIKit
import KeychainAccess

public class FlutterUdidPlugin: NSObject, FlutterPlugin {

  public static func register(with registrar: FlutterPluginRegistrar) {
    let channel = FlutterMethodChannel(name: "flutter_udid", binaryMessenger: registrar.messenger())
    let instance = FlutterUdidPlugin()
    registrar.addMethodCallDelegate(instance, channel: channel)
  }

  public func handle(_ call: FlutterMethodCall, result: @escaping FlutterResult) {
    switch call.method {
    case "getUDID":
      self.getUniqueDeviceIdentifierAsString(result: result)
    default:
      result(FlutterMethodNotImplemented)
    }
  }

  private func getUniqueDeviceIdentifierAsString(result: FlutterResult) {
    let bundleName = Bundle.main.infoDictionary?["CFBundleName"] as? String ?? "flutter_udid"
    let accountName = Bundle.main.bundleIdentifier ?? "com.default.app"

    // Use KeychainAccess with same structure as original SAMKeychain implementation
    let keychain = Keychain(service: bundleName).synchronizable(false)

    // Try to read existing UUID from keychain
    if let applicationUUID = try? keychain.get(accountName), !applicationUUID.isEmpty {
      result(applicationUUID)
      return
    }

    // Generate new UUID if none exists
    guard let vendorId = UIDevice.current.identifierForVendor?.uuidString else {
      result(FlutterError(code: "UNAVAILABLE",
                         message: "UDID not available",
                         details: nil))
      return
    }

    // Save the new UUID to keychain
    do {
      try keychain.set(vendorId, key: accountName)
      result(vendorId)
    } catch {
      result(FlutterError(code: "UNAVAILABLE",
                         message: "UDID not available",
                         details: nil))
    }
  }
}

