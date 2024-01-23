#include "sandsim.hpp"

SandSim::SandSim()
{
	x_size = 0;
	y_size = 0;
	tiles = 0;
}

SandSim::SandSim(int x_input, int y_input)
{
	x_size = x_input;
	y_size = y_input;

	tiles = new particle* [x_size];
	new_tiles = new particle* [x_size];

	chunks = new chunk* [x_size / chunk_size];

	for (int x = 0; x < x_size; ++x)
	{
		tiles[x] = new particle[y_size];
		new_tiles[x] = new particle[y_size];
	}
	for (int x = 0; x < x_size / chunk_size; ++x)
	{
		chunks[x] = new chunk[y_size / chunk_size];
	}
}

void SandSim::clear()
{
	for (int x = 0; x < x_size; ++x)
		for (int y = 0; y < y_size; ++y)
			set_tile(x, y, empty, true);
}

bool SandSim::is_tile_valid(int x, int y)
{
	return x >= 0 && x < x_size && y >= 0 && y < y_size;
}

particle SandSim::get_tile(int x, int y, bool current = false)
{
	if (is_tile_valid(x, y))
	{
		if (current)
			return new_tiles[x][y];
		else
			return tiles[x][y];
	}
	else
		return 0;
}

void SandSim::set_tile(int x, int y, particle input, bool current = false)
{
	if (is_tile_valid(x, y))
	{
		new_tiles[x][y] = input;
		if (current)
			tiles[x][y] = input;

		make_active(x, y);
	}
}

void SandSim::heat_tile(int x, int y, float change)
{
	if (is_tile_valid(x, y))
	{
		tiles[x][y].temperature += change;
		new_tiles[x][y].temperature += change;
		make_active(x, y);
		//tiles[x][y].color_r += 1;
		//new_tiles[x][y].color_r += 1;
	}
}

void SandSim::tick()
{
	++time;

	/*
	for (int chunk_x = 0; chunk_x < x_size / chunk_size; ++chunk_x)
		for (int chunk_y = 0; chunk_y < y_size / chunk_size; ++chunk_y)
			if (chunks[chunk_x][chunk_y].active)
				for (int x = 0; x < chunk_size; ++x)
					for (int y = 0; y < chunk_size; ++y)
	*/

	for (int chunk_x = 0; chunk_x < x_size / chunk_size; ++chunk_x)
		for (int chunk_y = 0; chunk_y < y_size / chunk_size; ++chunk_y)
		{
			chunks[chunk_x][chunk_y].active = chunks[chunk_x][chunk_y].active_next;
			chunks[chunk_x][chunk_y].active_next = false;
		}

	for (int chunk_x = 0; chunk_x < x_size / chunk_size; ++chunk_x)
		for (int chunk_y = 0; chunk_y < y_size / chunk_size; ++chunk_y)
			if (chunks[chunk_x][chunk_y].active)
				for (int x = 0; x < chunk_size; ++x)
					for (int y = 0; y < chunk_size; ++y)
						new_tiles[chunk_x * chunk_size + x][chunk_y * chunk_size + y] = tiles[chunk_x * chunk_size + x][chunk_y * chunk_size + y];
	
	//for (int chunk_index = 0; chunk_index < active_chunks.size(); ++chunk_index)
	const int spacing = 2;
	int sim_dir = std::rand() % 2;
	for (int chunk_x = 0; chunk_x < x_size / chunk_size; ++chunk_x)
		for (int chunk_y = 0; chunk_y < y_size / chunk_size; ++chunk_y)
			if (chunks[chunk_x][chunk_y].active)
				for (int i_x = 0; i_x < spacing; ++i_x)
					for (int i_y = 0; i_y < spacing; ++i_y)
						for (int x = 0; x < chunk_size / spacing; ++x)
							for (int y = 0; y < chunk_size / spacing; ++y)
							{
								//std::cout << chunk_x * chunk_size + (chunk_size - 1 - x * spacing - i_x) * sim_dir + (x * spacing + i_x) * (1 - sim_dir) << std::endl;
								simulate_tile
								(
									chunk_x * chunk_size + (chunk_size - 1 - x * spacing - i_x) * sim_dir + (x * spacing + i_x) * (1 - sim_dir),
									chunk_y * chunk_size + (chunk_size - 1 - y * spacing - i_y) * sim_dir + (y * spacing + i_y) * (1 - sim_dir)
								);
							}
	
	for (int chunk_x = 0; chunk_x < x_size / chunk_size; ++chunk_x)
		for (int chunk_y = 0; chunk_y < y_size / chunk_size; ++chunk_y)
			if (chunks[chunk_x][chunk_y].active)
				for (int x = 0; x < chunk_size; ++x)
					for (int y = 0; y < chunk_size; ++y)
						tiles[chunk_x * chunk_size + x][chunk_y * chunk_size + y] = new_tiles[chunk_x * chunk_size + x][chunk_y * chunk_size + y];

	for (int chunk_x = 0; chunk_x < x_size / chunk_size; ++chunk_x)
		for (int chunk_y = 0; chunk_y < y_size / chunk_size; ++chunk_y)
			if (chunks[chunk_x][chunk_y].active)
				for (int x = 0; x < chunk_size; ++x)
					for (int y = 0; y < chunk_size; ++y)
						if (tiles[chunk_x * chunk_size + x][chunk_y * chunk_size + y].element != EMPTY)
							disperse_heat(chunk_x * chunk_size + x, chunk_y * chunk_size + y);

	for (int chunk_x = 0; chunk_x < x_size / chunk_size; ++chunk_x)
		for (int chunk_y = 0; chunk_y < y_size / chunk_size; ++chunk_y)
			if (chunks[chunk_x][chunk_y].active)
				for (int x = 0; x < chunk_size; ++x)
					for (int y = 0; y < chunk_size; ++y)
						tiles[chunk_x * chunk_size + x][chunk_y * chunk_size + y].temperature = new_tiles[chunk_x * chunk_size + x][chunk_y * chunk_size + y].temperature;
}

