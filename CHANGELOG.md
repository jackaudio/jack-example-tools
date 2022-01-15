# Changelog

## [Unreleased]

### Added

### Changed

### Deleted

## [1]

### Added
- Add legacy [example-clients](https://github.com/jackaudio/example-clients)
  and [tools](https://github.com/jackaudio/tools)
- Add man pages for example-clients and tools from
  [jack2](https://github.com/jackaudio/jack2)
- Add jack2-only example-clients
- Add license files for example-clients and tools and imported zalsa files
- Add meson build system
- Add CI builds against jack1 and jack2 in an Arch Linux container
- Add supported jack version to project description: jack1 (>=0.126.0), jack2
  (>=1.9.20), pipewire-jack (>=0.3.44)
- Add support to compile against different jack implementations based on
  available features and function definitions

### Changed
- Consolidate example-clients and tools with the versions in
  [jack2](https://github.com/jackaudio/jack2)
- Apply commits from open pull requests in
  [example-clients](https://github.com/jackaudio/example-clients) and
  [tools](https://github.com/jackaudio/tools)
- Make target executables and libraries compatible with jack1 and jack2 (if
  possible)
