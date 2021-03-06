#ifndef LOG_H
#define LOG_H
#include <stdlib.h>
#include <iostream>

class Log
{
public:
    Log();
    ~Log();
    static inline void log(std::ostream& os, const std::string& msg);
};

void Log::log(std::ostream& os, const std::string& msg)
{
    os << msg << " error: " << SDL_GetError() << std::endl;
}
#endif