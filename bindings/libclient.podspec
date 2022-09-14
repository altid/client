#
#  Be sure to run `pod spec lint libclient.podspec' to ensure this is a
#  valid spec and to remove all comments including this before submitting the spec.
#
#  To learn more about Podspec attributes see https://guides.cocoapods.org/syntax/podspec.html
#  To see working Podspecs in the CocoaPods repo see https://github.com/CocoaPods/Specs/

Pod::Spec.new do |spec|
  spec.name                  = "libclient"
  spec.version               = "0.0.2"
  spec.summary               = "Client helper Library for Altid."
  spec.description           = <<-DESC
  libclient is a helper library for writing Altid clients
                   DESC

  spec.homepage              = "https://gihub.com/altid/client"
  spec.license               = "MIT"
  spec.author                = {
    "halfwit" => "michaelmisch1985@gmail.com"
  }
  spec.public_header_files   = "bindings/libclient.h"
  spec.source                = {
    :git => 'https://github.com/altid/client.git',
    :tag => 'v0.0.2'
  }

  spec.ios.deployment_target  = '12.0'
  spec.vendored_libraries = 'bindings/libclient.dylib'
  spec.osx.deployment_target  = '10.10'
end
