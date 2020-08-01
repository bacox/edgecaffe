//
// Created by bacox on 12/04/2020.
//

#include "InferenceOutput.h"

namespace EdgeCaffe
{

    void InferenceOutput::initFromLayerVector(const std::vector<std::string>& layerNames)
    {
        int idx = 0;
        for (const auto &layer : layerNames)
        {
            LayerProfile lp;
            lp.layerId = idx++;
            lp.layerName = layer;
            networkProfile.push_back(lp);
        }
    }

    void InferenceOutput::setLoadingTime(Task *task)
    {
        int layerId = task->layerId;
        networkProfile[layerId].loading = task->profileLine;
    }

    void InferenceOutput::setExecutionTime(Task *task)
    {
        networkProfile[task->layerId].executing = task->profileLine;
    }

    std::vector<std::string> InferenceOutput::toCsvLines()
    {
        std::vector<std::string> lines;
        std::string sep = ",";
        for (const LayerProfile &pl : networkProfile)
        {

            std::string line = networkName + sep + std::to_string(pl.layerId) + sep + pl.layerName + sep +
                               std::to_string(pl.loading.duration) + sep +
                               std::to_string(pl.executing.duration) + sep + policy;
            lines.push_back(line);
        }
        return lines;
    }

    void InferenceOutput::addTaskProfile(Task *task, bool isLoading)
    {
        taskProfile.push_back({
            task->id,
            task->layerId,
            isLoading,
            task->moveToWaiting,
            task->moveToReady,
            task->startTask,
            task->stopTask,
            task->networkFinished
        });
    }



    std::vector<std::string> InferenceOutput::calculateTaskProfile(std::vector<event> events)
    {
        std::vector<std::string> lines;
        std::sort(events.begin(), events.end(), [](event a, event b){ return a.time < b.time; });

        int count_waiting = 0;
        int count_running = 0;
        int count_finished = 0;
        for( auto e : events)
        {
            std::string line;
            switch (e.type)
            {
                case WAITING_UP:
                    count_waiting++;
                    line = std::to_string(e.time) + "," + std::to_string(count_waiting) + "," + "waiting";
                    break;
                case WAITING_DOWN:
                    count_waiting--;
                    line = std::to_string(e.time) + "," + std::to_string(count_waiting) + "," + "waiting";
                    break;
                case RUNNING_UP:
                    count_running++;
                    line = std::to_string(e.time) + "," + std::to_string(count_running) + "," + "running";
                    break;
                case RUNNING_DOWN:
                    count_running--;
                    line = std::to_string(e.time) + "," + std::to_string(count_running) + "," + "running";
                    break;
                case FINISHED_UP:
                    count_finished++;
                    line = std::to_string(e.time) + "," + std::to_string(count_finished) + "," + "finished";
                    break;
            }
            lines.push_back(line);
        }
        return lines;
    }

    void InferenceOutput::getTaskEvents(std::vector<event> &list, std::chrono::time_point<std::chrono::system_clock> ref)
    {
        for(auto a: taskProfile)
        {
            list.push_back({(a.toWaiting - ref).count(), WAITING_UP});
            list.push_back({(a.startTask - ref).count(), WAITING_DOWN});
            list.push_back({(a.startTask - ref).count(), RUNNING_UP});
            list.push_back({(a.stopTask - ref).count(), RUNNING_DOWN});
            list.push_back({(a.stopTask - ref).count(), FINISHED_UP});
        }
    }
}