void SandSim::disperse_heat(int x, int y)
{
	float avg_temp = 0.0;

	const double insulation = 0.95;

	for (int x1 = -1; x1 < 2; ++x1)
		for (int y1 = -1; y1 < 2; ++y1)
			if (!(x1 == 0 && y1 == 0))
			{
				if (is_tile_valid(x + x1, y + y1))
				{
					if (tiles[x + x1][y + y1].element != EMPTY)
						avg_temp += tiles[x + x1][y + y1].temperature;
					else //if (tiles[x][y].state == GAS)
						avg_temp += tiles[x][y].temperature;
					//else
					//	avg_temp += room_temperature;
				}
				else
					avg_temp += tiles[x][y].temperature;
			}
	new_tiles[x][y].temperature = new_tiles[x][y].temperature * insulation + avg_temp / 8.0f * (1.0f - insulation);
	if (new_tiles[x][y].temperature != tiles[x][y].temperature)
		make_active(x, y);
}

void SandSim::simulate_tile(int x, int y)
{
	particle* tile = &tiles[x][y];
	
	if (tile->element == EMPTY)
	{
		//tile->state = 100;
		//tile->temperature = room_temperature;
		if (new_tiles[x][y].element == EMPTY)
		{
		//	new_tiles[x][y].state = 100;
		//	new_tiles[x][y].temperature = room_temperature;
		}
			return;
	}
	/*
	particle* ntile = tile;
	if (is_tile_valid(x, y + 1))
		ntile = &tiles[x][y + 1];

	std::cout << "\n\nELEMENT: " << ntile->element
		<< "\nSTATE: " << ntile->state
		<< "\nX-VEL: " << ntile->x_velocity
		<< "\nY-VEL: " << ntile->y_velocity
		<< "\nTEMP: " << ntile->temperature;

	*/

	switch (tile->state)
	{
		case SOLID:
			if (tile->temperature > melting_point[tile->element])
			{
				tile->state = LIQUID;
				new_tiles[x][y].state = LIQUID;
				tile->recolor();
				new_tiles[x][y].color_r = tile->color_r;
				new_tiles[x][y].color_g = tile->color_g;
				new_tiles[x][y].color_b = tile->color_b;
				simulate_tile(x, y);
				return;
			}
			break;

		case POWDER:

			if (tile->temperature > melting_point[tile->element])
			{
				tile->state = LIQUID;
				new_tiles[x][y].state = LIQUID;
				tile->recolor();
				new_tiles[x][y].color_r = tile->color_r;
				new_tiles[x][y].color_g = tile->color_g;
				new_tiles[x][y].color_b = tile->color_b;
				simulate_tile(x, y);
				return;
			}

			if (can_move_through(tile, get_tile(x, y + 1)))
			{
				tile->y_velocity = 1;
				tile->x_velocity = 0;
			}
			else
			{
				int side_array[2] = { -1, 1 };
				int side = side_array[std::rand() % 2];

				if (can_move_through(tile, get_tile(x + side, y + 1)))
				{
					tile->y_velocity = 1;
					tile->x_velocity = side;
				}
				else if (can_move_through(tile, get_tile(x - side, y + 1)))
				{
					tile->y_velocity = 1;
					tile->x_velocity = -side;
				}
				else
				{
					tile->x_velocity *= 0.5;
					tile->y_velocity = 0;
				}
			}
			break;
		case LIQUID:

			if (tile->temperature < melting_point[tile->element])
			{
				tile->state = POWDER;
				new_tiles[x][y].state = POWDER;
				tile->recolor();
				new_tiles[x][y].color_r = tile->color_r;
				new_tiles[x][y].color_g = tile->color_g;
				new_tiles[x][y].color_b = tile->color_b;
				simulate_tile(x, y);
				return;
			}
			else if (tile->temperature > boiling_point[tile->element])
			{
				tile->state = GAS;
				new_tiles[x][y].state = GAS;
				tile->recolor();
				new_tiles[x][y].color_r = tile->color_r;
				new_tiles[x][y].color_g = tile->color_g;
				new_tiles[x][y].color_b = tile->color_b;
				simulate_tile(x, y);
				return;
			}

			if (can_move_through(tile, get_tile(x, y + 1)))
			{
				tile->y_velocity = 1;
				tile->x_velocity = 0;
				//std::cout << "\n LIQUID DOWN";
			}
			else
			{
				int side_array[2] = { -1, 1 };
				int side = side_array[std::rand() % 2];

				if (can_move_through(tile, get_tile(x + side, y + 1)))
				{
					tile->y_velocity = 1;
					tile->x_velocity = side * 1;// (std::rand() % (fluid_spread / 2) + fluid_spread / 2);
					//std::cout << "\n LIQUID DIAGONAL";
				}
				else if (can_move_through(tile, get_tile(x - side, y + 1)))
				{
					tile->y_velocity = 1;
					tile->x_velocity = -side * 1;// (std::rand() % (fluid_spread / 2) + fluid_spread / 2);
					//std::cout << "\n LIQUID DIAGONAL";
				}
				else
				{
					tile->y_velocity = 0;
					if (true)
					{
						if (can_move_through(tile, get_tile(x + side, y)))
						{
							tile->x_velocity = side * (std::rand() % (fluid_spread / 2) + fluid_spread / 2);
							//std::cout << "\n LIQUID SIDE";
						}
						else if (can_move_through(tile, get_tile(x - side, y)))
						{
							tile->x_velocity = -side * (std::rand() % (fluid_spread / 2) + fluid_spread / 2);
							//std::cout << "\n LIQUID SIDE";
						}
						else
						{
							tile->x_velocity = 0;
							tile->y_velocity = 0;
							//std::cout << "\n LIQUID STILL";
						}
					}
				}
			}
			break;
		case GAS:

			if (tile->temperature < boiling_point[tile->element])
			{
				tile->state = LIQUID;
				new_tiles[x][y].state = LIQUID;
				tile->recolor();
				new_tiles[x][y].color_r = tile->color_r;
				new_tiles[x][y].color_g = tile->color_g;
				new_tiles[x][y].color_b = tile->color_b;
				simulate_tile(x, y);
				return;
			}

			if (can_move_through(tile, get_tile(x, y - 1)) && std::rand() % 3 == 0)
			{
				tile->y_velocity = -1;
				tile->x_velocity = 0;
				//std::cout << "\nGAS UP";
			}
			else
			{
				int side_array[2] = { -1, 1 };
				int side = side_array[std::rand() % 2];

				if (can_move_through(tile, get_tile(x + side, y - 1)) && std::rand() % 3 == 0)
				{
					tile->y_velocity = -1;
					tile->x_velocity = side * 1;// (std::rand() % (fluid_spread / 2) + fluid_spread / 2);
				}
				else if (can_move_through(tile, get_tile(x - side, y - 1)) && std::rand() % 3 == 0)
				{
					tile->y_velocity = -1;
					tile->x_velocity = -side * 1;// (std::rand() % (fluid_spread / 2) + fluid_spread / 2);
				}
				else
				{
					tile->y_velocity = 0;
					if (can_move_through(tile, get_tile(x + side, y)) && std::rand() % 3 == 0)
					{
						if (tile->x_velocity * side >= 0)
							tile->x_velocity = side * (std::rand() % (fluid_spread / 2) + fluid_spread / 2);
					}
					else if (can_move_through(tile, get_tile(x - side, y)))
					{
						if (tile->x_velocity * side >= 0)
							tile->x_velocity = -side * (std::rand() % (fluid_spread / 2) + fluid_spread / 2);
					}
					else
					{
						tile->x_velocity = 0;
					}
				}
			}
			break;
	}
	//tile->recolor();
	move_toward(x, y, x + (int)(tile->x_velocity), y + (int)(tile->y_velocity));
}

