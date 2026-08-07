# PipeDreamer Changelog
All notable changes to PipeDreamer will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## [Unreleased]
### Added
- Added dedicated Windows/macOS/Linux deployment infrastructure under Resources/Deployment, matching the Umsci project's build/package/notarize scripts

### Changed
- Changed submodule JUCE to tag 9.0.0

### Fixed

## [0.4.0] 2026-05-10
### Added
- Added app icon, iOS launchscreen and settings/about UI
- Added JUCE-AppBasics submodule and adopted its CustomLookAndFeel and iOS utilities

### Changed
- Split UI from monolithic MainComponent into dedicated Board-, Queue-, and GameRenderer classes

## [0.3] 2025
### Added
- Initial playable release: board, tile queue, scoring, and difficulty configuration
