#pragma once

// windows
#include <windows.h>
#include <oleauto.h>

namespace rap
{
    class bstr
    {
    public:
        bstr()
        : _str(nullptr)
        { }

        bstr(const bstr& that)
        : _str(::SysAllocString(that._str))
        { }

        bstr(bstr&& that)
        : _str(that._str)
        {
            that._str = nullptr;
        }

        ~bstr()
        {
            ::SysFreeString(this->_str);
            this->_str = nullptr;   
        }

        bstr& operator=(const bstr& that)
        {
            ::SysFreeString(this->_str);
            this->_str = ::SysAllocString(that._str);
            return *this;
        }

        bstr& operator=(bstr&& that)
        {
            ::SysFreeString(this->_str);
            this->_str = that._str;
            that._str = nullptr;
            return *this;
        }

        BSTR* operator&()
        {
            return &_str;
        }

        ::BSTR get()
        {
            return _str;
        }

        const ::BSTR get() const
        {
            return _str;
        }

    private:
        ::BSTR _str;
    };
}