#pragma once

#include "graph.h"

#include <functional>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>

#include <cstdint>


namespace ygraph {

    namespace detail {

        template <typename T, typename F>
        using BFSCallbackSignatureCheck = typename std::result_of<F&(
            const T from, const T to, const std::vector<bool>& visited
        )>::type;

    }  // namespace detail

    template <typename T, typename F, typename R=detail::BFSCallbackSignatureCheck<T, F>>
    void BFS(const Graph<T>& graph, const T start, F&& callback);


    template <typename T>
    class BFSDistanceCallback {
    public:
        explicit BFSDistanceCallback(const std::uint32_t vertices_count, const T start)
            : distance_(vertices_count, std::numeric_limits<T>::max()) {
            distance_[start] = {};
        }

        void operator ()(const T from, const T to, const std::vector<bool>&) {
            distance_[to] = distance_[from] + 1;
        }

        std::vector<std::uint32_t> Get() {
            return std::move(distance_);
        }

    private:
        std::vector<std::uint32_t> distance_;
    };

}  // namespace ygraph


// Implementation BEGIN

#include <queue>

#include <cassert>

template <typename T, typename F, typename R>
void ygraph::BFS(const Graph<T>&graph, const T start,F&& callback) {
    auto visited = std::vector<bool>(graph.vertices_count);
    auto vertices_queue = std::queue<T>{};

    vertices_queue.push(start);
    visited[start] = true;
    while (!vertices_queue.empty()) {
        const auto from = vertices_queue.front();
        vertices_queue.pop();

        assert(from < graph.vertices_count);
        for (const auto to: graph.adjacency_list[from]) {
            assert(to < graph.vertices_count);
            if (!visited[to]) {
                visited[to] = true;
                callback(from, to, visited);
                vertices_queue.push(to);
            }
        }
    }
}

// Implementation END

