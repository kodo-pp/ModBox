#ifndef GAME_GAME_LOOP_HPP
#define GAME_GAME_LOOP_HPP

#include <cstdint>
#include <functional>
#include <future>
#include <string>
#include <type_traits>
#include <vector>

#include <core/destroy.hpp>
#include <log/log.hpp>
#include <util/util.hpp>

void drawBarrier();

std::recursive_mutex& getDrawFunctionsMutex();
std::recursive_mutex& getIrrlichtMutex();

template <typename F>
auto addDrawFunction(const F& func, bool barrier = false) -> decltype(func())
{
    LOG("Adding draw function");
    using ReturnType = decltype(func());
    extern std::vector<std::packaged_task<void()>> drawFunctions;
    extern std::recursive_mutex drawFunctionsMutex;

    static_assert(
            std::is_same_v<
                    ReturnType,
                    void> || std::is_default_constructible_v<ReturnType> || (std::is_copy_assignable_v<ReturnType> && std::is_copy_constructible_v<ReturnType>),
            "ReturnType is neither void, nor default constructible, nor copy "
            "constructible&assignable");
    if constexpr (std::is_same_v<ReturnType, void>) {
        {
            std::lock_guard<std::recursive_mutex> lock(drawFunctionsMutex);
            drawFunctions.emplace_back([=]() { func(); });
        }
        if (barrier) {
            std::future<void> future = drawFunctions.back().get_future();
            future.wait();
        }
        LOG("Called");
        return;
    } else if constexpr (std::is_default_constructible_v<ReturnType>) {
        ReturnType ret;
        std::future<void> future;
        {
            std::lock_guard<std::recursive_mutex> lock(drawFunctionsMutex);
            drawFunctions.emplace_back([&]() { ret = func(); });
            future = drawFunctions.back().get_future();
        }
        future.wait();
        return ret;
        LOG("Called");
    } else if constexpr (std::is_copy_assignable_v<
                                 ReturnType> && std::is_copy_constructible_v<ReturnType>) {
        ReturnType* ret;
        std::future<void> future;
        {
            std::lock_guard<std::recursive_mutex> lock(drawFunctionsMutex);
            drawFunctions.emplace_back([&]() { ret = new ReturnType(func()); });
            future = drawFunctions.back().get_future();
        }
        future.wait();
        ReturnType retval(*ret);
        delete ret;
        LOG("Called");
        return retval;
    }
}

void gameLoop();
void drawLoop();
void eachTickWithParam(const std::string& name, uint64_t param);

#endif /* end of include guard: GAME_GAME_LOOP_HPP */