bool SandSim::move_toward(int x1, int y1, int x2, int y2)
{
	bool has_moved = false;

	int xDiff = x1 - x2;
	int yDiff = y1 - y2;
	bool xDiffIsLarger = std::abs(xDiff) > std::abs(yDiff);

	int xModifier = xDiff < 0 ? 1 : -1;
	int yModifier = yDiff < 0 ? 1 : -1;

	int longerSideLength = std::max(std::abs(xDiff), std::abs(yDiff));
	int shorterSideLength = std::min(std::abs(xDiff), std::abs(yDiff));
	float slope = (shorterSideLength == 0 || longerSideLength == 0) ? 0 : ((float)(shorterSideLength) / (longerSideLength));

	int pastX = x1;
	int pastY = y1;

	int shorterSideIncrease;
	for (int i = 1; i <= std::min((float)longerSideLength, 300000.0f); i++)
	{
		shorterSideIncrease = std::round(i * slope);
		int yIncrease, xIncrease;
		if (xDiffIsLarger)
		{
			xIncrease = i;
			yIncrease = shorterSideIncrease;
		}
		else
		{
			yIncrease = i;
			xIncrease = shorterSideIncrease;
		}
		int currentY = y1 + (yIncrease * yModifier);
		int currentX = x1 + (xIncrease * xModifier);
		if (is_tile_valid(currentX, currentY))
		{
			if (attempt_move(pastX, pastY, currentX, currentY, has_moved))
				has_moved = true;
			else
			{
				if (new_tiles[pastX][pastY].state == POWDER && new_tiles[pastX][pastY].y_velocity > 30)
					new_tiles[pastX][pastY].x_velocity = std::rand() % 21 - 10;
				else
					new_tiles[pastX][pastY].x_velocity = 0;
				new_tiles[pastX][pastY].y_velocity = 0;
				break;
			}
				pastX = currentX;
			pastY = currentY;
		}
		else
			break;
	}
	return has_moved;
}

