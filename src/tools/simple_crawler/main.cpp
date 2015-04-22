#include <third_party/cxxopts/src/cxxopts.hpp>
#include <third_party/json11/json11.hpp>

#include <library/crawler/simple_crawler.h>
#include <library/mod_chooser/mod_chooser.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <cstdint>
#include <cstdlib>


std::string ReadFile(const std::string& file_name) {
    std::ifstream file(file_name);
    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
}


auto ParseConfig(const std::string& file_name) {
    const auto json = [&file_name](){
        auto error = std::string{};
        const auto res = json11::Json::parse(ReadFile(file_name), error);
        if (res.is_null()) {
            throw std::runtime_error{error};
        }
        return res;
    }();
    if (!json.is_object()) {
        std::runtime_error{"Malformed config"};
    }

    auto config = ycrawler::SimpleCrawlerConfig{};

    for (const auto& kv: json.object_items()) {
        if ("threads" == kv.first) {
            config.set_threads(static_cast<std::uint32_t>(kv.second.int_value()));
        } else if ("urls_seed" == kv.first) {
            for (const auto& value: kv.second.array_items()) {
                if (!value.is_string()) {
                    throw std::runtime_error{"Malformed config"};
                }
                *config.add_urls_seed() = value.string_value();
            }
        } else if ("tries_limit" == kv.first) {
            config.set_tries_limit(static_cast<std::uint32_t>(kv.second.int_value()));
        } else if ("state" == kv.first) {
            if (!kv.second.is_object()) {
                std::runtime_error{"Malformed config"};
            }
            for (const auto& ikv: kv.second.object_items()) {
                if ("directory" == ikv.first) {
                    config.mutable_state()->set_directory(ikv.second.string_value());
                } else if ("queued_urls_file_name" == ikv.first) {
                    config.mutable_state()->set_queued_urls_file_name(ikv.second.string_value());
                } else if ("failed_urls_file_name" == ikv.first) {
                    config.mutable_state()->set_failed_urls_file_name(ikv.second.string_value());
                } else if ("processed_urls_file_name" == ikv.first) {
                    config.mutable_state()->set_processed_urls_file_name(ikv.second.string_value());
                } else if ("config_file_name" == ikv.first) {
                    config.mutable_state()->set_config_file_name(ikv.second.string_value());
                }
            }
        } else if ("documents" == kv.first) {
            if (!kv.second.is_object()) {
                std::runtime_error{"Malformed config"};
            }
            for (const auto& ikv: kv.second.object_items()) {
                if ("documents_directory" == ikv.first) {
                    config.mutable_documents()->set_documents_directory(ikv.second.string_value());
                } else if ("documents_data_directory" == ikv.first) {
                    config.mutable_documents()->set_documents_data_directory(ikv.second.string_value());
                } else if ("url_to_id_file_name" == ikv.first) {
                    config.mutable_documents()->set_url_to_id_file_name(ikv.second.string_value());
                } else if ("web_graph_file_name" == ikv.first) {
                    config.mutable_documents()->set_web_graph_file_name(ikv.second.string_value());
                }
            }
        }
    }

    // validation
    if (0 == config.threads()) {
        throw std::runtime_error{"Number of threads is zero"};
    }
    if (0 == config.urls_seed_size()) {
        throw std::runtime_error{"URLs seed is empty"};
    }
    if (0 == config.tries_limit()) {
        throw std::runtime_error{"Number of tries is zero"};
    }
    return config;
}


namespace mode_new {

    static const auto MODE_NAME = std::string{"new"};
    static const auto MODE_DESCRIPTION = std::string{"Create new crawler"};

    struct Args {
        std::string config_file_name;
    };


    auto ParseOptions(int argc, char* argv[]) {
        auto options = cxxopts::Options{MODE_NAME, std::string{"\n  "} + MODE_DESCRIPTION};
        auto args = Args{};
        options.add_options()(
            "c,config",
            "Crawler config file",
            cxxopts::value<>(args.config_file_name),
            "FILE"
        )(
            "h,help",
            "Print help"
        );
        options.parse(argc, argv);
        if (options.count("help")) {
            std::cout << options.help({""}) << std::endl;
            std::exit(EXIT_SUCCESS);
        }
        return args;
    }


    int Main(int argc, char* argv[]) {
        const auto args = ParseOptions(argc, argv);
        const auto config = ParseConfig(args.config_file_name);

        auto crawler = std::make_unique<ycrawler::SimpleCrawler>();
        crawler->SetConfig(config);
        crawler->Start();

        return EXIT_SUCCESS;
    }

}  // namespace mode_new


namespace mode_restore {

    static const auto MODE_NAME = std::string{"restore"};
    static const auto MODE_DESCRIPTION = std::string{"Restore crawler from state"};

    struct Args {
        std::string config_file_name;
        bool move_failed_to_queue = false;
    };


    auto ParseOptions(int argc, char* argv[]) {
        auto options = cxxopts::Options{MODE_NAME, std::string{"\n  "} + MODE_DESCRIPTION};
        auto args = Args{};
        options.add_options()(
            "c,config",
            "Config saved by crawler (probably will be located inside crawler state directory)",
            cxxopts::value<>(args.config_file_name),
            "FILE"
        )(
            "move-failed-to-queue",
            "Move failed urls to queue",
            cxxopts::value<>(args.move_failed_to_queue)
        )(
            "h,help",
            "Print help"
        );
        options.parse(argc, argv);
        if (options.count("help")) {
            std::cout << options.help({""}) << std::endl;
            std::exit(EXIT_SUCCESS);
        }
        return args;
    }


    int Main(int argc, char* argv[]) {
        const auto args = ParseOptions(argc, argv);

        auto crawler = std::make_unique<ycrawler::SimpleCrawler>();
        crawler->Restore(args.config_file_name);
        if (args.move_failed_to_queue) {
            crawler->MoveFailedToQueue();
        }
        crawler->Start();

        return EXIT_SUCCESS;
    }

}  // namespace mode_restore


int main(int argc, char* argv[]) {
    auto mod_chooser = ymod_chooser::ModChooser{"Crawl simple Wikipedia."};
    mod_chooser.AddMode(
        mode_new::MODE_NAME,
        mode_new::Main,
        mode_new::MODE_DESCRIPTION
    );
    mod_chooser.AddMode(
        mode_restore::MODE_NAME,
        mode_restore::Main,
        mode_restore::MODE_DESCRIPTION
    );
    return mod_chooser.Run(argc, argv);
}
