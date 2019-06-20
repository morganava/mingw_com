// system
#include <windows.h>

// c++
#include <cstdio>

// local
#include "com_ptr.hpp"
#include "bstr.hpp"
#include "foo.h"

using namespace std;
using namespace rap;

int main(int argc, char** argv)
{
    try
    {
        TRACE();

        THROW_IF_FAILED(CoInitialize(nullptr));
        {
            com_ptr<IClassFactory> fooFactory;
            THROW_IF_FAILED(
                CoGetClassObject(
                    CLSID_Foo,
                    CLSCTX_LOCAL_SERVER,
                    nullptr,
                    IID_IClassFactory,
                    (void**)&fooFactory));

            com_ptr<IFoo> foo;
            THROW_IF_FAILED(fooFactory->CreateInstance(nullptr, IID_IFoo, (void**)&foo));
            LONG barCount = 0;
            THROW_IF_FAILED(foo->getBarCount(&barCount));
            TRACE_MSG("bar count : %d", barCount);
            for(LONG idx = 0; idx < barCount; ++idx)
            {
                com_ptr<IBar> bar;
                THROW_IF_FAILED(foo->getBarAt(idx, &bar));
                bstr str;
                THROW_IF_FAILED(bar->getString(&str));
                TRACE_MSG("bar[%i] : %S", idx, str.get());
            }
        }
        CoUninitialize();
        throw nullptr;
    }
    catch(...)
    {
        TRACE_MSG("Make sure you've done the required server and proxy registration, see registerFoo.bat");
    }
}