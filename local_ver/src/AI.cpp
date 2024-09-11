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

float Quadratic_Cost(float a, float y)
{
    return (a - y)*(a - y);
}

float dQuadratic_Cost(float a, float y)
{
    return (a - y);
}

float Total_Quadratic_Cost(float *a, float *y, uint32_t size)
{
    float ret = 0.0;
    for(uint32_t u = 0; u < size; ++u)
    {
        ret += (a[u] - y[u]);
        ret *= ret;    
    }
    return ret;
}

float Cross_Entropy_Cost(float a, float y)
{
    //!TODO
    return 0.0;
}

float dCross_Entropy_Cost(float a, float y)
{
    //!TODO
    return 0.0;
}

float Total_Cross_Entropy_Cost(float *a, float *y, uint32_t size)
{
    //!TODO
    return 0.0;
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
    discount = 0.75;
    if(initialiser() == -1)
        exit(1);
}

AI::AI(reward_repartition rep, std::vector<uint32_t> &network_size_arg, std::vector<float (*)(float)> &rectifiers_arg, std::vector<float (*)(float)> &drectifiers_arg, float epsilon_arg, float discount_arg) : r(rep), network_size(network_size_arg), rectifiers(rectifiers_arg), drectifiers(drectifiers_arg), epsilon(epsilon_arg), discount(discount_arg)
{
    if(initialiser() == -1)
        exit(1);
}

