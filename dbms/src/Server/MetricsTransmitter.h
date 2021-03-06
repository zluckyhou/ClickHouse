#pragma once

#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <Common/ProfileEvents.h>


namespace DB
{
class AsynchronousMetrics;

/**    Automatically sends
  * - difference of ProfileEvents;
  * - values of CurrentMetrics;
  * - values of AsynchronousMetrics;
  *  to Graphite at beginning of every minute.
  */
class MetricsTransmitter
{
public:
    MetricsTransmitter(const AsynchronousMetrics & async_metrics, const std::string & config_name)
        : async_metrics{async_metrics}, config_name{config_name}
    {
    }
    ~MetricsTransmitter();

private:
    void run();
    void transmit(std::vector<ProfileEvents::Count> & prev_counters);

    const AsynchronousMetrics & async_metrics;
    const std::string config_name;

    bool quit = false;
    std::mutex mutex;
    std::condition_variable cond;
    std::thread thread{&MetricsTransmitter::run, this};

    static constexpr auto profile_events_path_prefix = "ClickHouse.ProfileEvents.";
    static constexpr auto current_metrics_path_prefix = "ClickHouse.Metrics.";
    static constexpr auto asynchronous_metrics_path_prefix = "ClickHouse.AsynchronousMetrics.";
};
}
