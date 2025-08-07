import Flutter
import UIKit
import KeychainAccess
import SAMKeychain

public class FlutterUdidPlugin: NSObject, FlutterPlugin {
  
  private struct Constants {
    static let channelName = "flutter_udid"
    /// Device ID prefix in Keychain
    static let deviceIdPrefix = "device_id_"
    /// Default Bundle ID (used when unable to get real Bundle ID)
    static let defaultBundleId = "com.default.app"
    /// Default service name for legacy SAMKeychain
    static let defaultServiceName = "flutter_udid"
  }
  
  // MARK: - Error Codes
  /// UDID related error types
  private enum UDIDErrorCode: String {
    /// Unable to get Vendor ID (device not supported or system restrictions)
    case vendorIdUnavailable = "VENDOR_ID_UNAVAILABLE"
    /// Migration from legacy version failed
    case migrationFailed = "MIGRATION_FAILED"
    /// New user UUID save failed
    case newUserSaveFailed = "NEW_USER_SAVE_FAILED"
  }
  
  public static func register(with registrar: FlutterPluginRegistrar) {
    let channel = FlutterMethodChannel(name: Constants.channelName, binaryMessenger: registrar.messenger())
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

  // MARK: - Main Business Logic
  /**
   *
   * Execution flow:
   * 1. Get iOS system's identifierForVendor as foundation
   * 2. Try to read existing UUID from new Keychain first
   * 3. If new version doesn't have it, try to migrate from legacy SAMKeychain
   * 4. If neither has it, create new UUID for new users
   */
  private func getUniqueDeviceIdentifierAsString(result: FlutterResult) {
    // Step 1: Get identifierForVendor
    guard let vendorId = UIDevice.current.identifierForVendor?.uuidString else {
      result(FlutterError(code: UDIDErrorCode.vendorIdUnavailable.rawValue,
                         message: "Vendor ID not available",
                         details: nil))
      return
    }
    
    let newKey = "\(Constants.deviceIdPrefix)\(vendorId)"
    let service = Bundle.main.bundleIdentifier ?? Constants.defaultBundleId
    let newKeychain = Keychain(service: service).synchronizable(true)
    
    // Step 2: Try to read UUID using new logic
    if let uuid = try? newKeychain.get(newKey), !uuid.isEmpty {
      result(uuid)
      return
    }
    
    // Step 3: Try to migrate from legacy SAMKeychain
    if let migratedUUID = migrateFromLegacyKeychain(to: newKeychain, key: newKey) {
      result(migratedUUID)
      return
    }
    
    // Step 4: Create and save UUID for new users
    do {
      try newKeychain.set(vendorId, key: newKey)
      result(vendorId)
    } catch let error {
      result(FlutterError(code: UDIDErrorCode.newUserSaveFailed.rawValue,
                         message: "Failed to save new UUID",
                         details: nil))
    }
  }
  
  // MARK: - Helper Methods
  
  /**
   * Migrate UUID from legacy SAMKeychain to new Keychain
   *
   * Migration flow:
   * 1. Try to read UUID from legacy SAMKeychain
   * 2. If found, save it to new Keychain
   * 3. After successful save, clean up legacy data to avoid redundancy
   * 4. Even if migration fails, return the found legacy UUID
   *
   * @param newKeychain New Keychain instance
   * @param key Key for new storage
   * @return Migrated UUID, or nil if no legacy data exists
   */
  private func migrateFromLegacyKeychain(to newKeychain: Keychain, key: String) -> String? {
    // Build legacy SAMKeychain
    let legacyService = Bundle.main.infoDictionary?["CFBundleName"] as? String ?? Constants.defaultServiceName
    let legacyAccount = Bundle.main.bundleIdentifier ?? Constants.defaultBundleId
    
    // Try to read UUID from legacy
    guard let legacyUUID = SAMKeychain.password(forService: legacyService, account: legacyAccount),
          !legacyUUID.isEmpty else {
      return nil
    }
    
    // Execute migration operation
    do {
      // Save legacy UUID to new Keychain
      try newKeychain.set(legacyUUID, key: key)
      
      // Clean up legacy data
      SAMKeychain.deletePassword(forService: legacyService, account: legacyAccount)
      
    } catch let error {
      // Even if migration fails, it doesn't affect this return result
      // Will try migration again on next startup
    }
    return legacyUUID
  }
}

