#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"

struct PhysVehicle3D;
struct PhysBody3D;

#define MAX_ACCELERATION 3000.0f
#define TURN_DEGREES 15.0f * DEGTORAD
#define BRAKE_POWER 3000.0f

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();
	void CreatePlayer(float x, float y, float z);

public:

	PhysVehicle3D* vehicle;
	float turn;
	float acceleration;
	float brake;
	bool stopped;

private:
	bool camera_debug = false;
};