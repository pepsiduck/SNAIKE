#include "../headers/train.h"

std::string directions[PLAYERS] = {"","","","","",""};

int8_t snake_step(std::vector<std::pair<int32_t,int32_t>> *snakes, std::pair<int32_t,int32_t> *apples, std::pair<int32_t,int32_t> *walls, bool* alive, float *rewards, Gradient **gradients, AI *ai, uint32_t game_step)
{
    for(uint32_t r = 0; r < PLAYERS; ++r)
        rewards[r] = 0.0;

    //DISPLAY

    std::string display(WIDTH * HEIGHT, '0');
    for(uint32_t a = 0; a < NB_APPLES; ++a)
        display[apples[a].first + WIDTH * apples[a].second] = '1';
    for(uint32_t w = 0; w < NB_WALLS; ++w)
        display[walls[w].first + WIDTH * walls[w].second] = '2';

    for(uint32_t p = 0; p < PLAYERS; ++p)
    {
         if(!alive[p])
             continue;
         display[snakes[p][0].first + WIDTH * snakes[p][0].second] = char('A' + p);
         for(uint32_t b = 1; b < snakes[p].size(); ++b)
              display[snakes[p][b].first + WIDTH * snakes[p][b].second] = char('a' + p);
    }

     //DECISION

    for(uint32_t p = 0; p < PLAYERS; ++p)
    {
        if(!alive[p])
            continue;
        std::string env = display;
        for(uint32_t c = 0; c < env.size(); ++c)
        {
            if(env[c] == char('A' + p))
                env[c] = '*';
            else if(env[c] == char('a' + p))
                 env[c] = '-';
            else if(((env[c] >= 'A') && (env[c] < char('A' + PLAYERS)))||((env[c] >= 'a') && (env[c] < char('a' + PLAYERS))))
                env[c] = '_';
        }

        env += "|";

        for(uint32_t p2 = 0; p2 < PLAYERS; ++p2)
        {
            if(p2 == p)
                continue;
            env += "(" + std::to_string(snakes[p2][0].second) + ", " + std::to_string(snakes[p2][0].first) + ")";
        }

        directions[p] = ai->action(env);
    }

    //CALCULATION

    for(uint32_t p = 0; p < PLAYERS; ++p)
    {   
        
        if(!alive[p])
            continue;

        for(uint32_t b = snakes[p].size() - 1; b > 0; --b)
            snakes[p][b] = snakes[p][b - 1];          

        if(directions[p] == "UP")
        {
            snakes[p][0].second--;
            gradients[p]->backward_pass(0,true);
        }
        else if(directions[p] == "DOWN")
        {
            snakes[p][0].second++;
            gradients[p]->backward_pass(3,true);
        }
        else if(directions[p] == "RIGHT")
        {
            snakes[p][0].first++;
            gradients[p]->backward_pass(1,true);
        }
        else if(directions[p] == "LEFT")
        {
            snakes[p][0].first--;
            gradients[p]->backward_pass(2,true);
        }
        else
            return -1;

        rewards[p] += ai->r.advance;

        for(uint32_t a = 0; a < NB_APPLES; ++a)
        {
            if(snakes[p][0] == apples[a])
            {
                snakes[p].push_back(snakes[p][snakes[p].size() - 1]);
                rewards[p] += ai->r.eat;

                bool out;

                std::pair<int32_t,int32_t> buffer;

                for(uint32_t t = 0; t < 50; ++t)
                {
                    if(t == 49)
                    {
                        apples[a].first = -2;
                        apples[a].second = -2;
                    }

                    std::pair<int32_t,int32_t> buffer = std::pair<int32_t,int32_t>(rand() % WIDTH, rand() % HEIGHT);

                    out = false;
                    for(uint32_t s = 0; s < PLAYERS; ++s)
                    {
                        for(uint32_t b = 0; b < snakes[p].size(); ++b)
                        {
                            if(buffer == snakes[p][b])
                            {
                                out = true;
                                break; 
                            }
                        }
                        if(out)
                            break;
                    }
                    if(out)
                        continue;

                    for(uint32_t w = 0; w < NB_WALLS; ++w)
                    {
                        if(walls[w] == buffer)
                        {
                            out = true;
                            break;
                        }
                    }
                }
                if(out)
                    continue;

                for(uint32_t a2 = 0; a2 < NB_WALLS; ++a2)
                {
                    if(apples[a2] == buffer)
                    {
                         out = true;
                         break;
                    }
                }
                if(out)
                    continue;

                apples[a] = buffer;
                break;
            }
            break;
        }
    }

            //DEAD CHECK

    bool to_be_alive[PLAYERS];

    for(uint32_t p = 0; p < PLAYERS; ++p)
    {
        to_be_alive[p] = alive[p];

        if(alive[p])
        {

            if(snakes[p][0].first < 0 || snakes[p][0].first >= WIDTH)
            {
                to_be_alive[p] = false;
                rewards[p] += ai->r.die;
                continue;
            }

            if(snakes[p][0].second < 0 || snakes[p][0].second >= HEIGHT)
            {
                to_be_alive[p] = false;
                rewards[p] += ai->r.die;
                continue;
            }
                    
            for(uint32_t w = 0; w < NB_WALLS; ++w)
            {
                if(walls[w] == snakes[p][0])
                {
                    to_be_alive[p] = false;
                    rewards[p] += ai->r.die;
                    break;
                }
            }
            if(!to_be_alive[p])
                continue;

            for(uint32_t s = 0; s < PLAYERS; ++s)
            {
                if(!alive[s])
                    continue;
                for(uint32_t b = s == p; b < snakes[s].size(); ++b)
                {
                    if(snakes[p][0] == snakes[s][b])
                    {
                        to_be_alive[p] = false;
                        rewards[p] += ai->r.die;
                        if(s != p)
                            rewards[s] += ai->r.kill;
                        break;
                    }
                }
                if(!to_be_alive[p])
                    break;
            }

        }

    }

    for(uint32_t p = 0; p < PLAYERS; ++p)
         alive[p] = to_be_alive[p];

    int8_t alive_players = 0;
    for(uint32_t p = 0; p < PLAYERS; ++p)
        alive_players += alive[p] == true;

    return alive_players > 1; //No more than 127 players

}

