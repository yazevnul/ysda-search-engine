#pragma once

#include <library/save_load/save_load.h>

#include <string>
#include <vector>


namespace ygraph {

    template <typename T>
    using AdjacencyList = std::vector<std::vector<T>>;

}  // namespace ygraph
