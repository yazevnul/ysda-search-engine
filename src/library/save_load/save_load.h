#pragma once

#include <string>


namespace ysave_load {

    template <typename T>
    void Save(const T& data, const std::string& file_name);


    template <typename T>
    T Load(const std::string& file_name);

}  // namespace ysave_load

