#ifndef AI_H_INCLUDED
#define AI_H_INCLUDED

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <inttypes.h>

#include "consts.h"

typedef struct reward_repartition reward_repartition;
struct reward_repartition
{
    int32_t advance;
    int32_t eat;
    int32_t kill;
    int32_t die;
};

float sigmoid(float arg);
float ReLu(float arg);
float dsigmoid(float arg);
float dReLu(float arg);

/*
*
*   sigmoid = 0
*
*   ReLu = 1
*/

float Quadratic_Cost(float a, float y);
float dQuadratic_Cost(float a, float y);
float Total_Quadratic_Cost(float *a, float *y, uint32_t size);

float Cross_Entropy_Cost(float a, float y);
float dCross_Entropy_Cost(float a, float y);
float Total_Cross_Entropy_Cost(float *a, float *y, uint32_t size);

class AI
{
public:
    //!FUNC

    AI();
    AI(reward_repartition rep, std::vector<uint32_t> &network_size_arg, std::vector<float (*)(float)> &rectifiers_arg, std::vector<float (*)(float)> &drectifiers_arg, float epsilon_arg, float discount_arg);
    AI(reward_repartition rep, std::string filename, float epsilon_arg, float discount_arg);
    virtual ~AI();
    std::string action(std::string &env);
    std::string forward_pass(std::string &env);
    float getEpsilon() const;
    void setEpsilon(float arg);
    int32_t initialiser();
    int32_t initialiser(std::string filepath);
    int32_t write(std::string filename) const;
    

    //!VAR

    reward_repartition r;
    std::vector<uint32_t> network_size;
    std::vector<float*> neurons;
    std::vector<float*> z;
    std::vector<float**> weights;
    std::vector<float*> biases;
    std::vector<float (*)(float)> rectifiers;
    std::vector<float (*)(float)> drectifiers;
    float epsilon;
    float discount;
protected:
};

class Gradient
{
public:
    //!FUNC
    Gradient(AI *arg);
    virtual ~Gradient();
    int8_t backward_pass(uint32_t selected, bool quad);
    int8_t gradient_mult(float mult);
    int8_t gradient_add(Gradient *add);
    int8_t gradient_set_0();
    int8_t gradient_apply() const;
    int32_t initialiser(std::vector<uint32_t> &network_size_arg);

    //!VAR

    std::vector<float**> dweights;
    std::vector<float*> dbiases;
    AI* parent;
protected:
};

#endif
