// Copyright 1999-2019 Ed Porras
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy,
// modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
// BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// eit_desc.h: class definition for the DVB defined EIT descriptors
// -----------------------------------

#pragma once

#include <list>
#include <string>
#include "descriptor.h"

namespace sigen {

   class LanguageCode;

   // ---------------------------
   // Component Descriptor
   //
   class ComponentDesc : public Descriptor
   {
   private:
      std::string text;
      LanguageCode language_code;
      ui8 component_type,
         component_tag,
         stream_content : 4;
      bool reserved;

   public:
      enum { TAG = 0x50, BASE_LEN = 6 };

      // constructor
      ComponentDesc(ui8 sc, ui8 ctype, ui8 ctag,
                    const std::string& code, const std::string& text,
                    bool rsrvd = true);

      // util
      virtual Descriptor *clone() const { return new ComponentDesc(*this); }
      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif
   };


   // ---------------------------
   // Content Descriptor
   //
   class ContentDesc : public Descriptor
   {
   public:
      enum { TAG = 0x54, BASE_LEN = 0 };

      // content data struct
      struct Content {
         enum { BASE_LEN = 2 };

         ui8 nibble_level_1 : 4,
            nibble_level_2 : 4,
            user_nibble_1 : 4,
            user_nibble_2 : 4;

         // constructor
         Content(ui8 nl1, ui8 nl2, ui8 un1, ui8 un2) :
            nibble_level_1(nl1), nibble_level_2(nl2),
            user_nibble_1(un1), user_nibble_2(un2) { }
      };


      // constructor
      ContentDesc() : Descriptor(TAG, BASE_LEN) { }

      // utility
      virtual Descriptor *clone() const { return new ContentDesc(*this); }
      bool addContent(ui8 nl1, ui8 nl2, ui8 un1, ui8 un2);
      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      // descriptor data beings here
      std::list<Content> content_list;
   };


   // ---------------------------
   // Extended Event Descriptor
   //
   class ExtendedEventDesc : public Descriptor
   {
   public:
      enum { TAG = 0x4e, BASE_LEN = 6, MAX_DESC_IDX = 16 };

      // event item data struct
      struct Item {
         enum { BASE_LEN = 2 };

         std::string description,
            name;

         // constructor
         Item(const std::string &d, const std::string& n) :
            description(d), name(n) { }

         ui16 length() const {
            return description.length() + name.length() + BASE_LEN;
         }
      };


      // constructor
      ExtendedEventDesc(const std::string& lang_code, const std::string& evtext,
                        ui8 desc_num, ui8 last_desc_num = 0) :
         Descriptor(TAG, BASE_LEN ),
         language_code( lang_code ),
         text( incLength(evtext) ),
         descriptor_number( desc_num ),
         last_descriptor_number( last_desc_num ) // this may have to be re-set later
      { }

      // use to replace the descriptor count value
      void setLastDescriptorNumber( ui8 last_desc_num ) {
         last_descriptor_number = last_desc_num;
      }

      // adds an item to the loop.. we pass by value cause we might need to
      // modify the strings
      bool addItem(const std::string& desc, const std::string& item);
      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      // descriptor data begins here
      std::list<Item> item_list;
      LanguageCode language_code;
      std::string text;
      ui8 descriptor_number : 4,
         last_descriptor_number : 4;

   protected:
      ui8 itemListSize() const;
   };



   // ---------------------------
   // Multilingual Component Descriptor
   // - derived from MultilingualTextDesc (descriptor.h) and looks just
   // like MultilingualNetworkNameDesc except for TAG, BASE_LEN and
   // body of dump()
   //
   class MultilingualComponentDesc : public MultilingualTextDesc
   {
   private:
      ui8 component_tag;

   public:
      enum { TAG = 0x5e, BASE_LEN = 1 };

      // constructor
      MultilingualComponentDesc(ui8 ctag) :
         MultilingualTextDesc(TAG, BASE_LEN),
         component_tag(ctag) {}

      // utility
      virtual Descriptor *clone() const {
         return new MultilingualComponentDesc(*this);
      }

      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif
   };


   // ---------------------------
   // Parental Rating Descriptor
   //
   class ParentalRatingDesc : public Descriptor
   {
   public:
      enum { TAG = 0x55, BASE_LEN = 0 };

      // rating data struct
      struct Rating {
         enum { BASE_LEN = 4 };

         // data
         LanguageCode country_code;
         ui8 value;

         // constructor
         Rating(const std::string& code, ui8 r) :
            country_code(code), value(r) { }
      };


      // constructor
      ParentalRatingDesc() : Descriptor(TAG, BASE_LEN) { }

      // utility
      virtual Descriptor *clone() const { return new ParentalRatingDesc(*this); }
      bool addRating(const std::string& code, ui8 r);
      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif

   private:
      // descriptor data beings here
      std::list<Rating> rating_list;
   };



   // ---------------------------
   // PDC Descriptor
   //
   class PDCDesc : public Descriptor
   {
   private:
      ui32 programme_identification_label : 20;
      bool reserved;

   public:
      enum { TAG = 0x69, BASE_LEN = 3 };

      // constructor
      PDCDesc(ui32 pil, bool rsrvd = true) :
         Descriptor(TAG, BASE_LEN),
         programme_identification_label(pil),
         reserved(rsrvd)
      { }

      // util
      virtual Descriptor *clone() const { return new PDCDesc(*this); }
      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif
   };



   // ---------------------------
   // Short Event Descriptor
   //
   class ShortEventDesc : public Descriptor
   {
   private:
      LanguageCode language_code;
      std::string name,
         text;

   public:
      enum { TAG = 0x4d, BASE_LEN = 5 };

      // constructor
      ShortEventDesc(const std::string& code, const std::string& ev_name,
                     const std::string& text);

      // util
      virtual Descriptor *clone() const { return new ShortEventDesc(*this); }
      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif
   };



   // ---------------------------
   // Short Smoothing Buffer Descriptor
   //
   class ShortSmoothingBufferDesc : public Descriptor
   {
   private:
      std::string DVB_reserved;
      ui8 sb_size : 2,
         sb_leak_rate : 6;

   public:
      enum { TAG = 0x61, BASE_LEN = 1 };

      // constructor
      ShortSmoothingBufferDesc(ui8 size, ui8 leak_rate,
                               const std::string& dvb_reserved = "");

      // utility
      virtual Descriptor *clone() const {
         return new ShortSmoothingBufferDesc(*this);
      }
      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif
   };



   // ---------------------------
   // Time Shifted Event Descriptor
   //

   // could be derived from TSServiceD, but oh well..
   class TimeShiftedEventDesc : public Descriptor
   {
   private:
      ui16 ref_service_id,
         ref_event_id;

   public:
      enum { TAG = 0x4f, BASE_LEN = 4 };

      // constructor
      TimeShiftedEventDesc(ui16 sid,  ui16 evid) :
         Descriptor(TAG, BASE_LEN),
         ref_service_id(sid), ref_event_id(evid) {}

      // utility
      virtual Descriptor* clone() const {
         return new TimeShiftedEventDesc(*this);
      }
      virtual void buildSections(Section&) const;

#ifdef ENABLE_DUMP
      virtual void dump(std::ostream&) const;
#endif
   };

} // sigen namespace

