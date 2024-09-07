#include "../headers/AI.h"

float sigmoid(float arg)
{
    return 1.0 / (1.0 + std::exp(-arg));
}

float ReLu(float arg)
{
    if(arg < 0)
        return 0;
    return arg;
}

float dsigmoid(float arg)
{
    float ret = sigmoid(arg);
    return ret * (1 - ret);
}

float dReLu(float arg)
{
    return arg >= 0;
}

AI::AI()
{
    r.advance = -1;
    r.eat = 10;
    r.kill = 100;
    r.die = -30;
    network_size.push_back(887);
    network_size.push_back(10);
    network_size.push_back(4);
    rectifiers.push_back(sigmoid);
    rectifiers.push_back(sigmoid);
    drectifiers.push_back(dsigmoid);
    drectifiers.push_back(dsigmoid);
    epsilon = 1;
    if(initialiser() == -1)
        exit(1);
}

AI::AI(reward_repartition rep, std::vector<uint32_t> &network_size_arg, std::vector<float (*)(float)> &rectifiers_arg, std::vector<float (*)(float)> &drectifiers_arg, float epsilon_arg) : r(rep), network_size(network_size_arg), rectifiers(rectifiers_arg), drectifiers(drectifiers_arg), epsilon(epsilon_arg)
{
    if(initialiser() == -1)
        exit(1);
}

