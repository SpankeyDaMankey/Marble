#ifndef MARBLE_STRUCTURE_H_
#define MARBLE_STRUCTURE_H_

#include "marble_element.h"
#include <random>

using std::vector;
using std::cout;
using std::endl;

class Marble_Structure
{
    private:

        vector<vector<vector<Marble_Element>>> Marble;
        int size;


    public:

        Marble_Structure(int size) : Marble(size, vector<vector<Marble_Element>>(size, vector<Marble_Element>(size, Marble_Element(color(0,0,0))))), size{size} {}

        void print_structure()
        {
            for(int x = 0; x < size; ++x)
            {
                cout << "start of x: " << x << endl;

                for(int y = 0; y < size; ++y)
                {
                    for(int z = 0; z < size; ++z)
                    {
                        cout << Marble[x][y][z].getEnergy();
                    }
                    cout << endl;
                }
            }
        }

        void Initialize_Energy()
        {
            color initial_energy = color(1,0,0);

            for(int y = 0; y < size; ++y)
            {
                for(int z = 0; z < size; ++z)
                {
                    Marble[0][y][z].setEnergy(initial_energy);
                }
            }

        }

        void Initialize_Half_Density()
        {
            for(int x = size/2; x < size; ++x)
            {
                for(int y = 0; y < size; ++y)
                {
                    for(int z = 0; z < size; ++z)
                    {
                        Marble[x][y][z].setDensity(2);
                    }
                }
            }
        }

        void Initialize_Random_Density()
        {
            std::random_device rd;
            std::mt19937 gen(rd());

            std::uniform_int_distribution<int> distribution(1, 100);

            for(int x = 0; x < size; ++x)
            {
                for(int y = 0; y < size; ++y)
                {
                    for(int z = 0; z < size; ++z)
                    {
                        int randomNumber = distribution(gen);
                        if(randomNumber <= 5)
                        {
                            Marble[x][y][z].setDensity(2);
                        }    
                    }
                }
            }
        }

        void Propagate_Energy()
        {
            for(int y = 0; y < size; ++y)
            {
                for(int z = 0; z < size; ++z)
                {
                    int initial_distance = 0;
                    Traverse(0, y, z, initial_distance);
                }
            }
        }

        void Traverse(int x, int y, int z, int distance)
        {
            std::random_device rd;
            std::mt19937 gen(rd());

            std::uniform_int_distribution<int> distribution(1, 9);

            int distance_travelled = distance;

            int initial_x = x;
            int initial_y = y;
            int initial_z = z;

            color addEnergy = Marble[initial_x][initial_y][initial_z].getEnergy();

            int next_dir = getDirection(initial_x, initial_y, initial_z);

            int next_x = x;
            int next_y = y;
            int next_z = z;

            int valid_x, valid_y, valid_z;

            while(isValid(next_x, next_y, next_z))
            {
                valid_x = next_x;
                valid_y = next_y;
                valid_z = next_z;

                int current_direction = next_dir;

                int randomNumber = distribution(gen);
                
                if(isForward(current_direction))
                {
                    switch(randomNumber)
                    {
                        case 1:
                            next_x += 1;
                            next_y -= 1;
                            next_z += 1;
                            break;
                    
                        case 2:
                            next_x += 1;
                            next_z += 1;
                            break;
                        
                        case 3:
                            next_x += 1;
                            next_y += 1;
                            next_z += 1;
                            break;
                        
                        case 4:
                            next_x += 1;
                            next_y -= 1;
                            break;
                        
                        case 5:
                            next_x += 1;
                            break;
                        
                        case 6:
                            next_x += 1;
                            next_y += 1;
                            break;
                        
                        case 7:
                            next_x += 1;
                            next_y -= 1;
                            next_z -= 1;
                            break;

                        case 8:
                            next_x += 1;
                            next_z -= 1;
                            break;
                        
                        case 9:
                            next_x += 1;
                            next_y += 1;
                            next_z -= 1;
                            break;
                    }
                }
                else
                {
                    switch(randomNumber)
                    {
                        case 1:
                            next_x -= 1;
                            next_y -= 1;
                            next_z += 1;
                            break;
                    
                        case 2:
                            next_x -= 1;
                            next_z += 1;
                            break;
                        
                        case 3:
                            next_x -= 1;
                            next_y += 1;
                            next_z += 1;
                            break;
                        
                        case 4:
                            next_x -= 1;
                            next_y -= 1;
                            break;
                        
                        case 5:
                            next_x -= 1;
                            break;
                        
                        case 6:
                            next_x -= 1;
                            next_y -= 1;
                            break;
                        
                        case 7:
                            next_x -= 1;
                            next_y -= 1;
                            next_z -= 1;
                            break;

                        case 8:
                            next_x -= 1;
                            next_z -= 1;
                            break;
                        
                        case 9:
                            next_x -= 1;
                            next_y += 1;
                            next_z -= 1;
                            break;
                    
                    }
                }

                if(isValid(next_x, next_y, next_z))
                {
                    if(Marble[next_x][next_y][next_z].getDensity() == 1)
                    {
                        next_dir *= 1;
                        addEnergy *= 0.9;
                    }
                    else
                    {
                        next_dir *= -1;
                        //addEnergy *= 0.50;
                    }

                    ++distance_travelled;
                }
                

            }
            
            Marble[initial_x][initial_y][initial_z].setEnergy(color(0,0,0));
            Marble[valid_x][valid_y][valid_z].accumulateEnergy(addEnergy);
        }

        bool isValid(int next_x, int next_y, int next_z)
        {
            if(next_x < 0 || next_x >= size)
            {
                return false;
            }

            if(next_y < 0 || next_y >= size)
            {
                return false;
            }

            if(next_z < 0 || next_z >= size)
            {
                return false;
            }

            return true;

        }

        bool isForward(int direction)
        {
            if(direction == -1)
            {
                return true;
            }
            else return false;
        }

        int getDirection(int x, int y, int z)
        {
            if(Marble[x][y][z].getDensity() == 1)
            {
                return -1;
            }
            else return 1;
        }



        const vector<vector<vector<Marble_Element>>>& getMarble() const
        {
            return Marble;
        }

};

#endif