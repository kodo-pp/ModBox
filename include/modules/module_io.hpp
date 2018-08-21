#ifndef MODULES_MODULE_IO_HPP
#define MODULES_MODULE_IO_HPP

#include <cassert>
#include <string>

#include <core/core.hpp>

bool readModuleHeader(int sock);
std::wstring readModuleName(int sock);

void* recvArg(int sock, char spec);
void sendArg(int sock, void* arg, char spec);
void freeArg(void* arg, char spec);

template <typename T>
T getArgument(const std::vector<void*> args, size_t idx)
{
    assert(args.at(idx) != nullptr);
    return *static_cast<T*>(args[idx]);
}

template <typename T>
void setReturn(struct FuncResult* res, size_t idx, const T& value)
{
    // TODO: VERY uGly
#ifdef FORTIFY_SOURCE
    res->data.at(idx)
#else
    res->data[idx]
#endif
            = static_cast<void*>(new T(value));
}

#endif /* end of include guard: MODULES_MODULE_IO_HPP */
