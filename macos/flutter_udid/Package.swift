// swift-tools-version: 5.9
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
  name: "flutter_udid",
  platforms: [
    .macOS("10.15")
  ],
  products: [
    .library(name: "flutter-udid", targets: ["flutter_udid"])
  ],
  dependencies: [],
  targets: [
    .target(
      name: "flutter_udid",
      dependencies: [],
      resources: []
    )
  ]
)
