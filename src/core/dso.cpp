#include <cstring>
#include <string>

#include <modbox/log/log.hpp>
#include <modbox/util/dso.hpp>
#include <modbox/util/util.hpp>

#include <dlfcn.h>

Dso::Dso(const std::string& _filename) : filename(_filename)
{
    LOG("Loading shared object: " << wstring_cast(filename));
    dso_handle = dlopen(filename.c_str(), RTLD_NOW);
    LOG(dso_handle);
    if (dso_handle == nullptr) {
        auto err = dlerror();
        LOG("Failed to load shared object: " << err);
        throw std::runtime_error(std::string("Failed to load shared object: ") + err);
    }
    LOG("... successfully");
}

Dso::~Dso()
{
    LOG("DELETING Dso " << dso_handle);
    logStackTrace();
    dlclose(dso_handle);
    LOG("Dso deleted");
}
