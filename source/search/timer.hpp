#pragma once

#include "core/types.hpp"
#include <atomic>
#include <chrono>


namespace Tempo::Search {

    struct Timer {
      private:
        inline static TimePoint startTime;
        inline static int maxTimeMs;

      public:
        inline static std::atomic_bool stopFlag;

        inline static void start(int maxTime) {
            maxTimeMs = maxTime;
            startTime = SteadyClock::now();
            stopFlag.store(false);
        }

        static void requestStop() {
            stopFlag.store(true);
        }

        inline static int elapsed() {
            return Chrono::duration_cast<Chrono::milliseconds>(SteadyClock::now() - startTime).count();
        }

        inline static bool shouldStopSearch() {
            return stopFlag.load() || (maxTimeMs > 0 && elapsed() > maxTimeMs);
        }
    };
} // namespace Tempo::Search