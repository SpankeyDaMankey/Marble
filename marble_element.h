#ifndef MARBLE_ELEMENT_H_
#define MARBLE_ELEMENT_H_

#include "rtweekend.h"

#include <iostream>
#include <vector>

class Marble_Element
{
private:
	
	color energy;
	int density = 1;
	
public:

	Marble_Element(color energy) : energy{energy} {};
	
	color getEnergy() const {
		return energy;
	}
	
	void setEnergy(color newEnergy) {
		energy = newEnergy;
	}

	void setDensity(int new_density)
	{
		density = new_density;
	}

	int getDensity()
	{
		return density;
	}

    void accumulateEnergy(color addEnergy) {
        energy += addEnergy;
    }

};

#endif