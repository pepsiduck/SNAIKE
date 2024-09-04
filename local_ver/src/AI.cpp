#include "../headers/AI.h"

AI::AI()
{

}

AI::~AI()
{

}

std::string AI::action(std::string &env)
{
    uint8_t direction = std::rand() % 4;
    if(direction == 0)
        return "UP";
    if(direction == 1)
        return "RIGHT";
    if(direction == 2)
        return "LEFT";
    return "DOWN";
}
