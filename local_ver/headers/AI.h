#ifndef AI_H_INCLUDED
#define AI_H_INCLUDED

#include <iostream>
#include <string>
#include <inttypes.h>

class AI
{
public:
    AI();
    virtual ~AI();
    std::string action(std::string &env);
protected:
};

#endif
