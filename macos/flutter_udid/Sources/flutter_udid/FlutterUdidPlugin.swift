import Cocoa
import FlutterMacOS
import IOKit
import KeychainAccess

public class FlutterUdidPlugin: NSObject, FlutterPlugin {
  private static let keychainService = "de.gigadroid.flutter_udid"
  private static let keychainAccount = "udid"

  public static func register(with registrar: FlutterPluginRegistrar) {
    let channel = FlutterMethodChannel(name: "flutter_udid", binaryMessenger: registrar.messenger)
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
    let keychain = Keychain(service: FlutterUdidPlugin.keychainService)
      .synchronizable(false)

    do {
      if let applicationUUID = try keychain.get(FlutterUdidPlugin.keychainAccount), !applicationUUID.isEmpty {
        result(applicationUUID)
        return
      }
    } catch {
      result(FlutterError(code: "UNAVAILABLE",
                         message: "UDID not available",
                         details: "keychain_get_failed: \(error.localizedDescription)"))
      return
    }

    do {
      if let migratedUUID = try self.migratedUniqueDeviceIdentifier(keychain: keychain) {
        result(migratedUUID)
        return
      }
    } catch {
      result(FlutterError(code: "UNAVAILABLE",
                         message: "UDID not available",
                         details: "keychain_migration_failed: \(error.localizedDescription)"))
      return
    }

    guard let hardwareUUID = self.hardwareUUID(), !hardwareUUID.isEmpty else {
      result(FlutterError(code: "UNAVAILABLE",
                         message: "UDID not available",
                         details: "hardwareUUID returned nil or empty"))
      return
    }

    do {
      try keychain.set(hardwareUUID, key: FlutterUdidPlugin.keychainAccount)
      result(hardwareUUID)
    } catch {
      result(FlutterError(code: "UNAVAILABLE",
                         message: "UDID not available",
                         details: "keychain_set_failed: \(error.localizedDescription)"))
    }
  }

  private func migratedUniqueDeviceIdentifier(keychain: Keychain) throws -> String? {
    let legacyService = Bundle.main.infoDictionary?["CFBundleName"] as? String ?? "flutter_udid"
    let legacyAccount = Bundle.main.bundleIdentifier ?? "com.default.app"
    let legacyKeychain = Keychain(service: legacyService)
      .synchronizable(false)

    guard let legacyUUID = try legacyKeychain.get(legacyAccount), !legacyUUID.isEmpty else {
      return nil
    }

    try keychain.set(legacyUUID, key: FlutterUdidPlugin.keychainAccount)
    return legacyUUID
  }

  private func hardwareUUID() -> String? {
    let matchingDict = IOServiceMatching("IOPlatformExpertDevice")
    let platformExpert = IOServiceGetMatchingService(kIOMasterPortDefault, matchingDict)
    defer { IOObjectRelease(platformExpert) }

    guard platformExpert != 0 else { return nil }
    return IORegistryEntryCreateCFProperty(platformExpert, kIOPlatformUUIDKey as CFString, kCFAllocatorDefault, 0).takeRetainedValue() as? String
  }
}
