#include "mod_chooser.h"

#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

#include <cstdlib>


template <typename T>
static void PrintHelp(
    const std::string& error_message,
    const std::string& executable_name, const std::string& title, const T& mapping_,
    std::ostream& output
){
    if (!error_message.empty()) {
        output << "ERROR: " << error_message << '\n';
    }

    output << title << "\n\n";
    output << "Usage: " << executable_name << " [OPTIONS]" << '\n';
    output << '\n';
    output << "Modes:\n";
    for (const auto& value: mapping_) {
        output << '\t' << value.first << '\t' << value.second.description << '\n';
    }
    output << '\n';
    output << "\t{-?|-h|--help}\tTo see this message\n";

    output.flush();
}


ymod_chooser::ModChooser::ModChooser(const std::string& title)
    : title_(title) {
}


void ymod_chooser::ModChooser::AddMode(
    const std::string& name, MainFunction&& main, const std::string description
){
    if (mapping_.find(name) != mapping_.end()) {
        throw std::runtime_error{std::string{"Already have \""} + name + "\" mode"};
    }

    mapping_.insert({name, {std::forward<MainFunction>(main), description}});
}


int ymod_chooser::ModChooser::Run(int argc, char* argv[]) const {
    if (argc < 2) {
        PrintHelp("Not enough arguments", argv[0], title_, mapping_, std::cerr);
        return EXIT_FAILURE;
    }

    const auto first = std::string{argv[1]};
    if ("-h" == first || "-?" == first || "--help" == first) {
        PrintHelp("", argv[0], title_, mapping_, std::cout);
        return EXIT_SUCCESS;
    }

    const auto it = mapping_.find(first);
    if (it != mapping_.end()) {
        return it->second.main(argc - 1, argv + 1);
    } else {
        PrintHelp(std::string{"Unknown mode "} + first, argv[0], title_, mapping_, std::cerr);
        return EXIT_FAILURE;
    }
}
