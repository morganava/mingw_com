// system
#include <windows.h>

// c++
#include <cstdio>
#include <vector>
#include <string_view>
#include <atomic>

// local
#include "com_ptr.hpp"
#include "foo.h"

using namespace std;
using namespace rap;

class FooImpl : public IFoo
{
public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override
    {
        if (ppvObject == nullptr)
        {
            return E_INVALIDARG;
        }

        if (riid == IID_IUnknown)
        {
            *ppvObject = static_cast<IUnknown*>(this);
        }
        else if(riid == IID_IFoo)
        {
            *ppvObject = static_cast<IFoo*>(this);
        }
        else
        {
            *ppvObject = nullptr;
            return E_NOINTERFACE;
        }
        this->AddRef();
        return S_OK;
    }

    ULONG STDMETHODCALLTYPE AddRef() override
    {
        return ++_refcount;
    }

    ULONG STDMETHODCALLTYPE Release() override
    {
        auto retval = --_refcount;
        if (retval == 0)
        {
            delete this;
        }
        return retval;
    }

    HRESULT STDMETHODCALLTYPE DoBar()
    {
        TRACE();
        return S_OK;
    }
private:
    atomic_size_t _refcount = 0;
};

class FooFactoryImpl : public IClassFactory
{
public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override
    {
        if (ppvObject == nullptr)
        {
            return E_INVALIDARG;
        }

        if (riid == IID_IUnknown)
        {
            *ppvObject = static_cast<IUnknown*>(this);
        }
        else if(riid == IID_IClassFactory)
        {
            *ppvObject = static_cast<IClassFactory*>(this);
        }
        else
        {
            *ppvObject = nullptr;
            return E_NOINTERFACE;
        }
        this->AddRef();
        return S_OK;
    }

    ULONG STDMETHODCALLTYPE AddRef() override
    {
        return ++_refcount;
    }

    ULONG STDMETHODCALLTYPE Release() override
    {
        auto retval = --_refcount;
        if (retval == 0)
        {
            delete this;
        }
        return retval;
    }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL lock) override
    {
        if (lock) TRACE_MSG("LOCK");
        else TRACE_MSG("UNLOCK");

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown *pUnkOuter, REFIID riid, void** ppv) override
    {
        if (pUnkOuter != nullptr) return CLASS_E_NOAGGREGATION;

        if (riid == IID_IFoo)
        {
            com_ptr<IFoo> foo(new FooImpl());
            *ppv = foo.release();
        }
        else
        {
            return E_NOINTERFACE;
        }

        return S_OK;
    }

private:
    atomic_size_t _refcount = 0;
};

int main(int argc, char** argv)
{
    TRACE_MSG("init");

    vector<string_view> args;
    args.insert(args.begin(), argv, argv + argc);

    bool embedding = false;
    for(auto arg : args)
    {
        if (arg.find("Embedding") == string_view::npos) continue;
        embedding = true; break;
    }

    if (embedding)
    {
        THROW_IF_FAILED(CoInitialize(nullptr));

        com_ptr<ITypeLib> typelib;
        THROW_IF_FAILED(LoadTypeLibEx(L"fooproxy.dll", REGKIND_REGISTER, &typelib));
        TRACE_MSG("registered typelib");

        com_ptr<IClassFactory> fooFactory(new FooFactoryImpl());

        DWORD regId = 0;
        THROW_IF_FAILED(
            CoRegisterClassObject(
                CLSID_Foo,
                fooFactory.get(),
                CLSCTX_LOCAL_SERVER,
                REGCLS_MULTIPLEUSE,
                &regId));

        MSG ms;
        while(GetMessage(&ms, 0, 0, 0))
        {
            TranslateMessage(&ms);
            DispatchMessage(&ms);
        }

        CoRevokeClassObject(regId);
        CoUninitialize();
    }
    else
    {
        TRACE_MSG("not embedding");
    }
}