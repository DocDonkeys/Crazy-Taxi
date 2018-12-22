#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"
#include "p2Point.h"

#define MAX_SNAKE 2

struct PhysBody3D;
struct PhysMotor3D;

enum class ObstacleType
{
	NONE = -1,

	MAX_TYPES
};

struct CityData {
	float baseSize = 50.0f;
	float height;
	float maxHeight = 150.0f;
	float minHeight = 20.0f;
	float roadSize = 30.0f;
	int randHeight = (int)(maxHeight - minHeight) + 1;
};

//CHANGE/FIX DIDAC

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

private:
	Cube* GenerateBuilding(float x, float z);
	ObstacleType CreateCrossing(float x, float z);

public:
	/*
	PhysBody3D* pb_snake[MAX_SNAKE];
	Sphere s_snake[MAX_SNAKE];

	PhysBody3D* pb_snake2[MAX_SNAKE];
	Sphere s_snake2[MAX_SNAKE];
	*/

	PhysBody3D* pb_chassis;
	Cube p_chassis;

	PhysBody3D* pb_wheel;
	Cylinder p_wheel;

	PhysBody3D* pb_wheel2;
	Cylinder p_wheel2;

	PhysMotor3D* left_wheel;
	PhysMotor3D* right_wheel;


	//Vars to test
	int x, y, z = 0;
	float angle = 0;

	int distance_x;
	int distance_z;
	

	//Testing Sensors
	PhysBody3D* test;
	Cube* c1;

	//Carles
	CityData buildData;
	p2List<Cube*> buildings;
	Cube* c2;

	Timer disco;	//BPM 140 (2.33333 BPS)
	Uint32 discoDelay = 428;
	float red, green, blue;

private:
	//Cube* GenerateBuilding();
};