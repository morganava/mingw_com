#pragma once

#include <cstdint>
#include <cstdio>
#include <initializer_list>
#include <ctime>

#define THROW_IF_FALSE(B) ::rap::throw_if_false(__FILE__, __LINE__, #B, static_cast<bool>(B))
#define LOG_IF_FALSE(B) ::rap::log_if_false(__FILE__, __LINE__, #B, static_cast<bool>(B))
#define THROW_IF_FAILED(HR) ::rap::throw_if_failed(__FILE__, __LINE__, #HR, static_cast<HRESULT>(HR))
#define LOG_IF_FAILED(HR) ::rap::log_if_failed(__FILE__, __LINE__, #HR, static_cast<HRESULT>(HR))
// whitelist some HRESULTS
#define LOG_IF_FAILED_EX(HR, ...) ::rap::log_if_failed_ex(__FILE__, __LINE__, #HR, static_cast<HRESULT>(HR), { __VA_ARGS__ })
#define TRACE_MSG(...) FTRACE_MSG(stdout, __VA_ARGS__)
#define TRACE() FTRACE(stdout)
#define FTRACE_MSG(DEST, ...) do { fprintf(DEST, "%s %s(%i): ", rap::timestamp(), __FUNCTION__, __LINE__); fprintf(DEST, __VA_ARGS__); fputc('\n', DEST); fflush(DEST); } while(false)
#define FTRACE(DEST) do { fprintf(DEST, "%s %s(%i)\n", rap::timestamp(), __FUNCTION__, __LINE__); fflush(DEST); } while(false)



namespace rap
{
    inline bool log_if_false(const char* filename, int line, const char* code, bool b)
    {
        if(!b)
        {
            ::fprintf(stderr, "False at %s(%i): %s\n", filename, line, code);
            return false;
        }
        return true;
    }

    inline void throw_if_false(const char* filename, int line, const char* code, bool b)
    {
        if (!log_if_false(filename, line, code, b))
        {
            throw b;
        }
    }

    inline bool log_if_failed(const char* filename, int line, const char* code, HRESULT hr)
    {
        if (FAILED(hr)) {
            ::fprintf(stderr, "Failure at %s(%i): %s return 0x%08x\n", filename, line, code, static_cast<uint32_t>(hr));
            return false;
        }
        else if(hr == S_FALSE) {
            return true;
        }
        return true;
    }

    inline void throw_if_failed(const char* filename, int line, const char* code, HRESULT hr)
    {
        if (!log_if_failed(filename, line, code,  hr))
        {
            throw hr;
        }
    }

    inline bool log_if_failed_ex(const char* filename, int line, const char* code, HRESULT hr, std::initializer_list<HRESULT> ignoreList)
    {
        for(auto val : ignoreList)
        {
            if (hr == val) {
                return SUCCEEDED(hr);
            }
        }

        return log_if_failed(filename, line, code, hr);
    }

    inline const char* timestamp()
    {
        auto t = std::time(nullptr);
        static char retval[64];
        if(std::strftime(retval, sizeof(retval), "%H:%M:%S:", std::localtime(&t)))
            return retval;
        return nullptr;
    }
}