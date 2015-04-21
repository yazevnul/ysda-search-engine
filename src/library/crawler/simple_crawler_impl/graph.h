#pragma once

#include <library/graph/graph.h>

#include <mutex>
#include <utility>
#include <vector>


namespace ycrawler {

    namespace sci {

        template <typename T>
        class SparceGraphWithMutex {
        public:
            SparceGraphWithMutex() = default;

            void Add(const T vertice, std::vector<T>&& to) {
                std::lock_guard<std::mutex> lock_guard{mutex_};
                graph_.insert({vertice, std::forward<>(to)});
            }

            ygraph::SparceGraph<T>&& Get() {
                return std::move(graph_);
            }

            void Set(ygraph::SparceGraph<T>&& graph) {
                graph_ = graph;
            }

        private:
            std::mutex mutex_;
            ygraph::SparceGraph<T> graph_;
        };

    }  // namesapce sci

}  // namespace ycrawler

