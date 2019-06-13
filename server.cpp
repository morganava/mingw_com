// system
#include <windows.h>

// c++
#include <cstdio>
#include <vector>
#include <string_view>
#include <atomic>
#include <memory>

// local
#include "com_ptr.hpp"
#include "foo.h"

using namespace std;
using namespace rap;

namespace rap
{
    MAKE_IID(IUnknown);
    MAKE_IID(IClassFactory);
    MAKE_IID(IBar);
    MAKE_IID(IFoo);

    template<typename BASE, typename INT0, typename... INTN>
    HRESULT IUnknownComponent_QueryInterfaceImpl(BASE* self, REFIID riid, void** ppvObject)
    {
        if (riid == iid<INT0>::value)
        {
            *ppvObject = static_cast<INT0*>(self);
        }
        else
        {
            return IUnknownComponent_QueryInterfaceImpl<BASE, INTN...>(self, riid, ppvObject);
        }
        self->AddRef();
        return S_OK;
    }

    template<typename BASE>
    HRESULT IUnknownComponent_QueryInterfaceImpl(BASE* self, REFIID riid, void** ppvObject)
    {
        return E_NOINTERFACE;
    }

    template<typename BASE, typename... INTERFACES>
    class IUnknownComponent
    {
    public:
        HRESULT QueryInterface(BASE* self, REFIID riid, void** ppvObject)
        {
            if (!ppvObject) return E_INVALIDARG;
            return IUnknownComponent_QueryInterfaceImpl<BASE, INTERFACES...>(self, riid, ppvObject);
        }

        ULONG AddRef()
        {
            return ++_refcount;
        }

        ULONG Release(BASE* self)
        {
            auto retval = --_refcount;
            if (retval == 0)
            {
                delete self;
            }
            return retval;
        }
    private:
        atomic_size_t _refcount = 0;
    };

    class BarImpl : public IBar
    {
    public:
        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override
        {
            return iuc.QueryInterface(this, riid, ppvObject);
        }

        ULONG STDMETHODCALLTYPE AddRef() override
        {
            return iuc.AddRef();
        }

        ULONG STDMETHODCALLTYPE Release() override
        {
            return iuc.Release(this);
        }

        HRESULT STDMETHODCALLTYPE PrintMsg() override
        {
            TRACE_MSG("Bar Id: %d", id);
        }

        void setId(LONG id)
        {
            this->id = id;
        }
    private:
        IUnknownComponent<BarImpl, IUnknown, IBar> iuc;
        LONG id = -1;
    };

    class FooImpl : public IFoo
    {
    public:
        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override
        {
            return iuc.QueryInterface(this, riid, ppvObject);
        }

        ULONG STDMETHODCALLTYPE AddRef() override
        {
            return iuc.AddRef();
        }

        ULONG STDMETHODCALLTYPE Release() override
        {
            return iuc.Release(this);
        }

        HRESULT STDMETHODCALLTYPE getBarCount(LONG *count) override
        {
            TRACE();
            if (barCount == -1)
            {
                barCount = rand() % 10 + 1;
                TRACE();
                bars.reset(new BarImpl[barCount]);
                for(LONG idx = 0; idx < barCount; ++idx)
                {
                    TRACE();
                    bars[idx].setId(idx);
                }
            }
            *count = barCount;

            return S_OK;
        }

        HRESULT STDMETHODCALLTYPE getBarAt(LONG idx, IBar** bar) override
        {
            TRACE_MSG("idx : %d", idx);
            if (bar == nullptr || idx < 0 || idx >= barCount) {
                TRACE();
                return E_INVALIDARG;
            }

            TRACE();
            *bar = bars.get() + idx;
            return S_OK;
        }

    private:
        IUnknownComponent<FooImpl, IUnknown, IFoo> iuc;
        LONG barCount = -1;
        unique_ptr<BarImpl[]> bars;
    };

    class FooFactoryImpl : public IClassFactory
    {
    public:
        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override
        {
            return iuc.QueryInterface(this, riid, ppvObject);
        }

        ULONG STDMETHODCALLTYPE AddRef() override
        {
            return iuc.AddRef();
        }

        ULONG STDMETHODCALLTYPE Release() override
        {
            return iuc.Release(this);
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
        IUnknownComponent<FooFactoryImpl, IUnknown, IClassFactory> iuc;
    };
}

int main(int argc, char** argv)
{
    TRACE_MSG("init");

    srand(0);

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