bool SandSim::can_move_through(particle* p1, particle p2)
{
	return p2.element == EMPTY || p1->state < p2.state || (p2.state >= LIQUID && p1->element == p2.element && p1->temperature < p2.temperature);
}

bool SandSim::attempt_move(int x1, int y1, int x2, int y2, bool override)
{
	if
	(
		(override || get_tile(x1, y1).element == get_tile(x1, y1, true).element)
		&& get_tile(x1, y1).temperature == get_tile(x1, y1, true).temperature
		&& get_tile(x2, y2).element == get_tile(x2, y2, true).element
		&& get_tile(x2, y2).temperature == get_tile(x2, y2, true).temperature
	)
	{
		if (get_tile(x2, y2).state >= LIQUID)
		{
			swap_tiles(x1, y1, x2, y2, override);
			return true;
		}
	}
	return false;
}

void SandSim::swap_tiles(int x1, int y1, int x2, int y2, bool current = false)
{
	particle temp = get_tile(x1, y1, current);
	set_tile(x1, y1, get_tile(x2, y2, current));
	set_tile(x2, y2, temp);
}

void SandSim::make_active(int tile_x, int tile_y)
{
	if (tile_x < 0 || tile_x >= x_size || tile_y < 0 || tile_y >= y_size)
		return;
	chunks[tile_x / chunk_size][tile_y / chunk_size].active_next = true;

	if (tile_x % chunk_size == 0 && tile_x != 0)
		chunks[tile_x / chunk_size - 1][tile_y / chunk_size].active_next = true;
	if (tile_x % chunk_size == chunk_size - 1 && tile_x != x_size - 1)
		chunks[tile_x / chunk_size + 1][tile_y / chunk_size].active_next = true;

	if (tile_y % chunk_size == 0 && tile_y != 0)
		chunks[tile_x / chunk_size][tile_y / chunk_size - 1].active_next = true;
	if (tile_y % chunk_size == chunk_size - 1 && tile_y != y_size - 1)
		chunks[tile_x / chunk_size][tile_y / chunk_size + 1].active_next = true;
}

