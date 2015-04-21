#pragma once

#include <string>


namespace ysave_load {

    namespace detail {

        template <typename T>
        struct LoadImpl;

    }  // namespace detail

    template <typename T>
    void Save(const T& data, const std::string& file_name);


    template <typename T>
    T Load(const std::string& file_name) {
        return detail::LoadImpl<T>::Do(file_name);
    }

}  // namespace ysave_load

