#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"
#include "p2Point.h"

#define MAX_SNAKE 2

struct PhysBody3D;
struct PhysMotor3D;

enum class ObstacleType	//Unmovable, avoid
{
	NONE,

	//Unmovable
	RAMP,
	HOLE_RAMP,
	WALL,
	GROUND_BUMPS,
	GROUND_BARRIERS,
	LAMP_POSTS,

	//Movable
	BREAKABLE_WALL,
	MAILBOX,
	BOXES,
	BARRIER,
	SMALL_BARRIERS,
	SIGN,
	WRECKING_BALL,

	MAX_TYPES
};

struct Obstacle
{
	PhysBody3D* body;
	Primitive* shape;
	bool dynamic = false;

	~Obstacle() {	//Bodies are deleted on the full body list
		delete shape;
	}
};

struct BuildingData 
{
	float baseSize = 50.0f;
	float height;
	float maxHeight = 150.0f;
	float minHeight = 20.0f;
	int randHeight = (int)(maxHeight - minHeight) + 1;
};

struct CityData
{
	BuildingData building;
	float cityStart = 300.0f;
	float roadSize = 30.0f;
	float buildingDistance = building.baseSize + roadSize;
	float roadStart = cityStart + buildingDistance / 2;
};

struct TaxiStop {
	Cube* pole = nullptr;
	Cube* sign = nullptr;

	TaxiStop(float x, float y, float z) {
		pole = new Cube(x, y, z);
		sign = new Cube(4.0f, 2.0f, 1.0f);
	}

	~TaxiStop() {
		delete pole;
		delete sign;
	}
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
	int CreateCityBuildings();
	int CreateCityGoals();
	int CreateCityObstacles();

	Cube* GenerateBuilding(float x, float z);
	TaxiStop* GenerateGoal(float x, float z);
	ObstacleType GenerateObstacle(float x, float z, bool xRoad);

	void GenerateRamp(float x, float z, bool xRoad);
	void GenerateHoleRamp(float x, float z, bool xRoad);
	void GenerateWall(float x, float z, bool xRoad);
	void GenerateGroundBumps(float x, float z, bool xRoad);
	void GenerateGroundBarriers(float x, float z, bool xRoad);
	void GenerateLampPosts(float x, float z, bool xRoad);

	void GenerateBreakableWall(float x, float z, bool xRoad);
	void GenereateMailbox(float x, float z, bool xRoad);
	void GenerateBoxes(float x, float z, bool xRoad);
	void GenerateBarriers(float x, float z, bool xRoad);
	void GenerateSmallBarriers(float x, float z, bool xRoad);
	void GenerateSign(float x, float z, bool xRoad);
	void GenerateWreckingBall(float x, float z, bool xRoad);

	void ChooseGameplayGoals();

	void StartNewGame();

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

	float distance_x;
	float distance_z;

	//Carles
	CityData city;
	p2List<Cube*> buildings;
	p2List<Obstacle*> obstacles;
	p2List<TaxiStop*> goals;
	int minGoals = 5;

	Timer disco;	//BPM 140 (2.33333 BPS)
	Uint32 discoDelay = 428;
	float red, green, blue;

	//Arrow
	Cylinder arrowtest;
	float flagDetectionRadius = 800.0f;

	TaxiStop* game_destinations[5];
	PhysBody3D* taxiStop_sensor;
	vec3 taxiStop_positions[5];
	uint nextStop = 0;

	int time_left;
	int max_time;
	Timer time_passed;
	Timer goalTimer;
	int goalCollisionDelay = 1000;
	bool collided = false;
	bool lost = false;
	bool won = false;
};