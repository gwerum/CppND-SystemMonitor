#ifndef FORMAT_H
#define FORMAT_H

#include <string>

namespace Format {
std::string convertTimeToDoubleDigitString(int time);

std::string ElapsedTime(long total_seconds);  // TODO: See src/format.cpp
};                                    // namespace Format

#endif