#ifndef TRAIN_H_INCLUDED
#define TRAIN_H_INCLUDED

#include <inttypes.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <utility>

#include "AI.h"
#include "consts.h"

extern std::string directions[PLAYERS];

int8_t snake_step(std::vector<std::pair<int32_t,int32_t>> *snakes, std::pair<int32_t,int32_t> *apples, std::pair<int32_t,int32_t> *walls, bool* alive, float *rewards, Gradient *gradients, AI *ai, uint32_t game_step);

int8_t snake_reset(std::vector<std::pair<int32_t,int32_t>> *snakes, std::pair<int32_t,int32_t> *apples, std::pair<int32_t,int32_t> *walls, bool* alive);

void rewards_set_0(float **rewards);

int8_t train(std::string input);

#endif
