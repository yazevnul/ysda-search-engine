#pragma once

#include <library/save_load/save_load.h>

#include <string>
#include <vector>

#include <cstdint>


namespace ycrawler {

    //! ysci stands for "simple crawler implementation"
    namespace sci {

        namespace url {

            using UrlId = std::uint32_t;


            struct UrlIdWithTries {
                UrlId id;            // url identifier
                std::uint32_t tries; // number of failed download attempts

                UrlIdWithTries()
                    : id{0}
                    , tries{0} {
                }

                UrlIdWithTries(const UrlId id_other, const std::uint32_t tries_other)
                    : id{id_other}
                    , tries{tries_other} {
                }
            };

        }  // namespace url

    }  // namespace sci

}  // namespace ycrawler


namespace ysave_load {

    template <>
    void Save<>(const std::vector<ycrawler::sci::url::UrlId>& data, const std::string& file_name);


    namespace detail {
        template <>
        struct LoadImpl<std::vector<ycrawler::sci::url::UrlId>> {
            static std::vector<ycrawler::sci::url::UrlId> Do(const std::string& file_name);
        };
    }  // namespace detail

}  // namespace ysave_load

