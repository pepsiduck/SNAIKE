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

#define PAUSE 0.075
#define PLAYERS 6
#define WIDTH 35
#define HEIGHT 25
#define NB_APPLES 25
#define NB_WALLS 25

#define GENS 10

int8_t train(uint8_t arg);

#endif
