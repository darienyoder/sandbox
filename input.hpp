#include <SDL.h>
#include <list>

class InputManager
{
public:
	InputManager();

	void set_monitering(SDL_KeyCode);

	void update();

	bool is_pressed(SDL_KeyCode);
	bool is_just_pressed(SDL_KeyCode);
	bool is_just_released(SDL_KeyCode);

	int mouse_x = 0, mouse_y = 0;
	bool mouse_down = false, right_mouse_down = false;
	bool just_clicked = false, just_right_clicked = false;

	bool clicked_x = false;

private:
	std::list<SDL_KeyCode> keys_monitering;

	std::list<SDL_KeyCode> keys_pressed;
	std::list<SDL_KeyCode> keys_just_pressed;
	std::list<SDL_KeyCode> keys_just_released;
};