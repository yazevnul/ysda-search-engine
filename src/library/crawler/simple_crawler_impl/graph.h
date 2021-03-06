#pragma once

#include "detail.h"

#include <library/graph/graph.h>

#include <algorithm>
#include <mutex>
#include <type_traits>
#include <utility>
#include <vector>


namespace ycrawler {

    namespace sci {

        template <typename T>
        class SparceGraphWithMutex : public WithMutex {
            static_assert(std::is_integral<T>::value, "type must be integral");
        public:
            SparceGraphWithMutex() = default;
            virtual ~SparceGraphWithMutex() noexcept = default;

            void Add(const T vertice, std::vector<T>&& to) {
                std::lock_guard<std::mutex> lock_guard{object_mutex_};
                if (to.empty()) {
                    ++graph_.sink_vertices_count;
                }
                graph_.edges_count += to.size();
                if (to.empty()) {
                    graph_.vertices_count = std::max(
                        graph_.vertices_count, static_cast<std::uint32_t>(vertice)
                    );
                } else {
                    graph_.vertices_count = std::max({
                        graph_.vertices_count, static_cast<std::uint32_t>(vertice),
                        static_cast<std::uint32_t>(*std::max_element(to.begin(), to.end()))
                    });
                }
                graph_.adjacency_list.insert({vertice, std::forward<std::vector<T>>(to)});
            }

            ygraph::SparceGraph<T>&& Get() {
                ++graph_.vertices_count;
                return std::move(graph_);
            }

            void Set(ygraph::SparceGraph<T>&& graph) {
                graph_ = graph;
            }

        private:
            ygraph::SparceGraph<T> graph_;
        };

    }  // namesapce sci

}  // namespace ycrawler

