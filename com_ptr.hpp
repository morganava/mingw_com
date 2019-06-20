#pragma once

// windows
#include <windows.h>

// c++
#include <cstdio>

#ifdef INTERFACE
#undef INTERFACE
#endif

#include "logger.hpp"

#define MAKE_IID(INTERFACE) template<> struct iid<INTERFACE> { constexpr static auto& value = IID_##INTERFACE;}

namespace rap
{
    template<typename INTERFACE> struct iid {};

    template<typename INTERFACE>
    class com_ptr
    {
    public:
        com_ptr()
        : _pint(nullptr)
        { }

        com_ptr(INTERFACE* pInterface)
        : _pint(pInterface)
        {
            if (this->_pint) {
                this->_pint->AddRef();
            }
        }

        com_ptr(const com_ptr& that)
        : _pint(nullptr)
        {
            if (that._pint) {
                this->_pint = that._pint;
                this->_pint->AddRef();
            }
        }

        com_ptr(com_ptr&& that)
        {
            _pint = that._pint;
            that._pint = nullptr;
        }

        ~com_ptr()
        {
            if (this->_pint)
            {
                this->_pint->Release();
                this->reset();
            }
        }

        com_ptr& operator=(const com_ptr& that)
        {
            if(this->_pint != that._pint)
            {
                this->~com_ptr();
                this->_pint = that._pint;
                this->_pint->AddRef();
            }
            return *this;
        }

        com_ptr& operator=(com_ptr&& that)
        {
            if(this->_pint != that._pint)
            {
                this->~com_ptr();
                this->_pint = that._pint;
                that._print = nullptr;
            }
            return *this;
        }

        INTERFACE** operator&()
        {
            return &_pint;
        }

        INTERFACE* get()
        {
            return _pint;
        }

        INTERFACE* operator->()
        {
            return _pint;
        }

        void reset()
        {
            this->_pint = nullptr;
        }

        INTERFACE* release()
        {
            auto retval = _pint;
            this->_pint = nullptr;
            return retval;
        }

        template<typename T>
        com_ptr<T> QueryInterface()
        {
            com_ptr<T> retval;
            LOG_IF_FAILED(_pint->QueryInterface(iid<T>::value, reinterpret_cast<void**>(&retval)));
            return retval;
        }

        operator bool() const
        {
            return _pint != nullptr;
        }
    private:
        INTERFACE* _pint;
    };
}