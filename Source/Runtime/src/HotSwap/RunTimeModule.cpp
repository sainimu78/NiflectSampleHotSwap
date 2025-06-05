#include "HotSwap/RunTimeModule.h"
#include <stdexcept>
#if defined(_WIN32)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#if defined(_WIN32)
    #define LIB_PREFIX ""
    #define LIB_POSTFIX ".dll"
#elif defined(__APPLE__)
    #define LIB_PREFIX "lib"
    #define LIB_POSTFIX ".dylib"
#else
    #define LIB_PREFIX "lib"
    #define LIB_POSTFIX ".so"
#endif

static Niflect::CString FormatLibraryFilePath(const Niflect::CString& directory, const Niflect::CString& libraryName)
{
    auto path = directory;
    if (!path.empty() && path.back() != '/' && path.back() != '\\')
        path += '/';
    auto pos = libraryName.find(LIB_PREFIX);
    if (pos == std::string::npos)
        path += LIB_PREFIX;
    path += libraryName + LIB_POSTFIX;
    return path;
}
bool CRunTimeLinkingLibrary::Load(const Niflect::CString& directory, const Niflect::CString& libraryName)
{
    NIFLECT_ASSERT(libraryName.find_last_of(LIB_POSTFIX) == std::string::npos);
    auto path = FormatLibraryFilePath(directory, libraryName);
#if defined(_WIN32)
    m_handle = LoadLibrary(path.c_str());
    if (!m_handle)
    {
        DWORD err = GetLastError();
        throw std::runtime_error(("LoadLibrary failed: " + path + " (Error: " + std::to_string(err).c_str() + ")").c_str());
        return false;
    }
#else
    m_handle = dlopen(path.c_str(), RTLD_LAZY);
    if (!m_handle)
    {
        const char* err = dlerror();
        throw std::runtime_error(("dlopen failed: " + path + " (Error: " + (err ? err : "unknown") + ")").c_str());
        return false;
    }
#endif
    return true;
}
void CRunTimeLinkingLibrary::Unload()
{
    if (m_handle)
    {
#if defined(_WIN32)
        FreeLibrary(static_cast<HMODULE>(m_handle));
#else
        dlclose(m_handle);
#endif
    }
}
void* CRunTimeLinkingLibrary::InternalFindSymbol(const Niflect::CString& symbolName) const
{
#if defined(_WIN32)
    void* symbol = GetProcAddress(static_cast<HMODULE>(m_handle), symbolName.c_str());
    if (!symbol)
    {
        throw std::runtime_error(("GetProcAddress failed: " + symbolName).c_str());
    }
    return symbol;
#else
    void* symbol = dlsym(m_handle, symbolName.c_str());
    if (!symbol)
    {
        throw std::runtime_error(("dlsym failed: " + symbolName).c_str());
    }
    return symbol;
#endif
}