#include "particle.hpp"

particle::particle()
{

}

particle::particle(int t)
{
	element = t;
	switch (element)
	{
		case SAND:
			state = POWDER;
			density = 2.0;
			break;
		case WATER:
			state = LIQUID;
			density = 1.0;
			break;
		case STONE:
			state = SOLID;
			density = 5.0;
			break;
		case WOOD:
			state = SOLID;
			density = 3.0;
			break;
		case AIR:
			state = GAS;
			density = 0.5;
			break;
	}

	recolor();
}

void particle::swap(particle& rhs)
{
	int temp = element;
	element = rhs.element;
	rhs.element = temp;

	temp = state;
	state = rhs.state;
	rhs.state = temp;

	temp = x_velocity;
	x_velocity = rhs.x_velocity;
	rhs.x_velocity = temp;

	temp = y_velocity;
	y_velocity = rhs.y_velocity;
	rhs.y_velocity = temp;

	float tempf = density;
	density = rhs.density;
	rhs.density = tempf;

	tempf = temperature;
	temperature = rhs.temperature;
	rhs.temperature = tempf;

	temp = color_r;
	color_r = rhs.color_r;
	rhs.color_r = temp;

	temp = color_g;
	color_g = rhs.color_g;
	rhs.color_g = temp;

	temp = color_b;
	color_b = rhs.color_b;
	rhs.color_b = temp;
}

particle& particle::operator=(particle rhs)
{
	element = rhs.element;

	state = rhs.state;

	x_velocity = rhs.x_velocity;

	y_velocity = rhs.y_velocity;

	density = rhs.density;

	temperature = rhs.temperature;

	color_r = rhs.color_r;
	color_g = rhs.color_g;
	color_b = rhs.color_b;

	return *this;
}

void particle::recolor()
{
	float value = (float)(std::rand() % 100) / 100.0;
	value = (value - (int)value) * 0.25 + 0.75;
	/*
	color_r = std::rand() % 256;
	color_g = std::rand() % 256;
	color_b = std::rand() % 256;

	while (std::max(color_g, std::max(color_r, color_b)) < 255)
	{
		color_r += 1;
		color_g += 1;
		color_b += 1;
	}

	return;*/

	switch (element)
	{
	case EMPTY:
		switch (state)
		{
		case GAS:
			color_r = 0;
			color_g = 100;
			color_b = 0;
		case LIQUID:
			color_r = 100;
			color_g = 100;
			color_b = 0;
		case POWDER:
			color_r = 100;
			color_g = 0;
			color_b = 0;
		case 100:
			color_r = 0;
			color_g = 0;
			color_b = 0;
		}
		
		break;
	case SAND:
		switch (state)
		{
		case POWDER:
			color_r = 255;
			color_g = 230;
			color_b = 128;
			break;
		case LIQUID:
			color_r = 252;
			color_g = 157;
			color_b = 47;
			break;
		case GAS:
			color_r = 80;
			color_g = 80;
			color_b = 80;
			break;
		}
		break;
	case WATER:
		switch (state)
		{
		case LIQUID:
			color_r = 0;
			color_g = 50;
			color_b = 255;
			break;
		case POWDER:
			color_r = 50;
			color_g = 150;
			color_b = 255;
			break;
		case GAS:
			color_r = 200;
			color_g = 200;
			color_b = 200;
			break;
		}
		value = 0.5f + value / 2.0;
		break;
	case STONE:
		switch (state)
		{
		case LIQUID:
			color_r = 252;
			color_g = 157;
			color_b = 47;
			break;
		case SOLID: case POWDER:
			color_r = 50;
			color_g = 50;
			color_b = 50;
			break;
		case GAS:
			color_r = 80;
			color_g = 80;
			color_b = 80;
			break;
		}
		break;
	case WOOD:
		value = value * ((1 % 2) / 2.0 + 0.5);
		switch (state)
		{
		case SOLID:
			color_r = 128;
			color_g = 64;
			color_b = 0;
			break;
		case GAS:
			color_r = 80;
			color_g = 80;
			color_b = 80;
			break;
		}
	}

	color_r *= value;
	color_g *= value;
	color_b *= value;
}