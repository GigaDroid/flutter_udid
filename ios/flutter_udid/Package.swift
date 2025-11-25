// swift-tools-version: 5.9
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
  name: "flutter_udid",
  platforms: [
    .iOS("12.0")
  ],
  products: [
    .library(name: "flutter-udid", targets: ["flutter_udid"])
  ],
  dependencies: [
    .package(url: "https://github.com/kishikawakatsumi/KeychainAccess", .upToNextMajor(from: "4.2.2")),
  ],
  targets: [
    .target(
      name: "flutter_udid",
      dependencies: [
        .product(name: "KeychainAccess", package: "KeychainAccess")
      ],
      resources: []
    )
  ]
)