AI::AI(reward_repartition rep, std::string filename, float epsilon_arg, float discount_arg) : r(rep), epsilon(epsilon_arg), discount(discount_arg)
{
    if(initialiser(filename) == -1)
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
    for(uint32_t u5 = 0; u5 < network_size.size() - 1; ++u5)
        free(z[u5]);
    z.clear();
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

std::string AI::forward_pass(std::string &env)
{
    std::string s1 = "";
    std::string s2 = "";

    bool s1_active = true;
    
    for(uint32_t s = 0; s < env.size(); ++s)
    {
        if(env[s] == '|')
            s1_active = false;
        else if(s1_active)
            s1 += env[s];
        else
            s2 += env[s];
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
    uint32_t w, h;
    for(h = 0; h < HEIGHT; ++h)
    {
        for(w = 0; w < WIDTH; ++w)
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
                neurons[u] = 0;
        }
        else if(s2[s] >= '0' || s2[s] <= '9')
            neurons[0][u] = neurons[0][u] * 10 + (s2[s] - '0');
    }

    for(uint32_t m = 1; m < network_size.size(); ++m)
    {
        for(uint32_t n = 0; n < network_size[m]; ++n)
        {
            z[m][n] = 0.0;
            for(uint32_t o = 0; o < network_size[m - 1]; ++o)
               z[m][n] += neurons[m - 1][o] * weights[m - 1][n][o];
            z[m][n] += biases[m - 1][n];
            neurons[m][n] = rectifiers[m - 1](z[m][n]);
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
        float *tab = (float *) malloc(network_size[u + 1] * sizeof(float));
        if(tab == NULL)
        {
            std::cout << "Malloc Error"<< std::endl;
            for(uint32_t u2 = 0; u2 < network_size.size(); ++u2)
                free(neurons[u2]);
            for(uint32_t u3 = 0; u3 < network_size.size() - 1; ++u3)
                free(biases[u3]);
            for(uint32_t u4 = 0; u4 < network_size.size() - 1; ++u4)
                free(z[u4]);
            return -1;
        }
        for(uint32_t v = 0; v < network_size[u + 1]; ++v)
            tab[v] = 0.0; 

        z.push_back(tab);

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
            for(uint32_t u5 = 0; u5 < network_size.size() - 1; ++u5)
                free(z[u5]);
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
                for(uint32_t u5 = 0; u5 < network_size.size() - 1; ++u5)
                    free(z[u5]);
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

int32_t AI::initialiser(std::string filepath)
{
    FILE *fp = std::fopen(filepath.c_str(), "rb");
    if(fp == NULL)
    {
        std::cout << "Could not open AI file" << std::endl;
        return -1;
    }

    std::fseek(fp,0,SEEK_END);
    unsigned long size = std::ftell(fp);
    std::fseek(fp,0,SEEK_SET);

    size_t buffer;
    if(std::fread((void*) (&buffer), sizeof(size_t), 1, fp) != 0)
    {
        std::cout << "Could not read AI file" << std::endl;
        std::fclose(fp);
        return -1;
    }
    if(size - std::ftell(fp) < sizeof(uint32_t) * buffer)
    {
        std::cout << "Could not read AI file" << std::endl;
        std::fclose(fp);
        return -1;
    }

    uint32_t buffer2;
    for(uint32_t u = 0; u < buffer; ++u)
    {
        if(std::fread((void*) (&buffer2), sizeof(uint32_t), 1, fp) != 0)
        {
            std::cout << "Could not read AI file" << std::endl;
            std::fclose(fp);
            return -1;
        }
        network_size.push_back(buffer2);
    }

    if(size - std::ftell(fp) < sizeof(uint8_t) * (buffer - 1))
    {
        std::cout << "Could not read AI file" << std::endl;
        std::fclose(fp);
        return -1;
    }

    uint8_t buffer3;
    for(uint32_t u = 0; u < buffer - 1; ++u)
    {
        if(std::fread((void*) (&buffer3), sizeof(uint8_t), 1, fp) != 0)
        {
            std::cout << "Could not read AI file" << std::endl;
            std::fclose(fp);
            return -1;
        }
        if(buffer3 == 0)
        {
            rectifiers.push_back(sigmoid);
            drectifiers.push_back(dsigmoid);
        }
        else if(buffer3 == 1)
        {
            rectifiers.push_back(ReLu);
            drectifiers.push_back(dReLu);
        }
    }

    uint32_t nb = 0;
    for(uint32_t u = 0; u < buffer - 1; ++u)
        nb += (network_size[u] + 1) * network_size[u + 1];

    if(size - std::ftell(fp) < sizeof(float) * nb)
    {
        std::cout << "Could not read AI file" << std::endl;
        std::fclose(fp);
        return -1;
    }

    for(uint32_t u = 0; u < buffer - 1; ++u)
    {
        float *tab = (float *) malloc(network_size[u + 1] * sizeof(float));
        if(tab == NULL)
        {
            std::cout << "Malloc error" << std::endl;
            for(uint32_t v = 0; v < biases.size(); ++v)
                free(biases[v]);
            std::fclose(fp);
            return -1;
        }
        if(fread((void*) tab, sizeof(float), network_size[u + 1],fp) != 0)
        {
            std::cout << "Could not read AI file" << std::endl;
            for(uint32_t v = 0; v < biases.size(); ++v)
                free(biases[v]);
            std::fclose(fp);
            return -1;
        }
        biases.push_back(tab);
    }

    for(uint32_t u = 0; u < buffer - 1; ++u)
    {
        float **tab2 = (float **) malloc(network_size[u + 1] * sizeof(float*));  
        if(tab2 == NULL)
        {
            std::cout << "Malloc error" << std::endl;
            for(uint32_t v = 0; v < biases.size(); ++v)
                free(biases[v]);
            for(uint32_t v = 0; v < weights.size(); ++v)
            {
                for(uint32_t w = 0; w < network_size[v + 1]; ++w)
                    free(weights[v][w]);
                free(weights[v]);
            }
            std::fclose(fp);
            return -1;
        }
        for(uint32_t u2 = 0; u2 < network_size[u + 1]; ++u2)
        {
            tab2[u2] = (float*) malloc(network_size[u] * sizeof(float));
            if(tab2[u2] == NULL)
            {
                std::cout << "Malloc error" << std::endl;
                for(uint32_t v = 0; v < biases.size(); ++v)
                    free(biases[v]);
                for(uint32_t v = 0; v < weights.size(); ++v)
                {
                    for(uint32_t w = 0; w < network_size[v + 1]; ++w)
                        free(weights[v][w]);
                    free(weights[v]);
                }
                for(uint32_t u3 = 0; u3 < u2; ++u3)
                    free(tab2[u3]);
                free(tab2);
                std::fclose(fp);
                return -1;
            }
            if(fread((void*) tab2[u2], sizeof(float), network_size[u], fp) != 0)
            {
                std::cout << "Could not read in AI file" << std::endl;
                for(uint32_t v = 0; v < biases.size(); ++v)
                    free(biases[v]);
                for(uint32_t v = 0; v < weights.size(); ++v)
                {
                    for(uint32_t w = 0; w < network_size[v + 1]; ++w)
                        free(weights[v][w]);
                    free(weights[v]);
                }
                for(uint32_t u3 = 0; u3 < u2; ++u3)
                    free(tab2[u3]);
                free(tab2);
                std::fclose(fp);
                return -1;
            }
        }
        weights.push_back(tab2);
    }

    std::fclose(fp);

    for(uint32_t u = 0; u < buffer; ++u)
    {
        float *tab3 = (float*) malloc(network_size[u] * sizeof(float));
        if(tab3 == NULL)
        {
            std::cout << "Malloc error" << std::endl;
            for(uint32_t v = 0; v < biases.size(); ++v)
                free(biases[v]);
            for(uint32_t v = 0; v < weights.size(); ++v)
            {
                for(uint32_t w = 0; w < network_size[v + 1]; ++w)
                    free(weights[v][w]);
                free(weights[v]);
            }
            for(uint32_t v = 0; v < neurons.size(); ++v)
                free(neurons[v]);
            for(uint32_t v = 0; v < z.size(); ++v)
                free(z[v]);
            return -1;
        }
        for(uint32_t v = 0; v < network_size[u]; ++v)
            tab3[v] = 0.0;
        neurons.push_back(tab3);
        
        tab3 = (float*) malloc(network_size[u] * sizeof(float));
        if(tab3 == NULL)
        {
            std::cout << "Malloc error" << std::endl;
            for(uint32_t v = 0; v < biases.size(); ++v)
                free(biases[v]);
            for(uint32_t v = 0; v < weights.size(); ++v)
            {
                for(uint32_t w = 0; w < network_size[v + 1]; ++w)
                    free(weights[v][w]);
                free(weights[v]);
            }
            for(uint32_t v = 0; v < neurons.size(); ++v)
                free(neurons[v]);
            for(uint32_t v = 0; v < z.size(); ++v)
                free(z[v]);
            return -1;
        }
        for(uint32_t v = 0; v < network_size[u]; ++v)
            tab3[v] = 0.0;
        z.push_back(tab3);
    }


    return 0;
}

int32_t AI::write(std::string filename) const
{
    FILE *fp = std::fopen(filename.c_str(), "wb");
    if(fp == NULL)
    {
        std::cout << "Could not create AI file" << std::endl;
        return -1;
    }
    
    size_t to_write = network_size.size();

    if(std::fwrite((void *) (&to_write), sizeof(network_size.size()), 1, fp) != 0)
    {
        std::cout << "Could not write in AI file" << std::endl;
        std::fclose(fp);
        std::string command = "rm -f " + filename;
        std::system(command.c_str());
        return -1;
    }

    if(std::fwrite(network_size.data(), sizeof(network_size[0]), network_size.size(), fp) != 0)
    {
        std::cout << "Could not write in AI file" << std::endl;
        std::fclose(fp);
        std::string command = "rm -f " + filename;
        std::system(command.c_str());
        return -1;
    }

    uint8_t elem;
    for(uint32_t u = 0; u < network_size.size() - 1; ++u)
    {
        if(rectifiers[u] == sigmoid)
            elem = 0;
        else if(rectifiers[u] == ReLu)
            elem = 1;
        if(std::fwrite((void *) (&elem), sizeof(uint8_t), 1, fp) != 0)
        {
            std::cout << "Could not write in AI file" << std::endl;
            std::fclose(fp);
            std::string command = "rm -f " + filename;
            std::system(command.c_str());
            return -1;
        }
    }

    for(uint32_t u = 0; u < network_size.size() - 1; ++u)
    {
        if(std::fwrite(biases[u], sizeof(float), network_size[u + 1], fp) != 0)
        {
            std::cout << "Could not write in AI file" << std::endl;
            std::fclose(fp);
            std::string command = "rm -f " + filename;
            std::system(command.c_str());
            return -1;
        }
    }

    for(uint32_t u = 0; u < network_size.size() - 1; ++u)
    {
        for(uint32_t v = 0; v < network_size[u + 1]; ++v)
        {
            if(std::fwrite(weights[u][v], sizeof(float), network_size[u], fp) != 0)
            {
                std::cout << "Could not write in AI file" << std::endl;
                std::fclose(fp);
                std::string command = "rm -f " + filename;
                std::system(command.c_str());
                return -1;
            }
        }
    }

    std::fclose(fp);
    return 0;
}

Gradient::Gradient()
{

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

int8_t Gradient::backward_pass(uint32_t selected, bool quad)
{
    if(gradient_set_0() != 0)
        return -1;

    //first

    for(uint32_t u = 0; u < parent->network_size[parent->network_size.size() - 1]; ++u)
    {

        //db
        if(quad)
            dbiases[parent->network_size.size() - 2][u] = dQuadratic_Cost(parent->neurons[parent->network_size.size() - 1][u],u == selected) * parent->drectifiers[parent->network_size.size() - 2](parent->z[parent->network_size.size() - 2][u]);
        else
            dbiases[parent->network_size.size() - 2][u] = dCross_Entropy_Cost(parent->neurons[parent->network_size.size() - 1][u],u == selected) * parent->drectifiers[parent->network_size.size() - 2](parent->z[parent->network_size.size() - 2][u]); 


        //dw 
        for(uint32_t x = 0; x < parent->network_size[parent->network_size.size() - 2]; ++x)
            dweights[parent->network_size.size() - 2][u][x] = dbiases[parent->network_size.size() - 2][u] * parent->neurons[parent->network_size.size() - 2][x];  
    }

    for(uint32_t c = parent->network_size.size() - 3; c >= 0; --c)
    {
        for(uint32_t u = 0; u < parent->network_size[c + 1]; ++u)
        {
            //db
            for(uint32_t v = 0; v < parent->network_size[c + 2]; ++v)
                dbiases[c][u] += dbiases[c + 1][v] * parent->weights[c + 1][v][c];
            dbiases[c][u] *= parent->drectifiers[c](parent->z[c][u]);


            //dw
            for(uint32_t w = 0; w < parent->network_size[c]; ++w)
                dweights[c][u][w] = dbiases[c][u] * parent->neurons[c][w];
        }
        
    }
    return 0;
}

int8_t Gradient::gradient_mult(float mult)
{
    for(uint32_t u = 0; u < parent->network_size.size() - 1; ++u)
    {
        for(uint32_t m = 0; m < parent->network_size[u + 1]; ++m)
        {
            dbiases[u][m] *= mult;
            for(uint32_t n = 0; n < parent->network_size[u]; ++n)
                dweights[u][m][n] *= mult;
        }
    }
    return 0;
}

int8_t Gradient::gradient_add(Gradient &add)
{
    for(uint32_t u = 0; u < parent->network_size.size() - 1; ++u)
    {
        for(uint32_t m = 0; m < parent->network_size[u + 1]; ++m)
        {
            dbiases[u][m] += add.dbiases[u][m];
            for(uint32_t n = 0; parent->network_size[u]; ++n)
                dweights[u][m][n] += add.dweights[u][m][n];
        }
    }
    return 0;
}

int8_t Gradient::gradient_set_0()
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

int8_t Gradient::gradient_apply() const
{
    for(uint32_t u = 0; u < parent->network_size.size() - 1; ++u)
    {
        for(uint32_t m = 0; m < parent->network_size[u + 1]; ++m)
        {
            parent->biases[u][m] -= dbiases[u][m];
            for(uint32_t n = 0; n < parent->network_size[u]; ++n)
                parent->weights[u][m][n] -= dweights[u][m][n];
        }
    }
    return 0;
}

int32_t Gradient::initialiser(std::vector<uint32_t> &network_size_arg)
{
    if(network_size_arg.size() < 2)
    {
        std::cout << "Need multiple neurons layers" << std::endl;
        return -1;
    }


    for(uint32_t u = 0; u < network_size_arg.size() - 1; ++u)
    {
        float *tab = (float *) malloc(network_size_arg[u + 1] * sizeof(float));
        if(tab == NULL)
        {
            std::cout << "Malloc Error"<< std::endl;
            for(uint32_t u3 = 0; u3 < dbiases.size(); ++u3)
                free(dbiases[u3]);
            return -1;
        }
        for(uint32_t v = 0; v < network_size_arg[u + 1]; ++v)
            tab[v] = 0.0; 

        dbiases.push_back(tab);

    }

    for(uint32_t u = 0; u < network_size_arg.size() - 1; ++u)
    {

        float **tab2 = (float **) malloc(network_size_arg[u + 1] * sizeof(float*));
        if(tab2 == NULL)
        {
            std::cout << "Malloc Error"<< std::endl;
            for(uint32_t u3 = 0; u3 < network_size_arg.size() - 1; ++u3)
                free(dbiases[u3]);
            for(uint32_t u4 = 0; u4 < dweights.size(); ++u4)
            {
                for(uint32_t v = 0; v < network_size_arg[u4 + 1]; ++v)
                    free(dweights[u4][v]);
                free(dweights[u4]);
            }
            return -1;
        }
        for(uint32_t v = 0; v < network_size_arg[u + 1]; ++v)
        {
            tab2[v] = (float *) malloc(network_size_arg[u] * sizeof(float));
            if(tab2[v] == NULL)
            {
                std::cout << "Malloc Error"<< std::endl;
                for(uint32_t u3 = 0; u3 < network_size_arg.size() - 1; ++u3)
                    free(dbiases[u3]);
                for(uint32_t u4 = 0; u4 < dweights.size(); ++u4)
                {
                    for(uint32_t v2 = 0; v2 < network_size_arg[u4 + 1]; ++v2)
                        free(dweights[u4][v2]);
                    free(dweights[u4]);
                }
                for(uint32_t v3 = 0; v3 < v; ++v3)
                    free(tab2[v3]);
                free(tab2);
                return -1;
            }
            
            for(uint32_t v2 = 0; v2 < network_size_arg[u]; ++v2)
                tab2[v][v2] = 0.0; 

        }

        dweights.push_back(tab2);
        
    }

    return 0;
}
