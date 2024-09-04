#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

int main(int argc, char *argv[])
{
    
    std::srand(std::time(0));

    if(argc > 1)
    {
        if(strcmp(argv[1],"-play_a_random"))
        {
            std::cout << "Playing against snakes with random actions" << std::endl;
            /*            
            if(play_a_random(std::to_string(argv[2])) != 0)
            {
                std::cout << Program Stop << std::endl;
                return 1;
            }
            */
        }
        else if(strcmp(argv[1],"-play_a_perfect"))
        {
            std::cout << "Playing against snakes with perfect actions (not PVP)" << std::endl;
            /*            
            if(play_a_perfect(std::to_string(argv[2])) != 0)
            {
                std::cout << Program Stop << std::endl;
                return 1;
            }
            */
        }
        else if(strcmp(argv[1],"-play_a_themselves"))
        {
            std::cout << "Playing against snakes with same actions" << std::endl;
            /*            
            if(play_a_random(std::to_string(argv[2])) != 0)
            {
                std::cout << Program Stop << std::endl;
                return 1;
            }
            */
        }
        else
        {
            std::cout << "./SNAIKE to train a nex AI" << std::endl;
            std::cout << "OR" << std::endl;
            std::cout << "./SNAIKE [mode] [ai_file]" << std::endl;
            std::cout << "mode : " << std::endl;
            std::cout << "\t-play_a_random : Play the selected AI against snakes with random actions" << std::endl;
            std::cout << "\t-play_a_parfect : Play the selected AI against snakes with perfect actions (not PVP)" << std::endl;
            std::cout << "\t-play_a_themselves : Play the selected AI against snakes with same actions" << std::endl;
            std::cout << "ai_file:" << std::endl;
            std::cout << "\tFile containing the AI parameters" << std::endl;
        }
    }
    else
    {
        std::cout << "New Training" << std::endl;
        //train()
    }

    return 0;
}
