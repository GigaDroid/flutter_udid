#
# To learn more about a Podspec see http://guides.cocoapods.org/syntax/podspec.html.
# Run `pod lib lint flutter_udid.podspec` to validate before publishing.
#
Pod::Spec.new do |s|
  s.name             = 'flutter_udid'
  s.version          = '0.0.1'
  s.summary          = 'A new flutter plugin project.'
  s.description      = <<-DESC
A new flutter plugin project.
                       DESC
  s.homepage         = 'http://example.com'
  s.license          = { :file => '../LICENSE' }
  s.author           = { 'Your Company' => 'email@example.com' }
  s.source           = { :path => '.' }
  s.source_files     = 'flutter_udid/Sources/flutter_udid/**/*.swift'
  s.dependency 'FlutterMacOS'
  # fix: warning: 'SecKeychainItemDelete' is deprecated: first deprecated in macOS 10.10 - SecKeychain is deprecated [-Wdeprecated-declarations]
  # s.dependency 'SAMKeychain'
  # fix: warning: The macOS deployment target 'MACOSX_DEPLOYMENT_TARGET' is set to 10.11, but the range of supported deployment target versions is 10.13 to 13.1.99. (in target 'SAMKeychain' from project 'Pods')  
  s.platform = :osx, '10.15'
  s.pod_target_xcconfig = { 'DEFINES_MODULE' => 'YES' }
  s.swift_version = '5.0'
end
