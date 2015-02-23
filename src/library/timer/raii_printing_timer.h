#pragma once

#include <library/timer/timer.h>

#include <iostream>
#include <ostream>
#include <string>

class RaiiPrintingTimer {
public:
    RaiiPrintingTimer(const std::string& measurement_name, std::ostream& output = std::cout)
        : timer_()
        , measurement_name_(measurement_name)
        , output_(output)
    {
        output_ << measurement_name << "..." << std::endl;
        timer_.Start();
    }

    ~RaiiPrintingTimer() {
        const auto time = timer_.Stop().count();
        output_ << measurement_name_ << "...DONE in " << time << std::endl;
    }

private:
    Timer timer_;
    std::string measurement_name_;
    std::ostream& output_;
};
