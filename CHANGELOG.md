# Change Log

## 2.7.3 - 2019-07-17
### Changed
* SatelliteDeliverySystemDesc updated to 300 468 1.15.1 spec.
* TerrestrialDeliverySystemDesc updated to 300 468 1.15.1 spec.
* Renamed some enums in dvb_defs.h for consistency.
* Dump id/string dual array replaced w/ std::map. Cleanup of interface.
* MultilingualServiceNameSesc::addLanguage deprecated in favor off
  addInfo.
* Relocated descriptor tests to tables that make more sense.
* Deprecated various NIT/BAT methods that took onid argument for to
  match transport stream as it's not required. Replaced with methods
  that don't take it.

### Added
* DUPLICATE_CHECKS option to prevent adding duplicates to table loop
  entries (service loops in SDT, etc). Can be disabled via configure
  option.
* New ExtPSITable to refactor methods for adding descriptors to table
  loop entries.

## 2.7.2 - 2019-06-27
### Changed
* Improvements to some tests.
* Privatized some trace functions that didn't need to be declared
  public.
* new rsrvdStr method for reserved bit traces.
* Be more consistent with trace output of certain values.
* NIT_BAT doesn't need to track XportStram loop length as it's only
  used for trace output (real value is computed when building
  sections).

### Added
* AdaptationFieldDataDesc support.

### Fixed
* AnnouncementSupportDesc now correctly adds extra values in loop
  depending on ref_type.


## 2.7.1 - 2019-06-24
### Fixed
* Stuffing table now correctly sets data length.
* Trace output of PMT that included extra value.
* Dvb::Sat modulation enums and many others.
* Dumb order error in dump constants.

### Changed
* Switch most classes away from unique_ptr and use emplace_back.
* Refactored all inner state machines for section building of items
  with a list of descriptors.
* Consolidated NIT & BAT classes as the format is basically equal.
* Deprecated MultilingualDesc addLanguage method in favor of
  addText().
* Many members that should not have been public are now private or
  protected.
* Deprecated methods in CellListDesc and CellFrequenctLinkDesc in
  favor of others with better names.
* EIT methods now take const ref to UTC and BCDtime types.
* Reworked AC3Desc.

### Added
* LinkageDesc now throws std::domain_error if an attempt to create
  with a type for which a subclass exists to handle the descriptor.
* New DataBroadcastIdDesc that does not take optional component tag as
  arg. New abstract class.
* Support for ExtendedAC3Desc.
* Doxygen documentation.

## 2.7.0 - 2019-06-14
### Changed
* Now handling descriptor data loop length within `DescList`.
* All descriptor lists replaced with `DescList`. Now used as a member
  instead of a super-class.
* Moved length checks to inner-most methods.
* Switched many descriptors with inner lists of structs to use
  `std::unique_ptr`.
* No longer tracking section-build state with static values.

### Removed
* `reserved` arguments. This was a bad idea.

## 2.6.6 - 2019-05-24
### Changed
* Completely replaced `PtrWrapped` with `std::unique_ptr` (not available
  back in '99). This is probably overkill anyway.
* Disable many default and copy constructors, etc.
* Split tests into separate files.

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
