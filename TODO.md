Must do:

* Tables need to provide MAX_SIZE to base class for checking with incLength.

=====================================
The following are NOT YET implemented.

DVB Tables:
----------
* EIT Event Schedule

DVB Descriptors:
---------------
* Mosaic Descriptor
* Extended Event Descriptor is defined but makes user responsible
  for splitting data across multiple descriptors.  Need to add
  a support class to handle this.
