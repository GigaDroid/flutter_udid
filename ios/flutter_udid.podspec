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
  s.source_files = 'Classes/**/*'
  s.public_header_files = 'Classes/**/*.h'
  s.dependency 'Flutter'
  s.dependency 'SAMKeychain'
  s.ios.deployment_target = '8.0'
  s.swift_versions = ['4.0', '4.2', '5.0']
  s.pod_target_xcconfig = { 'DEFINES_MODULE' => 'YES' }
end

