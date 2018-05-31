#include <modules/module_manager.hpp>
#include <log/log.hpp>
#include <net/socketlib.hpp>
#include <core/core.hpp>
#include <exception>
#include <string>
#include <cstring>
#include <vector>
#include <sys/socket.h>
#include <util/util.hpp>
#include <unistd.h>
#include <modules/module_io.hpp>

ModuleWorker::ModuleWorker(int _sock): sock(_sock) { }
ModuleWorker::~ModuleWorker() { }

void ModuleWorker::please_work() noexcept {
    try {
        work();
    } catch (std::exception& e) {
        LOG(L"Module error: ModuleWorker::work() threw exception: '" << wstring_cast(e.what()) << L"'");
        return;
    } catch (...) {
        LOG(L"Module error: ModuleWorker::work() threw something we don't care about");
        return;
    }
}

void ModuleWorker::work() {
    std::string header("ModBox/M");
    sendFixed(sock, header);
    readModuleHeader(sock);
    std::wstring name = readModuleName(sock);
    LOG(L"Module '" << name << L"' connected");

    while (true) {
        // Receive command from module
        std::string cmd = recvString(sock);
        if (cmd == "exit") {
            break;
        }
        // Prepare to run it
        FuncProvider* prov;
        try {
            prov = getFuncProvider(cmd);
        } catch (...) {
            throw std::runtime_error(std::string("Function '") + cmd + "' not found");
        }
        if (prov == nullptr) {
            throw std::runtime_error(std::string("Function '") + cmd + "' not found");
        }
        ArgsSpec argsSpec = getArgsSpec(cmd);

        // Read its arguments
        std::vector <void *> args;
        args.reserve(argsSpec.length());
        for (char i : argsSpec) {
            args.push_back(recvArg(sock, i));
        }


        // Run it
        struct FuncResult * result = (*prov)(args);
        if (result == nullptr) {
            throw std::runtime_error("function provider error: reslut is nullptr");
        }

        // Send result back
        ArgsSpec retSpec = getRetSpec(cmd);
        for (size_t i = 0; i < retSpec.length(); ++i) {
            sendArg(sock, result->data.at(i), retSpec.at(i));
        }

        // Free memory allocated for arguments...
        for (size_t i = 0; i < argsSpec.length(); ++i) {
            freeArg(args.at(i), argsSpec.at(i));
        }
        // ... and for returned data
        for (size_t i = 0; i < retSpec.length(); ++i) {
            freeArg(result->data.at(i), retSpec.at(i));
        }
        delete result;
    }

    LOG(L"Exiting module worker");
}