int8_t snake_reset(std::vector<std::pair<int32_t,int32_t>> *snakes, std::pair<int32_t,int32_t> *apples, std::pair<int32_t,int32_t> *walls, bool* alive)
{
    //INIT    
    //SNAKE  
    for(uint32_t p = 0; p < PLAYERS; ++p)
    {      

        alive[p] = true;
        snakes[p].clear();


        for(uint32_t t = 0; t < 50; ++t)
        {

            if(t == 49)
            {
                std::cout << "Cannot spawn : too many players" << std::endl;
                return -1;
            }

            std::pair<int32_t,int32_t> buffer(rand() % WIDTH, rand() % HEIGHT);
            bool out = false;
            for(uint32_t y = 0; y < p; y++)
            {
                if(buffer == snakes[y][0]) 
                {
                    out = true;
                    break;
                }                    
            }
            if(out) 
                continue;
    
            snakes[p].push_back(buffer);

            break;
                
        }
    }

    //WALLS
    for(uint32_t w = 0; w < NB_WALLS; ++w)
    {
        for(uint32_t t = 0; t < 50; ++t)
        {
            if(t == 49)
            {
                std::cout << "Cannot spawn : too many walls & players" << std::endl;
                return -1;
            }

            std::pair<int32_t,int32_t> buffer(rand() % WIDTH, rand() % HEIGHT);
            bool out = false;
            for(uint32_t s = 0; s < PLAYERS; ++s)
            {
                if(buffer == snakes[s][0])
                {
                    out = true;
                    break;
                }   
            }
            if(out)
                continue;
        
            for(uint32_t y = 0; y < w; ++y)
            {
                if(buffer == walls[y])
                {
                    out = true;
                    break;
                } 
            }
            if(out)
                continue;

            walls[w] = buffer;
    
            break;
        }
    }

    //APPLES
    for(uint32_t a = 0; a < NB_APPLES; ++a)
    {
        for(uint32_t t = 0; t < 50; ++t)
        {
            if(t == 49)
            {
                std::cout << "Cannot spawn : too many apples & players" << std::endl;
                return -1;
            }

            std::pair<int32_t,int32_t> buffer(rand() % WIDTH, rand() % HEIGHT);
            bool out = false;
            for(uint32_t s = 0; s < PLAYERS; ++s)
            {
                if(buffer == snakes[s][0])
                {
                    out = true;
                    break;
                }    
            }
            if(out)
                continue;

            for(uint32_t w = 0; w < NB_WALLS; ++w)
            {
                if(buffer == walls[w])
                {
                    out = true;
                    break;
                } 
            }
            if(out)
                continue;
        
            for(uint32_t y = 0; y < a; ++y)
            {
                if(buffer == apples[y])
                {
                    out = true;
                    break;
                } 
            }
            if(out)
                continue;

            apples[a] = buffer;
    
            break;
        }
    } 

    return 0;   
}

