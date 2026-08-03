#pragma once

#include "core/types.hpp"
#include <chrono>
#include <atomic>

namespace Tempo::Search {

    using Time = std::chrono::steady_clock::time_point;

    struct Timer {
        private:
        inline static Time startTime;
        inline static int maxTimeMs;
        inline static std::atomic_bool stopFlag;

        public:
        inline static void start(int maxTime) {
            maxTimeMs = maxTime;
            startTime = std::chrono::steady_clock::now();
            stopFlag.store(false);
        }

        static void requestStop() {
            stopFlag.store(true);
        }

        inline static int elapsed() {
            return std::chrono::duration_cast<std::chrono::milliseconds>
                    (std::chrono::steady_clock::now() - startTime).count();
        }

        inline static bool shouldStopSearch() {
            return stopFlag.load() || (maxTimeMs > 0 && elapsed() > maxTimeMs);
        }
    };
}