#
# To learn more about a Podspec see http://guides.cocoapods.org/syntax/podspec.html
#
Pod::Spec.new do |s|
  s.name             = 'flutter_udid'
  s.version          = '0.0.1'
  s.summary          = 'Plugin for getting a persistent UDID on iOS and Android'
  s.description      = <<-DESC
Plugin for getting a persistent UDID on iOS and Android
                       DESC
  s.homepage         = 'http://example.com'
  s.license          = { :file => '../LICENSE' }
  s.author           = { 'Your Company' => 'email@example.com' }
  s.source           = { :path => '.' }
  s.source_files = 'flutter_udid/Sources/flutter_udid/**/*.swift'
  s.dependency 'Flutter'
  s.dependency 'SAMKeychain'
  s.dependency 'KeychainAccess'
  s.platform = :ios, '11.0'

  # Flutter.framework does not contain a i386 slice.
  s.pod_target_xcconfig = { 'DEFINES_MODULE' => 'YES', 'EXCLUDED_ARCHS[sdk=iphonesimulator*]' => 'i386' }
  s.swift_version = '5.0'
end

