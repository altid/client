#import "AltidMobilePlugin.h"
#if __has_include(<altid_mobile/altid_mobile-Swift.h>)
#import <altid_mobile/altid_mobile-Swift.h>
#else
// Support project import fallback if the generated compatibility header
// is not copied when this plugin is created as a library.
// https://forums.swift.org/t/swift-static-libraries-dont-copy-generated-objective-c-header/19816
#import "altid_mobile-Swift.h"
#endif

@implementation AltidMobilePlugin
+ (void)registerWithRegistrar:(NSObject<FlutterPluginRegistrar>*)registrar {
  [SwiftAltidMobilePlugin registerWithRegistrar:registrar];
}
@end
