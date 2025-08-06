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
        let vendorId = UIDevice.current.identifierForVendor?.uuidString ?? UUID().uuidString
        let key = "device_id_\(vendorId)"
        
        let keychain = Keychain(service: Bundle.main.bundleIdentifier ?? "com.default.app")
            .synchronizable(true)
        
        if let uuid = try? keychain.get(key), !uuid.isEmpty {
            result(uuid)
            return
        }
        
        do {
            try keychain.set(vendorId, key: key)
            result(vendorId)
        } catch let error {
            print("Keychain save error: \(error)")
            result(FlutterError(code: "UNAVAILABLE", message: "Failed to save UUID", details: error.localizedDescription))
        }
    }
}
