#include "input.hpp"

InputManager::InputManager()
{
	return;
}

void InputManager::set_monitering(SDL_KeyCode input)
{
	keys_monitering.push_back(input);
}

void InputManager::update()
{
	keys_just_pressed.clear();
	keys_just_released.clear();
	just_clicked = false;

	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type)
	{
	case SDL_QUIT:
		clicked_x = true;
	case SDL_KEYDOWN:
		for (std::list<SDL_KeyCode>::const_iterator i = keys_monitering.begin(); i != keys_monitering.end(); ++i)
		{
			if (event.key.keysym.sym == *i)
			{
				bool has = false;
				for (std::list<SDL_KeyCode>::const_iterator pi = keys_pressed.begin(); pi != keys_pressed.end(); ++pi)
				{
					if (*pi == *i)
					{
						has = true;
						break;
					}
				}
				if (!has)
				{
					keys_pressed.push_back(*i);
				}
				break;
			}
		}
		break;
	case SDL_KEYUP:
		for (std::list<SDL_KeyCode>::const_iterator i = keys_pressed.begin(); i != keys_pressed.end(); ++i)
		{
			if (event.key.keysym.sym == *i)
			{
				keys_pressed.erase(i);
				break;
			}
		}
		break;
	case SDL_MOUSEMOTION:
		mouse_x = event.motion.x;
		mouse_y = event.motion.y;
		break;
	case SDL_MOUSEBUTTONDOWN:
		if (event.button.button == SDL_BUTTON_LEFT)
		{
			if (mouse_down)
				just_clicked = true;
			mouse_down = true;
		}
		else if (event.button.button == SDL_BUTTON_RIGHT)
		{
			if (right_mouse_down)
				just_right_clicked = true;
			right_mouse_down = true;
		}
		break;
	case SDL_MOUSEBUTTONUP:
		if (event.button.button == SDL_BUTTON_LEFT)
			mouse_down = false;
		else if (event.button.button == SDL_BUTTON_RIGHT)
			right_mouse_down = false;
		break;
	}
}

bool InputManager::is_pressed(SDL_KeyCode input)
{
	for (std::list<SDL_KeyCode>::const_iterator i = keys_pressed.begin(); i != keys_pressed.end(); ++i)
	{
		if (*i == input)
		{
			return true;
		}
	}
	return false;
}

bool InputManager::is_just_pressed(SDL_KeyCode input)
{
	for (std::list<SDL_KeyCode>::const_iterator i = keys_just_pressed.begin(); i != keys_just_pressed.end(); ++i)
	{
		if (*i == input)
		{
			return true;
		}
	}
	return false;
}

bool InputManager::is_just_released(SDL_KeyCode input)
{
	for (std::list<SDL_KeyCode>::const_iterator i = keys_just_released.begin(); i != keys_just_released.end(); ++i)
	{
		if (*i == input)
		{
			return true;
		}
	}
	return false;
}