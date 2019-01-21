#pragma once
#include "Module.h"
#include "Globals.h"

#include "LogitechSteeringWheel/Include/LogitechSteeringWheelLib.h"

#define MAX_MOUSE_BUTTONS 5
#define MAX_STWHEELS 1

enum KEY_STATE
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class ModuleInput : public Module
{
public:
	
	ModuleInput(Application* app, bool start_enabled = true);
	~ModuleInput();

	bool Init();
	update_status PreUpdate(float dt);
	bool CleanUp();

	KEY_STATE GetKey(int id) const
	{
		return keyboard[id];
	}

	KEY_STATE GetMouseButton(int id) const
	{
		return mouse_buttons[id];
	}

	int GetMouseX() const
	{
		return mouse_x;
	}

	int GetMouseY() const
	{
		return mouse_y;
	}

	int GetMouseZ() const
	{
		return mouse_z;
	}

	int GetMouseXMotion() const
	{
		return mouse_x_motion;
	}

	int GetMouseYMotion() const
	{
		return mouse_y_motion;
	}

public: //LOGITECH STEERING WHEEL

	int GetWheeliX()
	{
		return iX;
	}

	int GetWheeliY()
	{
		return iY;
	}

	int GetWheeliZ()
	{
		return iZ;
	}
private:
	KEY_STATE* keyboard;
	KEY_STATE mouse_buttons[MAX_MOUSE_BUTTONS];
	int mouse_x;
	int mouse_y;
	int mouse_z;
	int mouse_x_motion;
	int mouse_y_motion;
	//int mouse_z_motion;

	//Logitech Steering Wheel 
	DIJOYSTATE2ENGINES* stWheel;
	int iX, iY, iZ;
};