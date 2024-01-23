#ifndef PARTICLE
#define PARTICLE

#include <iostream>

enum {
	EMPTY   = -1,
	WATER,  // 0
	SAND,   // 1
	STONE,  // 2
	WOOD,   // 3
	AIR,    // 5
};

enum {
	POWDER,
	SOLID,
	LIQUID,
	GAS,
};

const int melting_point[6] =
{
	0,    // WATER
	1000, // SAND
	1000, // STONE
	200, // WOOD
	1000,
	-300,
};

const int boiling_point[6] =
{
	100,  // WATER
	2500, // SAND
	2500, // STONE
	200,  // WOOD
	200,
	-100,
};

const int ignition_point[6] =
{

};

class particle
{
public:
	particle();
	particle(int);

    void swap(particle&);
    particle& operator=(particle);

	void move() {};
	void recolor();

	int element = EMPTY;
	int state = GAS;

	float x_velocity = 0;
	float y_velocity = 0;
	float density = 1.0f;
	float temperature = 23.0f;

	int color_r = 0, color_g = 0, color_b = 0;
};

const particle empty;

#endif