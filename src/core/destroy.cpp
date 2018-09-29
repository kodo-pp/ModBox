#include <thread>

#include <core/core.hpp>
#include <core/init.hpp>
#include <core/memory_manager.hpp>
#include <graphics/graphics.hpp>
#include <log/log.hpp>
#include <net/net.hpp>
#include <util/util.hpp>

#include <signal.h>

std::atomic<bool> doWeNeedToShutDown(false);
std::atomic<bool> areWeShuttingDown(false);

void destroy(void)
{
    // Just shut down
    LOG("Shutting down");
    exit(0);
}

void sigIntHandler(UNUSED int signal)
{
    LOG(L"Wow, you interrupted me! How rude...");
    destroy();
}

void sigAbrtHandler(int)
{
    static int recursive = 0;
    if (recursive == 1) {
        recursive = 2;
        LOG("Double SIGABRT");
        raise(SIGKILL);
    } else if (recursive >= 2) {
        raise(SIGKILL);
    }
    recursive = 1;

    LOG("SIGABRT Caught");
    logStackTrace();
    LOG("Raising SIGQUIT");
    raise(SIGQUIT);
}
void sigSegvHandler(int)
{
    static int recursive = 0;
    if (recursive == 1) {
        recursive = 2;
        LOG("Double SIGSEGV");
        raise(SIGKILL);
    } else if (recursive >= 2) {
        raise(SIGKILL);
    }
    recursive = 1;

    LOG("SIGSEGV Caught");
    logStackTrace();
    LOG("Raising SIGQUIT");
    raise(SIGQUIT);
}
