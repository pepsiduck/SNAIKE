#include "../headers/train.h"

int8_t train(uint8_t arg)
{

    AI new_AI = AI();   

    std::vector<std::pair<int32_t,int32_t>> snakes[PLAYERS];
    std::pair<int32_t,int32_t> apples[NB_APPLES];
    std::pair<int32_t,int32_t> walls[NB_WALLS];
    std::string directions[PLAYERS];
    bool alive[PLAYERS];
    

    for(uint32_t u = 0; u < GENS; ++u)
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

        //GAME
        bool cont = true;
        uint32_t game_steps = 0;
        while(cont && game_steps < 10000)
        {
            std::cout << "Game " << u << std::endl;
            std::cout << std::endl;

            //DISPLAY
            

            if(system("clear") != 0)
            {
                std::cout << "Clear Error" << std::endl;
                return -1;
            }

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

           if(arg > 0)
           {     

                for(int32_t h = 0; h < HEIGHT; ++h)
                {
                    for(int32_t w = 0; w < WIDTH; ++w)
                    {
                        std::string color = "\033[1;37m";

                        if(arg > 1)
                        {

                            bool check = true;

                            for(uint32_t w2 = 0; w2 < NB_WALLS; ++w2)
                            {
                                if(walls[w2].first == w && walls[w2].second == h)
                                {
                                    color = "\033[1;30m";
                                    check = false;
                                    break;
                                }
                            }

                            if(check)
                            {
                                for(uint32_t a = 0; a < NB_APPLES; ++a)
                                {
                                    if(apples[a].first == w && apples[a].second == h)
                                    {
                                        color = "\033[1;41m";
                                        check = false;
                                        break;
                                    }
                                }
                            }

                            if(check)
                            {
                                for(uint32_t p = 0; p < PLAYERS; ++p)
                                {
                                    if(!check || !alive[p])
                                        continue;
                                    for(uint32_t b = 0; b < snakes[p].size(); ++b)
                                    {
                                        if(snakes[p][b].first == w && snakes[p][b].second == h)
                                        {
                                            color = "\033[1;" + std::to_string(31 + p) + "m";
                                            check = false;
                                            break;
                                        }
                                    }
                                }
                            }

                        }

                        std::cout << color << display[w + WIDTH * h] << "\033[0m";
                    }
                    std::cout << std::endl;
                }

                std::cout << std::endl;
                for(uint32_t p = 0; p < PLAYERS; ++p)
                {
                    std::cout << "Player \033[1;" << std::to_string(31 + p) << "m" << char('A' + p) << "\033[0m Size : " << snakes[p].size() << " Status : ";
                    if(alive[p])
                        std::cout << "Alive" << std::endl;
                    else
                        std::cout << "Dead" << std::endl;
                }

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

                directions[p] = new_AI.action(env);
            }

            //CALCULATION

            for(uint32_t p = 0; p < PLAYERS; ++p)
            {   

                if(!alive[p])
                    continue;

                for(uint32_t b = snakes[p].size() - 1; b > 0; --b)
                    snakes[p][b] = snakes[p][b - 1];          

                if(directions[p] == "UP")
                    snakes[p][0].second--;
                else if(directions[p] == "DOWN")
                    snakes[p][0].second++;
                else if(directions[p] == "RIGHT")
                    snakes[p][0].first++;
                else if(directions[p] == "LEFT")
                    snakes[p][0].first--;
                else
                    return -1;

                for(uint32_t a = 0; a < NB_APPLES; ++a)
                {
                    if(snakes[p][0] == apples[a])
                    {
                        snakes[p].push_back(snakes[p][snakes[p].size() - 1]);
                        for(uint32_t t = 0; t < 50; ++t)
                        {
                            if(t == 49)
                            {
                                apples[a].first = -2;
                                apples[a].second = -2;
                            }

                            std::pair<int32_t,int32_t> buffer(rand() % WIDTH, rand() % HEIGHT);

                            bool out = false;
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
                        continue;
                    }

                    if(snakes[p][0].second < 0 || snakes[p][0].second >= HEIGHT)
                    {
                        to_be_alive[p] = false;
                        continue;
                    }
                    
                    for(uint32_t w = 0; w < NB_WALLS; ++w)
                    {
                        if(walls[w] == snakes[p][0])
                        {
                            to_be_alive[p] = false;
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

            uint32_t alive_players = 0;
            for(uint32_t p = 0; p < PLAYERS; ++p)
                alive_players += alive[p];

            cont = alive_players > 1;
            
            game_steps++;

            std::string command = "sleep 0.075";
            if(system(command.c_str()) != 0)
            {
                std::cout << "Pause error" << std::endl;
                return -1;
            }


        }

    }


    //SAVE
    //!TODO

    return 0;
}