void SandSim::draw(SDL_Renderer* renderer)
{
	SDL_Rect background = { 10, 10, tile_size * x_size, tile_size * y_size };
	SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
	SDL_RenderFillRect(renderer, &background);

	int r = 0, g = 0, b = 0;
	float value = 1.0f;

	for (int x = 0; x < x_size; ++x)
	{
		for (int y = 0; y < y_size; ++y)
		{
			//if (tiles[x][y].element == EMPTY)
			//	continue;

			//value = 1 + std::sin(x) + x / (1.1 + std::sin(y)) + (y * 0.7234f);
			//value = (value - (int)value) * 0.25 + 0.75;
			r = tiles[x][y].color_r;
			g = tiles[x][y].color_g;
			b = tiles[x][y].color_b;
			value = 1.0f;

			/*

			switch (tiles[x][y].element)
			{
				case EMPTY:
					r = 255;
					g = 255;
					b = 255;
					break;
				case SAND:
					r = 255;
					g = 230;
					b = 128;
					break;
				case WATER:
					switch (tiles[x][y].state)
					{
					case LIQUID:
						r = 0;
						g = 50;
						b = 255;
						break;
					case POWDER:
						r = 50;
						g = 150;
						b = 255;
						break;
					case GAS:
						r = 200;
						g = 200;
						b = 200;
						break;
					}
					value = 0.5f + value / 2.0;
					break;
				case STONE:
					r = 50;
					g = 50;
					b = 50;
					break;
				case WOOD:
					r = 128;
					g = 64;
					b = 0;
					value = value * ((x % 2) / 2.0 + 0.5);
					break;
			}

			*/

			if (show_temp) // TEMPERATURE
			{
				r = std::max(std::min(tiles[x][y].temperature, 100.0f), 0.0f) / 100.0f * 255.0f;
				g *= 1.0 - std::max(std::min(tiles[x][y].temperature, 100.0f), 0.0f) / 100.0f;
				b *= 1.0 - std::max(std::min(tiles[x][y].temperature, 100.0f), 0.0f) / 100.0f;
			}

			SDL_Rect box = { x * tile_size + 10, y * tile_size + 10, tile_size, tile_size };
			SDL_SetRenderDrawColor(renderer, r * value, g * value, b * value, 255);
			SDL_RenderFillRect(renderer, &box);
		}
	}
}