AI::~AI()
{
    for(uint32_t u = 0; u < network_size.size(); ++u)
        free(neurons[u]);
    neurons.clear();
    for(uint32_t u2 = 0; u2 < network_size.size() - 1; ++u2)
        free(biases[u2]);
    biases.clear();
    for(uint32_t u3 = 0; u3 < network_size.size() - 1; ++u3)
    {
        for(uint32_t u4 = 0; u4 < network_size[u3 + 1]; ++u4)
            free(weights[u3][u4]);
        free(weights[u3]);
    }
    weights.clear();
    network_size.clear();
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

std::string AI:forward_pass(std::string &env)
{
    std::string s1 = "";
    std::string s2 = "";
    
    while(std::getline(env, s2, '|'))
    {
        if(s1 == "")
        {
            s1 = s2;
            s2 = "";
        }
    }

    uint32_t u = 0;
    while(u < s1.size())
    {
        if(s1[u] == '0')
            neurons[0][u] = 0.0;
        else if(s1[u] == '1')
            neurons[0][u] = 1.0;
        else
            neurons[0][u] = -1.0;
        ++u;
    }

    bool out = false;
    for(uint32_t h = 0; h < HEIGHT; ++h)
    {
        for(uint32_t w = 0; w < WIDTH; ++w)
        {
            if(s1[h * WIDTH + w] == '*')
            {
                out = true;
                break;
            }
        }
        if(out)
            break;
    }
    
    neurons[0][u] = w;
    neurons[0][u + 1] = h;
    u += 2;

    for(uint32_t s = 0; s < s2.size(); ++s)
    {
        if(s2[s] == ',' || s2[s] == ')')
        {
            ++u;
            if(u < network_size.size())
                neurons[u] = 0.0;
        }
        else if(s2[s] >= '0' || s2[s] <= '9')
            neurons[0][u] = neurons[0][u] * 10 + (s2[s] - '0');
    }

    for(uint32_t m = 1; m < network_size(); ++m)
    {
        for(uint32_t n = 0; n < network_size[m]; ++n)
        {
            neurons[m][n] = 0.0;
            for(uint32_t o = 0; o < network_size[m - 1]; ++o)
                neurons[m][n] += neurons[m - 1][o] * weights[m - 1][n][o];
            neurons[m][n] = rectifiers[m - 1](neurons[m][n] + biases[m - 1][n]);
        }
    }
    
    float div = 0;
    for(uint32_t l = 0; l < network_size[network_size.size() - 1]; ++l)
    {
        if(neurons[network_size.size() - 1][l] > 86.0)
            neurons[network_size.size() - 1][l] = 86.0;
        neurons[network_size.size() - 1][l] = std::exp(neurons[network_size.size() - 1][l]);
        div += neurons[network_size.size() - 1][l];
    }
    for(uint32_t l = 0; l < network_size[network_size.size() - 1]; ++l)
        neurons[network_size.size() - 1][l] /= div;

    if(epsilon != 0.0)
    {
        if((std::rand() % 1000) / 1000.0 < epsilon)
        {
            for(uint32_t l = 1; l < network_size[network_size.size() - 1]; ++l)
                neurons[network_size.size() - 1][l] += neurons[network_size.size() - 1][l - 1];
            div = (std::rand() % 1000) / 1000.0;
            if(div < neurons[network_size.size() - 1][0])
                return "UP";
            if(div < neurons[network_size.size() - 1][1])
                return "RIGHT";
            if(div < neurons[network_size.size() - 1][2])
                return "LEFT";
            return "DOWN";
        }
    }

    uint32_t max_index = 0;
    for(uint32_t l = 1; l < network_size[network_size.size() - 1]; ++l)
    {
        if(neurons[network_size.size() - 1][l] > neurons[network_size.size() - 1][max_index])
            max_index = l;
    }

    if(max_index == 0)
        return "UP";
    else if(max_index == 1)
        return "RIGHT";
    else if(max_index == 2)
        return "LEFT";
    return "DOWN";

}

float AI::getEpsilon() const
{
    return epsilon;
}

void AI::setEpsilon(float arg)
{
    epsilon = arg;
}

int32_t AI::initialiser()
{
    if(network_size.size() < 2)
    {
        std::cout << "Need multiple neurons layers" << std::endl;
        return -1;
    }

    for(uint32_t u = 0; u < network_size.size(); ++u)
    {
        float *tab = (float *) malloc(network_size[u] * sizeof(float));
        if(tab == NULL)
        {
            std::cout << "Malloc Error"<< std::endl;
            for(uint32_t u2 = 0; u2 < neurons.size(); ++u2)
                free(neurons[u2]);
            return -1;
        }
        for(uint32_t v = 0; v < network_size[u]; ++v)
            tab[v] = 0.0;
        neurons.push_back(tab);
    }

    for(uint32_t u = 0; u < network_size.size() - 1; ++u)
    {
        float *tab = (float *) malloc(network_size[u + 1] * sizeof(float));
        if(tab == NULL)
        {
            std::cout << "Malloc Error"<< std::endl;
            for(uint32_t u2 = 0; u2 < network_size.size(); ++u2)
                free(neurons[u2]);
            for(uint32_t u3 = 0; u3 < biases.size(); ++u3)
                free(biases[u3]);
            return -1;
        }
        for(uint32_t v = 0; v < network_size[u + 1]; ++v)
            tab[v] = ((std::rand() % 1000) / 10.0) - 50.0; 

        biases.push_back(tab);

    }

    for(uint32_t u = 0; u < network_size.size() - 1; ++u)
    {

        float **tab2 = (float **) malloc(network_size[u + 1] * sizeof(float*));
        if(tab2 == NULL)
        {
            std::cout << "Malloc Error"<< std::endl;
            for(uint32_t u2 = 0; u2 < network_size.size(); ++u2)
                free(neurons[u2]);
            for(uint32_t u3 = 0; u3 < network_size.size() - 1; ++u3)
                free(biases[u3]);
            for(uint32_t u4 = 0; u4 < weights.size(); ++u4)
            {
                for(uint32_t v = 0; v < network_size[u4 + 1]; ++v)
                    free(weights[u4][v]);
                free(weights[u4]);
            }
            return -1;
        }
        for(uint32_t v = 0; v < network_size[u + 1]; ++v)
        {
            tab2[v] = (float *) malloc(network_size[u] * sizeof(float));
            if(tab2[v] == NULL)
            {
                std::cout << "Malloc Error"<< std::endl;
                for(uint32_t u2 = 0; u2 < network_size.size(); ++u2)
                    free(neurons[u2]);
                for(uint32_t u3 = 0; u3 < network_size.size() - 1; ++u3)
                    free(biases[u3]);
                for(uint32_t u4 = 0; u4 < weights.size(); ++u4)
                {
                    for(uint32_t v2 = 0; v2 < network_size[u4 + 1]; ++v2)
                        free(weights[u4][v2]);
                    free(weights[u4]);
                }
                for(uint32_t v3 = 0; v3 < v; ++v3)
                    free(tab2[v3]);
                free(tab2);
                return -1;
            }
            
            for(uint32_t v2 = 0; v2 < network_size[u]; ++v2)
                tab2[v][v2] = ((std::rand() % 1000) / 10.0) - 50.0; 

        }

        weights.push_back(tab2);
        
    }

    return 0;
    
}

Gradient::Gradient(AI *arg)
{
    parent = arg;
    if(initialiser(arg->network_size) != 0)
        exit(1);
}

Gradient::~Gradient()
{
    for(uint32_t u = 0; u < parent->network_size.size() - 1; ++u)
        free(dbiases[u]);
    dbiases.clear();
    for(uint32_t u3 = 0; u3 < parent->network_size.size() - 1; ++u3)
    {
        for(uint32_t u4 = 0; u4 < parent->network_size[u3 + 1]; ++u4)
            free(dweights[u3][u4]);
        free(dweights[u3]);
    }
    dweights.clear();
}

int8_t backward_pass(uint32_t selected)
{
    for(uint32_t u = 0; u < parent->network_size.size() - 1; ++u)
    {
        for(uint32_t m = 0; m < parent->network_size[u + 1]; ++m)
        {
            dbiases[u][m] = 0.0;
            for(uint32_t n = 0; parent->network_size[u]; ++n)
                dweights[u][m][n] = 0.0;
        }
    }
    return 0;
}

int8_t gradient_add(float mult) const
{
    for(uint32_t u = 0; u < parent->network_size.size() - 1; ++u)
    {
        for(uint32_t m = 0; m < parent->network_size[u + 1]; ++m)
        {
            parent->biases[u][m] += mult * dbiases[u][m];
            for(uint32_t n = 0; n < parent->network_size[u]; ++n)
                parent->weights[u][m][n] += mult * dweights[u][m][n];
        }
    }
    return 0;
}

int32_t initaliser(std::vector<uint32_t> &network_size_arg)
{
    if(network_size.size() < 2)
    {
        std::cout << "Need multiple neurons layers" << std::endl;
        return -1;
    }


    for(uint32_t u = 0; u < network_size.size() - 1; ++u)
    {
        float *tab = (float *) malloc(network_size[u + 1] * sizeof(float));
        if(tab == NULL)
        {
            std::cout << "Malloc Error"<< std::endl;
            for(uint32_t u3 = 0; u3 < dbiases.size(); ++u3)
                free(dbiases[u3]);
            return -1;
        }
        for(uint32_t v = 0; v < network_size[u + 1]; ++v)
            tab[v] = 0.0; 

        dbiases.push_back(tab);

    }

    for(uint32_t u = 0; u < network_size.size() - 1; ++u)
    {

        float **tab2 = (float **) malloc(network_size[u + 1] * sizeof(float*));
        if(tab2 == NULL)
        {
            std::cout << "Malloc Error"<< std::endl;
            for(uint32_t u3 = 0; u3 < network_size.size() - 1; ++u3)
                free(dbiases[u3]);
            for(uint32_t u4 = 0; u4 < dweights.size(); ++u4)
            {
                for(uint32_t v = 0; v < network_size[u4 + 1]; ++v)
                    free(dweights[u4][v]);
                free(dweights[u4]);
            }
            return -1;
        }
        for(uint32_t v = 0; v < network_size[u + 1]; ++v)
        {
            tab2[v] = (float *) malloc(network_size[u] * sizeof(float));
            if(tab2[v] == NULL)
            {
                std::cout << "Malloc Error"<< std::endl;
                for(uint32_t u3 = 0; u3 < network_size.size() - 1; ++u3)
                    free(dbiases[u3]);
                for(uint32_t u4 = 0; u4 < dweights.size(); ++u4)
                {
                    for(uint32_t v2 = 0; v2 < network_size[u4 + 1]; ++v2)
                        free(dweights[u4][v2]);
                    free(dweights[u4]);
                }
                for(uint32_t v3 = 0; v3 < v; ++v3)
                    free(tab2[v3]);
                free(tab2);
                return -1;
            }
            
            for(uint32_t v2 = 0; v2 < network_size[u]; ++v2)
                tab2[v][v2] = 0.0; 

        }

        dweights.push_back(tab2);
        
    }

    return 0;
}
