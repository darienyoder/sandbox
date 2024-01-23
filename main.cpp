#include <iostream>
#include <SDL.h>
#include <math.h>
#include "sandsim.hpp"
#include "input.hpp"

SDL_Window* window;
SDL_Renderer* renderer;

bool game_is_running = false;
float time_since_last_frame = 0.0;
int target_fps = 60;

SandSim sim(200, 120);
InputManager input;

int mouse_action = 1;

bool run_sim = true;

const int window_margin = 10;
const int button_size = 50;
const int button_count = 10;

void initialize_window()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow(
		"Sandbox",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		sim.x_size * sim.tile_size + window_margin * 3 + button_size,
		sim.y_size * sim.tile_size + window_margin * 2,
		0//SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED
	);
	renderer = SDL_CreateRenderer(window, -1, 0);

	game_is_running = true;
}

void cleanup()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void setup()
{
	for (int x = 0; x < sim.x_size; ++x)
	{
		for (int y = 270; y < sim.y_size; ++y)
			sim.set_tile(x, y, WATER, true);
		const float hill_offset = 15.5;//26;
		const float hill_width = 0.007f;
		//int base_height = 270 + std::sin(x * hill_width + hill_offset) * 5 + std::cos(x * hill_width * 2.0 + hill_offset * 133.4023);
		int base_height = 300 + 10 * std::sin(x * hill_width + hill_offset) * 5 + std::cos(x * hill_width * 2.0 + hill_offset * 133.4023);
		for (int y = base_height; y < sim.y_size; ++y)
			sim.set_tile(x, y, SAND, true);
	}
	input.set_monitering(SDLK_ESCAPE);
}

void place_tile(int new_tile, int state)
{
	const int tile_size = 5;
	for (int x = 0; x < tile_size; ++x)
		for (int y = 0; y < tile_size; ++y)
		{
			int tile_x = input.mouse_x / sim.tile_size + x - tile_size / 2;
			int tile_y = input.mouse_y / sim.tile_size + y - tile_size / 2;
			sim.set_tile(tile_x, tile_y, new_tile, true);
			sim.make_active(tile_x, tile_y);
			//sim.chunks[(input.mouse_x / sim.tile_size + x - tile_size / 2) / sim.chunk_size][(input.mouse_y / sim.tile_size + y - tile_size / 2) / sim.chunk_size].active_next = true;
			switch (state)
			{
				case POWDER:
					if (melting_point[new_tile] < sim.room_temperature)
						sim.heat_tile(tile_x, tile_y, melting_point[new_tile] - 50 - sim.get_tile(tile_x, tile_y, false).temperature);
					break;
				case LIQUID:
					if (melting_point[new_tile] > sim.room_temperature || boiling_point[new_tile] < sim.room_temperature)
						sim.heat_tile(tile_x, tile_y, (melting_point[new_tile] + boiling_point[new_tile]) / 2 - sim.get_tile(tile_x, tile_y, false).temperature);
					break;
				case GAS:
					if (boiling_point[new_tile] > sim.room_temperature)
						sim.heat_tile(tile_x, tile_y, boiling_point[new_tile] + 50 - sim.get_tile(tile_x, tile_y, false).temperature);
					break;
			}
				//sim.heat_tile(input.mouse_x / sim.tile_size + x - tile_size / 2, input.mouse_y / sim.tile_size + y - tile_size / 2, melting_point[new_tile] + 20);
		}
}

void get_input()
{
	input.update();

	if (input.mouse_down)
	{
		if (input.mouse_x > window_margin && input.mouse_x < window_margin + sim.x_size * sim.tile_size && input.mouse_y > window_margin && input.mouse_y < window_margin + sim.y_size * sim.tile_size)
			switch (mouse_action)
			{
				case 1:
					place_tile(SAND, POWDER);
					break;
				case 2:
					place_tile(WATER, LIQUID);
					break;
				case 3:
					place_tile(WATER, POWDER);
					break;
				case 4:
					place_tile(WATER, GAS);
					break;
				case 5:
					place_tile(WOOD, SOLID);
					break;
				case 6:
					place_tile(STONE, SOLID);
					break;
				case 7:
					place_tile(STONE, LIQUID);
					break;
				case 8:
					for (int x = -5; x < 6; ++x)
						for (int y = -5; y < 6; ++y)
							sim.heat_tile(input.mouse_x / sim.tile_size + x, input.mouse_y / sim.tile_size + y, 5);
					break;
				case 9:
					for (int x = -5; x < 6; ++x)
						for (int y = -5; y < 6; ++y)
							sim.heat_tile(input.mouse_x / sim.tile_size + x, input.mouse_y / sim.tile_size + y, -5);
					break;
			}
		else if (input.mouse_x > window_margin * 2 + sim.x_size * sim.tile_size && input.mouse_x < window_margin * 2 + sim.x_size * sim.tile_size + button_size)
		{
			for (int i = 0; i < button_count; ++i)
				if (input.mouse_y > window_margin + (window_margin + button_size) * i && input.mouse_y < (window_margin + button_size) * (i + 1))
				{
					if (i == 0)
						sim.clear();
					else
						mouse_action = i;
					break;
				}
		}

		/*
		for (int x = -5; x < 6; ++x)
			for (int y = -5; y < 6; ++y)
				sim.heat_tile(input.mouse_x / sim.tile_size + x, input.mouse_y / sim.tile_size + y, 6);//place_tile(WOOD);
		*/
	}
	else if (input.right_mouse_down)
	{
		place_tile(EMPTY, GAS);
	}

	if (input.is_pressed(SDLK_ESCAPE))
		game_is_running = false;
	/*
	if (input.is_pressed(SDLK_1))
		place_tile(SAND);

	if (input.is_pressed(SDLK_2))
		place_tile(WATER);

	if (input.is_pressed(SDLK_3))
		place_tile(STONE);

	if (input.is_pressed(SDLK_x))
		place_tile(EMPTY);*/

	if (input.is_pressed(SDLK_0))
	{
		if (sim.room_temperature == 23.0f)
			sim.room_temperature = 200.0f;
		else
			sim.room_temperature = 23.0f;
	}

	if (input.is_pressed(SDLK_9))
		sim.show_temp = !sim.show_temp;
}

