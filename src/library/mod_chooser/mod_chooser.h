#pragma once

#include <functional>
#include <ostream>
#include <string>
#include <unordered_map>


namespace ymod_chooser {

    class ModChooser {
    public:
        using MainFunction = std::function<int(int argc, char* argv[])>;

        explicit ModChooser(const std::string& title);

        void AddMode(const std::string& name, MainFunction&& main, const std::string description);

        int Run(int argc, char* argv[]) const;

    private:
        struct FunctionWithDescription {
            MainFunction main;
            std::string description;
        };

        std::string title_;
        std::unordered_map<std::string, FunctionWithDescription> mapping_;
    };

}  // namespace ymod_chooser

