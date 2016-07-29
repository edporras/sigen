//
// util.h: miscellaneous utilities
// -----------------------------------

#pragma once

#include <stdexcept>
#include <string>
#include "types.h"

namespace sigen {

    // reserved-bit related functions
    // computes a set of bits based on the value of the boolean
    ui32 rbits(bool reserved, ui32 mask);

    //
    // our own NULL ptr exception
    struct null_ptr : public std::logic_error {
        null_ptr() :
            std::logic_error("Attempt to de-reference NULL ptr") { }
    };

    //
    // a templated pointer wrapper (based on
    // http://www.xraylith.wisc.edu/~khan/software/stl/STL.newbie.html#pointer_deriv_template)
    // thanks to Mumit Khan <khan@nanotech.wisc.edu> for the idea
    //
    template <class T>
    class PtrWrapper {
    private:
        // pointer to the object
        T* t_ptr_;

    public:
        // construtors
        PtrWrapper(T* t_ptr = NULL) : t_ptr_(t_ptr) { }

        // copy constructor
        PtrWrapper(const PtrWrapper<T>& w) {
            t_ptr_ = w() ? w()->clone() : NULL;
        }
        ~PtrWrapper() { delete t_ptr_; }

        // function operators - return the pointer
        const T* operator()() const { return t_ptr_; }
        T* operator()() { return t_ptr_; }

        // de-reference operators - return the object or throw a null_ptr
        // exception if NULL
        const T& operator*() const throw(null_ptr) {
            if (t_ptr_) return *t_ptr_;
            throw null_ptr();
        }
        T& operator*() throw(null_ptr) {
            if (t_ptr_) return *t_ptr_;
            throw null_ptr();
        }

        // assignment operator
        PtrWrapper<T>& operator=(const PtrWrapper<T>& w) {
            if (this != &w)
            {
                delete t_ptr_;
                t_ptr_ = w()->clone();
            }
            return *this;
        }
    };

    // ---------------------------
    // Language Code container class
    //
    class LanguageCode
    {
    private:
        std::string code;

        void refit();

    public:
        enum { BASE_LEN = 3 };

        // constructor
        LanguageCode(const char* c) : code(c) { refit(); }
        LanguageCode(const std::string &c) : code(c) { refit(); }

        // util
        LanguageCode* clone() const { return new LanguageCode(*this); }
        const std::string& str() const { return code; }
        ui32 length() const { return code.length(); }
    };

} // sigen namespace