void rewards_set_0(float **rewards)
{
    for(uint32_t p = 0; p < PLAYERS; ++p)
    {
        for(uint32_t g = 0; g < GAME_STEPS; ++g)
            rewards[g][p] = 0.0;
    }
}

int8_t train(std::string input)
{

    std::cout << "Beginning training" << std::endl;

    std::cout << "Allocating memory" << std::endl;

    AI new_AI = AI();   

    std::vector<std::pair<int32_t,int32_t>> snakes[PLAYERS];
    std::pair<int32_t,int32_t> apples[NB_APPLES];
    std::pair<int32_t,int32_t> walls[NB_WALLS];
    bool alive[PLAYERS];
    
    //float rewards[GAME_STEPS][PLAYERS];
    float **rewards = (float**) malloc(GAME_STEPS * sizeof(float*));
    if(rewards == NULL)
    {
        std::cout << "Malloc error" << std::endl;
        return -1;
    }
    for(uint32_t g = 0; g < GAME_STEPS; ++g)
    {
        rewards[g] = (float*) malloc(PLAYERS * sizeof(float));
        if(rewards[g] == NULL)
        {
            std::cout << "Malloc error" << std::endl;
            for(uint32_t g2 = 0; g2 < g; ++g2)
                free(rewards[g2]);
            free(rewards);
            return -1;
        }
    }

    float add;

    

    Gradient* temp_gradients[GAME_STEPS][PLAYERS];
    for(uint32_t g = 0; g < GAME_STEPS; ++g)
    {
        for(uint32_t p = 0; p < PLAYERS; ++p)
            temp_gradients[g][p] = new Gradient(&new_AI);
    }

    Gradient* gradients[GAME_STEPS][PLAYERS];
    for(uint32_t g = 0; g < GAME_STEPS; ++g)
    {
        for(uint32_t p = 0; p < PLAYERS; ++p)
            gradients[g][p] = new Gradient(&new_AI);
    }

    

    uint32_t survived[PLAYERS];
    float total_reward[PLAYERS];

    std::cout << "Beginning games" << std::endl;

    for(uint32_t u = 0; u < GENS / BATCH_SIZE; ++u)
    {

        std::cout << "Generation " << u << std::endl;
        std::cout << std::endl;
        
        for(uint32_t g = 0; g < GAME_STEPS; ++g)
        {
            for(uint32_t p = 0; p < PLAYERS; ++p)
                gradients[g][p]->gradient_set_0();
        }

        for(uint32_t b = 0; b < BATCH_SIZE; ++b)
        {

            std::cout << "Game " << b << std::endl;
            std::cout << std::endl;            

            rewards_set_0(rewards);

            
            for(uint32_t g = 0; g < GAME_STEPS; ++g)
            {
                for(uint32_t p = 0; p < PLAYERS; ++p)
                    temp_gradients[g][p]->gradient_set_0();
            }

            
            if(snake_reset(snakes,apples,walls,alive) == -1)
                return -1;

            for(uint32_t p = 0; p < PLAYERS; ++p)
                survived[p] = 0;

            //GAME
            int8_t cont = 1;
            uint32_t game_steps = 0;
            while(cont && game_steps < GAME_STEPS)
            {

                std::printf("Gamestep %u", game_steps);
                for(uint32_t p = 0; p < PLAYERS; ++p)
                {
                    std::printf(" | ");
                    if(alive[p])
                        std::printf("Size_");
                    else
                        std::printf("Dead_");
                    std::printf("%u : %lu",p,snakes[p].size());
                }
                std::cout << std::endl;

                cont = snake_step(snakes,apples,walls,alive,rewards[game_steps],temp_gradients[game_steps],&new_AI,game_steps);

                for(uint32_t p = 0; p < PLAYERS; ++p)
                {
                    if(alive[p])
                        survived[p]++;
                }

                if(cont == -1)
                    return -1;
                
                game_steps++;

            }

            std::printf("Gamestep %u", game_steps);
            for(uint32_t p = 0; p < PLAYERS; ++p)
            {
                std::printf(" | ");
                if(alive[p])
                    std::printf("Size_");
                else
                    std::printf("Dead_");
                std::printf("%u : %lu",p,snakes[p].size());
            }
            std::cout << std::endl;
            std::cout << std::endl;

            float avg_reward = 0;

            for(uint32_t p = 0; p < PLAYERS; ++p)
            {

                total_reward[p] = 0;

                for(uint32_t g = 0; g < GAME_STEPS; ++g)
                {
                    add = 0.0;
                    for(uint32_t h = g; h < GAME_STEPS; ++h)
                        add += rewards[h][p] * std::pow(new_AI.discount,h - g);      
                    temp_gradients[g][p]->gradient_mult(add);
                    gradients[g][p]->gradient_add(temp_gradients[g][p]);

                    if(rewards[g][p] != 0)
                        total_reward[p] += rewards[g][p];
                }

                std::cout << "Player " << p << " : Survived Frames : " << survived[p] << " | Total Reward : " << total_reward[p] << " | Size : " << snakes[p].size() << std::endl;    
                avg_reward += total_reward[p];    

            }

            std::cout << "Avg reward : " << avg_reward / PLAYERS << std::endl;
            std::cout << std::endl;


        }

        for(uint32_t p = 0; p < PLAYERS; ++p)
        {
            for(uint32_t g = 0; g < GAME_STEPS; ++g)
                gradients[g][p]->gradient_apply();
        }

    }

    std::cout << "Freeing memory" << std::endl;

    for(uint32_t g = 0; g < GAME_STEPS; ++g)
        free(rewards[g]);
    free(rewards);

    for(uint32_t g = 0; g < GAME_STEPS; ++g)
    {
        for(uint32_t p = 0; p < PLAYERS; ++p)
            delete temp_gradients[g][p];
    }

    for(uint32_t g = 0; g < GAME_STEPS; ++g)
    {
        for(uint32_t p = 0; p < PLAYERS; ++p)
            delete gradients[g][p];
    }

    std::string response = "";

    while(response != "y" || response != "n")
    {
        std::cout << "Saving data [y/n]?" << std::endl;
        std::cin >> response;
    }

    if(response == "y")
    {
        if(new_AI.write(input) != 0)
            return -1;
        std::cout << "Data saved" << std::endl;
    }

    return 0;
}
