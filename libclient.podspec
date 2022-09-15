Pod::Spec.new do |spec|
  spec.name                  = "libclient"
  spec.version               = "0.0.4"
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
    :tag => 'v0.0.4'
  }

  spec.ios.deployment_target  = '12.0'
  spec.vendored_libraries = 'bindings/libclient.dylib'
  spec.osx.deployment_target  = '10.10'
end
