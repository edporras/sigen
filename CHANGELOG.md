# Change Log

## 2.6.5 - 2019-05-22
### Added
* Better test suite comparing output against reference binaries.

### Fixed
* header includes now use local paths.
* some uninitialized reserved members.

### Changed
* enabling c++-11 build.
* copy & move constructors + op= now prohibited using delete declaration.
* replacing PtrWrapper with c++-11 smart pointers.
* new disable-text-dump configure flag to control enabling of DUMP macros.

## 2.6.4 - 2016-07-29
### Changed
* autotools build system.

## 2.6.3 - 2015-06-20
### Changed
* Initial commit with original source last updated in the early
  2000's.
