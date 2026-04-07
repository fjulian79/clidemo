# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [4.1.0] - 2026-03-07

### Added
- libCli unit tests

### Changed
- Updated to libcli v4.4.0
- Monitor filter configuration with "direct" for multiple environments
- Documentation updates and formatting improvements

### Fixed
- Function declaration for TelnetServer::info
- Whitespace issues cleanup

## [4.0.0] - 2025-02-08

### Added
- Telnet server support with optional configuration

### Changed
- Adopted to libcli v4.0.0
- Improved initialization of Serial()

### Fixed
- Monitor EOL configuration removed (was causing issues)

## [3.1.0] - 2024-09-12

### Added
- Raspberry Pi Pico board support

## [3.0.0] - 2023-01-25

### Added
- Support for more target platforms
- New library features integration

### Changed
- Updated to libcli v3.0.0
- Adopted to recent changes in libgeneric and platformio

### Removed
- Old version script

## [2.0.0] - 2021-05-24

### Added
- NodeMCU environment configuration
- git-version.py script to generate version header
- Generic code support for both STM32 and ESP8266

### Changed
- Code made generic across multiple platforms

### Removed
- Git submodules

## [1.0.0] - 2020-05-01

### Added
- Arduino framework support
- Command to test arguments
- Help command functionality
- JLink debug configuration

### Changed
- Converted to Arduino framework
- Updated to new libcli version and API
- Migrated to PlatformIO project structure
- Now using BSP for hardware access

### Fixed
- Various bug fixes for functionality
- Type fixes

## [0.1.0] - 2019-04-14

### Added
- Initial commit with Eclipse project
- Basic CLI demo functionality
- Configuration command (cfg)
- Support for Nucleo STM32F103RB eval board

[Unreleased]: https://github.com/fjulian79/clidemo/compare/v4.0.0...HEAD
[4.0.0]: https://github.com/fjulian79/clidemo/compare/v3.1.0...v4.0.0
[3.1.0]: https://github.com/fjulian79/clidemo/compare/v3.0.0...v3.1.0
[3.0.0]: https://github.com/fjulian79/clidemo/compare/v2.0.0...v3.0.0
[2.0.0]: https://github.com/fjulian79/clidemo/compare/v1.0.0...v2.0.0
[1.0.0]: https://github.com/fjulian79/clidemo/compare/v0.1.0...v1.0.0
[0.1.0]: https://github.com/fjulian79/clidemo/releases/tag/v0.1.0
