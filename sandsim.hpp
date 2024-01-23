#ifndef SANDSIM
#define SANDSIM

#include <SDL.h>
#include <iostream>
#include <math.h>
#include "particle.hpp"

struct chunk
{
	bool active = false;
	bool active_next = false;
	int x = 0;
	int y = 0;
};

class SandSim
{
public:
	SandSim();
	SandSim(int x_size, int y_size);

	void clear();
	bool is_tile_valid(int x, int y);

	particle get_tile(int x, int y, bool current);
	void set_tile(int x, int y, particle input, bool current);
	void heat_tile(int x, int y, float change);

	void tick();
	void simulate_tile(int x, int y);
	void disperse_heat(int x, int y);

	bool can_move_through(particle*, particle);
	void swap_tiles(int x1, int y1, int x2, int y2, bool current);
	bool attempt_move(int x1, int y1, int x2, int y2, bool override = false);
	bool move_toward(int x1, int y1, int x2, int y2);

	void make_active(int tile_x, int tile_y);

	void draw(SDL_Renderer* r);

	int tile_size = 5;
	int x_size, y_size;

	int time = 0;

	chunk** chunks = 0;
	const int chunk_size = 10;

	particle** tiles = 0;
	particle** new_tiles = 0;

	float room_temperature = 90.0f;//23.0f;
	bool show_temp = false;

	const int fluid_spread = 5;
};

#endif