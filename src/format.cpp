#include <string>

#include "format.h"

using std::string;

string Format::convertTimeToDoubleDigitString(int time) {
    string time_string;
    if (time < 10 ) {
        time_string = "0" + std::to_string(time);
    } else {
        time_string = std::to_string(time);
    }
    return time_string;
}

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
    // Convert integers to string with two chars
    string seconds_str, minutes_str, hours_str;
    seconds_str = convertTimeToDoubleDigitString(seconds);
    minutes_str = convertTimeToDoubleDigitString(minutes);
    hours_str = convertTimeToDoubleDigitString(hours);
    // Convert to string format HH:MM:SS
    string elapsed_time = hours_str + ":" +
                          minutes_str + ":" +
                          seconds_str;
    return elapsed_time; 
}