void update()
{
	int time_to_wait = (1000.0f / target_fps) - (SDL_GetTicks() - time_since_last_frame);

	if (time_to_wait > 0 && time_to_wait <= (1000.0f / target_fps))
		SDL_Delay(time_to_wait);


	float delta = (SDL_GetTicks() - time_since_last_frame) / 1000.0f;
	time_since_last_frame = SDL_GetTicks();

	if (run_sim)
	{
		sim.tick();
	}
}

void draw_buttons()
{
	const int r[button_count] = { 255, 255,   0,  50, 200, 128, 50, 252, 255,  50 };
	const int g[button_count] = { 0,   255,   0, 150, 200,  64, 50, 157,   0, 150 };
	const int b[button_count] = { 0,     0, 255, 255, 200,   0, 50,  47,   0, 255 };

	for (int i = 0; i < button_count; ++i)
	{
		SDL_Rect button_rect;
		button_rect.x = sim.x_size * sim.tile_size + window_margin * 2;
		button_rect.y = window_margin + (button_size + window_margin) * i;
		button_rect.w = button_size;
		button_rect.h = button_size;

		if (mouse_action == i)
			SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
		else
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderFillRect(renderer, &button_rect);

		button_rect.x += 5;
		button_rect.y += 5;
		button_rect.w -= 10;
		button_rect.h -= 10;

		SDL_SetRenderDrawColor(renderer, r[i], g[i], b[i], 255);
		SDL_RenderFillRect(renderer, &button_rect);

		if (i == 0)
		{
			const int scale = 3;
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderSetScale(renderer, scale, scale);
			SDL_RenderDrawLine(renderer, (button_rect.x + 5) / scale, (button_rect.y + 5) / scale, (button_rect.x + button_rect.w - 5) / scale, (button_rect.y + button_rect.h - 5) / scale);
			SDL_RenderDrawLine(renderer, (button_rect.x + 5) / scale, (button_rect.y + button_rect.h - 5) / scale, (button_rect.x + button_rect.w - 5) / scale, (button_rect.y + 5) / scale);
			SDL_RenderSetScale(renderer, 1, 1);
		}
		else if (i == 8)
		{
			const int scale = 3;
			SDL_SetRenderDrawColor(renderer, 252, 157, 47, 255);
			SDL_RenderSetScale(renderer, scale, scale);
			SDL_RenderDrawLine(renderer, (button_rect.x + button_rect.w / 2) / scale, (button_rect.y + 5) / scale, (button_rect.x + button_rect.w / 2) / scale, (button_rect.y + button_rect.h - 5) / scale);
			SDL_RenderDrawLine(renderer, (button_rect.x + button_rect.w / 2) / scale, (button_rect.y + 5) / scale, (button_rect.x + 5) / scale, (button_rect.y + button_rect.h / 2) / scale);
			SDL_RenderDrawLine(renderer, (button_rect.x + button_rect.w / 2) / scale, (button_rect.y + 5) / scale, (button_rect.x + button_rect.w - 5) / scale, (button_rect.y + button_rect.h / 2) / scale);
			SDL_RenderSetScale(renderer, 1, 1);
		}
		else if (i == 9)
		{
			const int scale = 3;
			SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
			SDL_RenderSetScale(renderer, scale, scale);
			SDL_RenderDrawLine(renderer, (button_rect.x + button_rect.w / 2) / scale, (button_rect.y + 5) / scale, (button_rect.x + button_rect.w / 2) / scale, (button_rect.y + button_rect.h - 5) / scale);
			SDL_RenderDrawLine(renderer, (button_rect.x + button_rect.w / 2) / scale, (button_rect.y + button_rect.h - 5) / scale, (button_rect.x + 5) / scale, (button_rect.y + button_rect.h / 2) / scale);
			SDL_RenderDrawLine(renderer, (button_rect.x + button_rect.w / 2) / scale, (button_rect.y + button_rect.h - 5) / scale, (button_rect.x + button_rect.w - 5) / scale, (button_rect.y + button_rect.h / 2) / scale);
			SDL_RenderSetScale(renderer, 1, 1);
		}
	}
}

void draw()
{
	SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
	SDL_RenderClear(renderer);

	int size[2];
	SDL_GetWindowSize(window, &size[0], &size[1]);
	if ((float)size[0] / (float)size[1] < (float)sim.x_size / (float)sim.y_size)
		sim.tile_size = size[0] / sim.x_size;
	else
		sim.tile_size = size[1] / sim.y_size;

	sim.draw(renderer);

	draw_buttons();

	SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[])
{
	initialize_window();

	setup();

	while (game_is_running && !input.clicked_x)
	{
		get_input();
		update();
		draw();
	}

	cleanup();

	return 0;
}