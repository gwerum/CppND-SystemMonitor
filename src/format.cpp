#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long total_seconds[[maybe_unused]]) {
    // Convert total seconds to hours, seconds and minutes
    int seconds, minutes, hours;
    minutes = total_seconds / 60;
    seconds = total_seconds % 60;
    hours = minutes / 60;
    minutes = minutes % 60;
    // Convert integers to string of format HH:MM:SS
    string elapsed_time = std::to_string(hours) + ":" +
                          std::to_string(minutes) + ":" +
                          std::to_string(seconds);
    return elapsed_time; 
}