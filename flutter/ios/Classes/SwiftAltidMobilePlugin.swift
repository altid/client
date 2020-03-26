import Flutter
import UIKit

import Client

class Singleton: ClientClient {
    static var shared = ClientClient()
}

class Feed: ClientFeedIterator {
    static var data = ClientFeedIterator()
}

public class SwiftAltidMobilePlugin: NSObject, FlutterPlugin {

    public static func register(with registrar: FlutterPluginRegistrar) {
        let channel = FlutterMethodChannel(
            name: "altid_mobile",
            binaryMessenger: registrar.messenger()
        )
        
        let instance = SwiftAltidMobilePlugin()
        registrar.addMethodCallDelegate(
            instance,
            channel: channel
        )
    }
    
    public func handle(_ call: FlutterMethodCall, result: @escaping FlutterResult) {
        switch call.method {
        case "altidStart":
            if let arg = call.arguments as? String {
                if let _client = ClientNewClient(arg) {
                    do {
                        try _client.connect(0)
                    } catch {
                        result(FlutterError.init(
                            code: "CONN_FAIL",
                            message: "Connection failed",
                            details: nil
                        ))
                    }
                    Singleton.shared = _client
                    result(nil)
                } else {
                    result(FlutterError.init(
                        code: "LIB_FAIL",
                        message: "Failed to call client library",
                        details: nil
                    ))
                }
            } else {
                result(FlutterError.init(
                    code: "BAD_ARGS",
                    message: "Wrong argument type",
                    details: nil
                ))
            }
        case "altidAuth":
            do {
                try Singleton.shared.auth()
            } catch {
                result(FlutterError.init(
                    code: "AUTH_FAIL",
                    message: "Authorization failed",
                    details: nil
                ))
            }
        case "altidAttach":
            do {
                try Singleton.shared.attach()
            } catch {
                result(FlutterError.init(
                    code: "ATT_FAIL",
                    message: "Attach failed",
                    details: nil
                ))
            }
        case "altidTitle":
            do {
                try Singleton.shared.title()
            } catch {
                result(FlutterError.init(
                    code: "TITLE",
                    message: "Unable to read title",
                    details: nil
                ))
            }
            
        case "altidTabs":
            do {
                try Singleton.shared.tabs()
            } catch {
                result(FlutterError.init(
                    code: "TABS",
                    message: "Unable to read tabs",
                    details: nil
                ))
            }
        case "altidStatus":
            do {
                try Singleton.shared.status()
            } catch {
                result(FlutterError.init(
                    code: "STATUS",
                    message: "Unable to read status",
                    details: nil
                ))
            }
        case "altidDocument":
            do {
                try Singleton.shared.document()
            } catch {
                result(FlutterError.init(
                    code: "DOC",
                    message: "Unable to read document",
                    details: nil
                ))
            }
        case "altidAside":
            do {
                try Singleton.shared.aside()
            } catch {
                result(FlutterError.init(
                    code: "ASIDE",
                    message: "Unable to read aside",
                    details: nil
                ))
            }
        case "altidNotify":
            do {
                try Singleton.shared.notifications()
            } catch {
                result(FlutterError.init(
                    code: "NOTIFY",
                    message: "Unable to read notifications",
                    details: nil
                ))
            }
        case "altidInput":
            if let arg = call.arguments as? Data {
                do {
                    try Singleton.shared.input(arg, ret0_: nil)
                } catch {
                    result(FlutterError.init(
                        code: "Input",
                        message: "Unable to write input",
                        details: nil
                    ))
                }
            } else {
                result(FlutterError.init(
                    code: "BAD_ARGS",
                    message: "Wrong argument type",
                    details: nil
                ))
            }
        case "altidBuffer":
            if let arg = call.arguments as? String {
                do {
                    try Singleton.shared.buffer(arg, ret0_: nil)
                } catch {
                    result(FlutterError.init(
                        code: "BUFFER",
                        message: "Unable to switch to buffer",
                        details: nil
                    ))
                }
            } else {
                result(FlutterError.init(
                    code: "BAD_ARGS",
                    message: "Wrong argument type",
                    details: nil
                ))
            }
        case "altidOpen":
            if let arg = call.arguments as? String {
                do {
                    try Singleton.shared.open(arg, ret0_: nil)
                } catch {
                    result(FlutterError.init(
                        code: "OPEN",
                        message: "Unable to open buffer",
                        details: nil
                    ))
                }
            } else {
                result(FlutterError.init(
                    code: "BAD_ARGS",
                    message: "Wrong argument type",
                    details: nil
                ))
            }
        case "altidClose":
            if let arg = call.arguments as? String {
                do {
                    try Singleton.shared.close(arg, ret0_: nil)
                } catch {
                    result(FlutterError.init(
                        code: "CLOSE",
                        message: "Unable to close buffer",
                        details: nil
                    ))
                }
            } else {
                result(FlutterError.init(
                    code: "BAD_ARGS",
                    message: "Wrong argument type",
                    details: nil
                ))
            }
        case "altidLink":
            if let arg = call.arguments as? String {
                do {
                    
                    try Singleton.shared.link(arg, ret0_: nil)
                } catch {
                    result(FlutterError.init(
                        code: "LINK",
                        message: "Unable to switch to buffer",
                        details: nil
                    ))
                }
            } else {
                result(FlutterError.init(
                    code: "BAD_ARGS",
                    message: "Wrong argument type",
                    details: nil
                ))
            }
        case "altidFeed":
            do {
                var _feed: ClientFeedIterator
                try _feed = Singleton.shared.feedIterator()
                Feed.data = _feed
                result(nil)
            } catch {
                result(FlutterError.init(
                    code: "FEED",
                    message: "Failed to get feed",
                    details: nil
                ))
            }
        case "altidReadFeed":
            do {
                var _data: Data
                try _data = Feed.data.next()
                result(_data)
            } catch {
                
                result(FlutterError.init(
                    code: "FEED",
                    message: "Failed to read feed",
                    details: nil
                ))
            }
        case "getPlatformVersion":
            result("iOS " + UIDevice.current.systemVersion)
        default:
            result(FlutterMethodNotImplemented)
        }
    }
}
