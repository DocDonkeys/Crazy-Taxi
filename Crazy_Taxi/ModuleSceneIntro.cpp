#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"
#include <math.h>
#include <ctime>

//CHANGE/FIX this is included because of arrow testing
#include "ModulePlayer.h"
#include "PhysVehicle3D.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	max_time = 120;
	time_left = max_time;

	nextStop = 0;

	lost = false;
	won = false;

	//Rand seed based on current time
	srand((uint)time(NULL));

	int counter;

	//City generator
	CreateCityBuildings();
	CreateCityObstacles();
	counter = CreateCityGoals();

	//Reach minimum goals
	while (counter < minGoals) {
		for (p2List_item<TaxiStop*>* item = goals.getFirst(); item != nullptr; item = item->next) {
			delete item->data;
		}
		goals.clear();
		counter = CreateCityGoals();
	}

	//Add Goal Bodies when minimal goals are reached
	for (p2List_item<TaxiStop*>* item = goals.getFirst(); item != nullptr; item = item->next) {
		App->physics->AddBody(*item->data->pole, 0.0f);
		App->physics->AddBody(*item->data->sign, 0.0f);
	}

	//Choose the TaxiStops that will be used in the game
	ChooseGameplayGoals();

	//TESTING DIDAC
	for (int i = 0; i < 5; ++i)
	{
		taxiStop_positions[i] = game_destinations[i]->pole->GetPosition();
	}
	game_destinations[nextStop]->pole->color.Set(1.0f,1.0f,0.0f);
	game_destinations[nextStop]->sign->color.Set(1.0f, 1.0f, 0.0f);
	//Place Test sensor in all of the Taxi Stops
	Cube sensor(8, 4, 8);
	sensor.SetPos(taxiStop_positions[nextStop].x, taxiStop_positions[nextStop].y - 2.5, taxiStop_positions[nextStop].z);
	taxiStop_sensor = App->physics->AddBody(sensor, 0.0f);
	taxiStop_sensor->SetAsSensor(true);
	taxiStop_sensor->collision_listeners.add(App->scene_intro);

	//ARROW
	arrowtest.radius = 0.5;
	arrowtest.height = 3;

	//Music:
	App->audio->PlayMusic("audio/music/Yellow_Line.ogg");
	App->audio->SetMusicVolume(30);
	disco.Start();

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	//City CleanUp
	for (p2List_item<Cube*>* item = buildings.getFirst(); item != nullptr; item = item->next) {
		delete item->data;
	}
	buildings.clear();

	//Obstacle CleanUp
	for (p2List_item<Obstacle*>* item = obstacles.getFirst(); item != nullptr; item = item->next) {
		delete item->data;
	}
	obstacles.clear();

	//Goals CleanUp
	for (p2List_item<TaxiStop*>* item = goals.getFirst(); item != nullptr; item = item->next) {
		delete item->data;
	}
	goals.clear();

	time_passed.Start();

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();

	Cube ground(800, 1, 800);
	ground.SetPos(0,-0.5,0);
	ground.color.Set(0.2f,0.2f,0.2f);
	ground.Render();

	//Debug Time function
	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	{
		max_time += 30;
	}

	//Check Disco Update
	if (disco.Read() > discoDelay) {
		for (p2List_item<Cube*>* item = buildings.getFirst(); item != nullptr; item = item->next) {
			red = (float)(rand() % 101) / 100.0f;
			green = (float)(rand() % 101) / 100.0f;
			blue = (float)(rand() % 101) / 100.0f;
			item->data->color.Set(red, green, blue);
		}
		disco.Start();
	}

	//City Rendering
	for (p2List_item<Cube*>* item = buildings.getFirst(); item != nullptr; item = item->next) {
		item->data->Render();
	}

	//Obstacle Rendering
	for (p2List_item<Obstacle*>* item = obstacles.getFirst(); item != nullptr; item = item->next) {
		if (item->data->dynamic) {
			item->data->body->GetTransform(&item->data->shape->transform);
		}
		item->data->shape->Render();
	}

	//Goals Rendering
	for (p2List_item<TaxiStop*>* item = goals.getFirst(); item != nullptr; item = item->next) {
		item->data->pole->Render();
		item->data->sign->Render();
	}

	//CHANGE/FIX Dídac: Testing some kind of arrow pointing to the destination
	arrowtest.color.Set(128, 128, 0, 0.7f);
	
	vec3 arrowpos = App->player->vehicle->GetPosition() + App->player->vehicle->GetForwardVec() + vec3(0,8,0);

	arrowtest.SetPos(arrowpos.x, arrowpos.y, arrowpos.z);
	
	vec3 objPos(taxiStop_positions[nextStop].x, taxiStop_positions[nextStop].y, taxiStop_positions[nextStop].z);

	distance_x = objPos.x - arrowpos.x;
	
	distance_z = objPos.z - arrowpos.z;
	
	float tan = M_PI/2;
	if (distance_x != 0.0f)
	{
		tan = distance_z / distance_x;
	}

	angle = -RADTODEG * atanf(tan);

	
	arrowtest.SetRotation(angle,vec3(0,1,0));
	
	float distance_mod = (float)sqrt(distance_x*distance_x + distance_z*distance_z);
	
	if (distance_mod > flagDetectionRadius)
		distance_mod = flagDetectionRadius;

	float red = (flagDetectionRadius - distance_mod) / flagDetectionRadius;
	float green = distance_mod / flagDetectionRadius;
	
	arrowtest.color.Set(red, green, 0.0f, 1.0f);

	arrowtest.Render();

	//CHANGE/FIX ARROWTEST ENDS HERE

	if (time_left > 0)
	{
		time_left = int(max_time - time_passed.Read() / 1000.0f);
	}
	

	if (time_left <= 0 && lost == false) {
		App->audio->PlayFx(App->audio->lose.id);
		lost = true;
	}
	else if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN) {
		lost = false;
		StartNewGame();
	}

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	if (body1 == taxiStop_sensor && App->player->stopped == true && lost == false)
	{
		if (collided == false) {
			goalTimer.Start();
			collided = true;
			nextStop++;
			if (nextStop < 5)
			{
				App->audio->PlayFx(App->audio->goal.id);
				game_destinations[nextStop - 1]->pole->color.Set(1.0f, 1.0f, 1.0f);
				game_destinations[nextStop - 1]->sign->color.Set(1.0f, 1.0f, 1.0f);

				taxiStop_sensor->SetPos(taxiStop_positions[nextStop].x, taxiStop_positions[nextStop].y - 2.5f, taxiStop_positions[nextStop].z);
				game_destinations[nextStop]->pole->color.Set(1.0f, 1.0f, 0.0f);
				game_destinations[nextStop]->sign->color.Set(1.0f, 1.0f, 0.0f);
				max_time += 30;
			}
			else if (nextStop >= 5)
			{
				won = true;
				game_destinations[4]->pole->color.Set(0.0f, 0.0f, 1.0f);
				game_destinations[4]->sign->color.Set(0.0f, 0.0f, 1.0f);
				taxiStop_sensor->SetPos(0, 100, 0);
				App->audio->PlayFx(App->audio->win.id);
				time_passed.Stop();
			}
		}
		else if (collided && goalTimer.Read() > goalCollisionDelay) {
			collided = false;
		}
	}
}

int ModuleSceneIntro::CreateCityBuildings()
{
	int counter = 0;

	Cube* tmpCube;

	for (float currX = city.cityStart; currX > -city.cityStart; currX -= city.buildingDistance) {
		for (float currZ = city.cityStart; currZ > -city.cityStart; currZ -= city.buildingDistance) {
			tmpCube = GenerateBuilding(currX, currZ);
			buildings.add(tmpCube);
			App->physics->AddBody(*tmpCube, 0.0f);
			counter++;
		}
	}

	return counter;
}

int ModuleSceneIntro::CreateCityGoals()
{
	int counter = 0;
	TaxiStop* tmpStop;
	int rng;

	for (float currX = city.roadStart; currX > -city.roadStart; currX -= city.buildingDistance) {
		for (float currZ = city.roadStart; currZ > -city.roadStart; currZ -= city.buildingDistance) {
			rng = rand() % 2;

			if (rng == 1) {
				tmpStop = GenerateGoal(currX, currZ);
				goals.add(tmpStop);
				counter++;
			}
		}
	}

	return counter;
}

int ModuleSceneIntro::CreateCityObstacles()
{
	int counter = 0;
	ObstacleType ret;

	for (float currX = city.roadStart; currX > -city.roadStart; currX -= city.buildingDistance) {
		for (float currZ = city.cityStart; currZ > -city.cityStart; currZ -= city.buildingDistance) {
			ret = GenerateObstacle(currX, currZ, false);
			if (ret != ObstacleType::NONE)
				counter++;
		}
	}
	for (float currX = city.cityStart; currX > -city.cityStart; currX -= city.buildingDistance) {
		for (float currZ = city.roadStart; currZ > -city.roadStart; currZ -= city.buildingDistance) {
			GenerateObstacle(currX, currZ, true);
			if (ret != ObstacleType::NONE)
				counter++;
		}
	}

	return counter;
}

Cube* ModuleSceneIntro::GenerateBuilding(float x, float z)
{
	city.building.height = (float)(rand() % city.building.randHeight) + city.building.minHeight;

	red = (float)(rand() % 101) / 100.0f;
	green = (float)(rand() % 101) / 100.0f;
	blue = (float)(rand() % 101) / 100.0f;

	Cube* tmpCube = new Cube(city.building.baseSize, city.building.height, city.building.baseSize);
	tmpCube->color.Set(red, green, blue);
	tmpCube->SetPos(x, city.building.height/2, z);

	return tmpCube;
}

TaxiStop* ModuleSceneIntro::GenerateGoal(float x, float z)
{
	TaxiStop* stop = new TaxiStop(1.0f, 10.0f, 1.0f);

	stop->pole->SetPos(x, 5.0f, z);
	stop->sign->SetPos(x + 2.5f, 10.0f - 1.0f, z);

	return stop;
}

ObstacleType ModuleSceneIntro::GenerateObstacle(float x, float z, bool xRoad)
{
	ObstacleType obstacle = (ObstacleType)(rand() % (int)ObstacleType::MAX_TYPES);

	switch (obstacle)
	{
	//Unmovable
	case ObstacleType::NONE:
		break;
	case ObstacleType::RAMP:
		GenerateRamp(x, z, xRoad);
		break;
	case ObstacleType::HOLE_RAMP:
		GenerateHoleRamp(x, z, xRoad);
		break;
	case ObstacleType::WALL:
		GenerateWall(x, z, xRoad);
		break;
	case ObstacleType::GROUND_BUMPS:
		GenerateGroundBumps(x, z, xRoad);
		break;
	case ObstacleType::GROUND_BARRIERS:
		GenerateGroundBarriers(x, z, xRoad);
		break;
	case ObstacleType::LAMP_POSTS:
		GenerateLampPosts(x, z, xRoad);
		break;
	case ObstacleType::BARRIER:
		GenerateBarriers(x, z, xRoad);
		break;
	
	//Movable
	case ObstacleType::BREAKABLE_WALL:
		GenerateBreakableWall(x, z, xRoad);
		break;
	case ObstacleType::MAILBOX:
		GenereateMailbox(x, z, xRoad);
		break;
	case ObstacleType::BOXES:
		GenerateBoxes(x, z, xRoad);
		break;
	case ObstacleType::SMALL_BARRIERS:
		GenerateSmallBarriers(x, z, xRoad);
		break;
	case ObstacleType::SIGN:
		GenerateSign(x, z, xRoad);
		break;
	case ObstacleType::WRECKING_BALL:
		GenerateWreckingBall(x, z, xRoad);
		break;
	}

	return obstacle;
}

void ModuleSceneIntro::GenerateHoleRamp(float x, float z, bool xRoad)
{
	Obstacle* ramp;
	Obstacle* wall;

	vec3 rampVector;

	if (xRoad == true) {
		rampVector.Set(0.0f, 0.0f, 1.0f);
		for (int i = -1; i < 2; i += 2) {
			ramp = new Obstacle;
			ramp->shape = new Cube(30.0f, 3.0f, 10.0f);
			ramp->shape->SetPos(x, 2.0f, z + (i * 10.0f));
			ramp->shape->SetRotation(15.0f, rampVector);
			ramp->shape->color.Set(1.0f, 0.0f, 0.0f);

			ramp->body = App->physics->AddBody(*(Cube*)(ramp->shape), 0.0f);
			obstacles.add(ramp);

			wall = new Obstacle;
			wall->shape = new Cube(1.0f, 2.0f, 10.0f);
			wall->shape->SetPos(x + 8.0f, 1.0f, z + (i * 10.0f));
			wall->shape->color.Set(1.0f, 0.0f, 0.0f);

			wall->body = App->physics->AddBody(*(Cube*)(wall->shape), 0.0f);
			obstacles.add(wall);
		}
	}
	else {
		rampVector.Set(1.0f, 0.0f, 0.0f);
		for (int i = -1; i < 2; i += 2) {
			ramp = new Obstacle;
			ramp->shape = new Cube(10.0f, 3.0f, 30.0f);
			ramp->shape->SetPos(x + (i * 10.0f), 2.0f, z);
			ramp->shape->SetRotation(15.0f, rampVector);
			ramp->shape->color.Set(1.0f, 0.0f, 0.0f);

			ramp->body = App->physics->AddBody(*(Cube*)(ramp->shape), 0.0f);
			obstacles.add(ramp);

			wall = new Obstacle;
			wall->shape = new Cube(10.0f, 2.0f, 1.0f);
			wall->shape->SetPos(x + (i * 10.0f), 1.0f, z - 8.0f);
			wall->shape->color.Set(1.0f, 0.0f, 0.0f);

			wall->body = App->physics->AddBody(*(Cube*)(wall->shape), 0.0f);
			obstacles.add(wall);
		}
	}
}

void ModuleSceneIntro::GenerateRamp(float x, float z, bool xRoad)
{
	vec3 vector;
	Obstacle* ramp1 = new Obstacle;
	Obstacle* ramp2 = new Obstacle;

	ramp1->shape = new Cube(20.0f, 3.0f, 20.0f);
	ramp2->shape = new Cube(20.0f, 3.0f, 20.0f);

	if (xRoad == true) {
		vector.Set(0.0f, 0.0f, 1.0f);
		ramp1->shape->SetPos(x + 10.0f, 0.0f, z);
		ramp2->shape->SetPos(x - 10.0f, 0.0f, z);
	}
	else {
		vector.Set(1.0f, 0.0f, 0.0f);
		ramp1->shape->SetPos(x, 0.0f, z - 10.0f);
		ramp2->shape->SetPos(x, 0.0f, z + 10.0f);
	}
	ramp1->shape->color.Set(1.0f, 0.0f, 0.0f);
	ramp2->shape->color.Set(1.0f, 0.0f, 0.0f);

	ramp1->shape->SetRotation(350.0f, vector);
	ramp2->shape->SetRotation(10.0f, vector);

	ramp1->body = App->physics->AddBody(*(Cube*)(ramp1->shape), 0.0f);
	ramp2->body = App->physics->AddBody(*(Cube*)(ramp2->shape), 0.0f);

	obstacles.add(ramp1);
	obstacles.add(ramp2);
}

void ModuleSceneIntro::GenerateWall(float x, float z, bool xRoad)
{
	Obstacle* wall = new Obstacle;

	if (xRoad == true) {
		wall->shape = new Cube(1.0f, 10.0f, 25.0f);
		wall->shape->SetPos(x + 24.5f, 5.0f, z - 2.5f);
	}
	else {
		wall->shape = new Cube(25.0f, 10.0f, 1.0f);
		wall->shape->SetPos(x - 2.5f, 5.0f, z + 24.5f);
	}

	wall->shape->color.Set(1.0f, 0.0f, 0.0f);

	wall->body = App->physics->AddBody(*(Cube*)wall->shape, 0.0f);
	obstacles.add(wall);
}

void ModuleSceneIntro::GenerateGroundBumps(float x, float z, bool xRoad)
{
	if (xRoad == true) {
		vec3 vector(0.0f, 1.0f, 0.0f);
		for (int i = -1; i < 2; i++) {
			Obstacle* bump = new Obstacle;
			bump->shape = new Cylinder(2.0f, 23.0f);
			bump->shape->color.Set(1.0f, 0.0f, 0.0f);
			bump->shape->SetPos(x + (15.0f * i), -0.9f, z);
			bump->shape->transform.rotate(90.0, vector);
			bump->body = App->physics->AddBody(*(Cylinder*)bump->shape, 0.0f);
			obstacles.add(bump);
		}
	}
	else {
		for (int i = -1; i < 2; i++) {
			Obstacle* bump = new Obstacle;
			bump->shape = new Cylinder(2.0f, 23.0f);
			bump->shape->color.Set(1.0f, 0.0f, 0.0f);
			bump->shape->SetPos(x, -0.9f, z + (15.0f * i));
			bump->body = App->physics->AddBody(*(Cylinder*)bump->shape, 0.0f);
			obstacles.add(bump);
		}
	};
}

void ModuleSceneIntro::GenerateGroundBarriers(float x, float z, bool xRoad)
{
	if (xRoad == true) {
		for (int i = -1; i < 2; i += 2) {
			Obstacle* barrier = new Obstacle;
			barrier->shape = new Cube(2.0f, 3.0f, 14.0f);
			barrier->shape->color.Set(1.0f, 0.0f, 0.0f);
			barrier->shape->SetPos(x + (10.0f * i), 1.5f, z + (8.0f * i));
			barrier->body = App->physics->AddBody(*(Cube*)barrier->shape, 0.0f);
			obstacles.add(barrier);
		}
	}
	else {
		for (int i = -1; i < 2; i += 2) {
			Obstacle* barrier = new Obstacle;
			barrier->shape = new Cube(14.0f, 3.0f, 2.0f);
			barrier->shape->color.Set(1.0f, 0.0f, 0.0f);
			barrier->shape->SetPos(x + (10.0f * i), 1.5f, z + (8.0f * i));
			barrier->body = App->physics->AddBody(*(Cube*)barrier->shape, 0.0f);
			obstacles.add(barrier);
		}
	}
}

void ModuleSceneIntro::GenerateLampPosts(float x, float z, bool xRoad)
{
	float lampWidth = 1.0f;

	if (xRoad == true) {
		for (int i = -1; i < 2; i++) {
			for (int j = -1; j < 2; j += 2) {
				Obstacle* post = new Obstacle;
				post->shape = new Cube(lampWidth, 14.0f, lampWidth);
				post->shape->color.Set(1.0f, 0.0f, 0.0f);
				post->shape->SetPos(x + (14.0f * i), 7.0f, z + (8.0f * j));
				post->body = App->physics->AddBody(*(Cube*)post->shape, 0.0f);
				obstacles.add(post);

				Obstacle* top = new Obstacle;
				top->shape = new Cube(2.5f, 1.0f, 4.0f);
				top->shape->color.Set(1.0f, 0.0f, 0.0f);
				top->shape->SetPos(x + (14.0f * i), 14.0f - 0.5f, z + (8.0f * j) - (j * 2.0f + j * lampWidth / 2));
				top->body = App->physics->AddBody(*(Cube*)top->shape, 0.0f);
				obstacles.add(top);
			}
		}
	}
	else {
		for (int i = -1; i < 2; i++) {
			for (int j = -1; j < 2; j += 2) {
				Obstacle* post = new Obstacle;
				post->shape = new Cube(lampWidth, 14.0f, lampWidth);
				post->shape->color.Set(1.0f, 0.0f, 0.0f);
				post->shape->SetPos(x + (8.0f * j), 7.0f, z + (14.0f * i));
				post->body = App->physics->AddBody(*(Cube*)post->shape, 0.0f);
				obstacles.add(post);

				Obstacle* top = new Obstacle;
				top->shape = new Cube(4.0f, 1.0f, 2.5f);
				top->shape->color.Set(1.0f, 0.0f, 0.0f);
				top->shape->SetPos(x + (8.0f * j) - (j * 2.0f + j * lampWidth / 2), 14.0f - 0.5f, z + (14.0f * i));
				top->body = App->physics->AddBody(*(Cube*)top->shape, 0.0f);
				obstacles.add(top);
			}
		}
	}
}

void ModuleSceneIntro::GenerateBarriers(float x, float z, bool xRoad)
{
	vec3 poleAnchor(0.0f, 1.0f, 0.0f);

	if (xRoad == true) {
		for (int j = -1; j < 2; j += 2) {
			for (int i = -1; i < 2; i += 2) {
				vec3 barrierAnchor(0.0f, -0.5f, i * 6.0f);
				Obstacle* pole = new Obstacle;
				pole->shape = new Cube(1.0f, 1.0f, 1.0f);
				pole->shape->SetPos(x + (j * 12.0f), 0.5f, z + (i * 14.0f));
				pole->shape->color.Set(1.0f, 0.0f, 0.0f);
				pole->body = App->physics->AddBody(*(Cube*)pole->shape, 0.0f);
				obstacles.add(pole);

				Obstacle* barrier = new Obstacle;
				barrier->shape = new Cube(1.0f, 2.0f, 14.0f);
				barrier->shape->SetPos(x + (j * 12.0f), 1.0f + 1.0f, z - (i * 6.0f) + (i * 14.0f));
				barrier->shape->color.Set(0.0f, 0.7f, 0.0f);
				barrier->dynamic = true;
				barrier->body = App->physics->AddBody(*(Cube*)barrier->shape, 50.0f);
				obstacles.add(barrier);

				App->physics->AddConstraintHinge(*pole->body, *barrier->body, poleAnchor, barrierAnchor, vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
			}
		}
	}
	else {
		for (int j = -1; j < 2; j += 2) {
			for (int i = -1; i < 2; i += 2) {
				vec3 barrierAnchor(i * 6.0f, -0.5f, 0.0f);
				Obstacle* pole = new Obstacle;
				pole->shape = new Cube(1.0f, 1.0f, 1.0f);
				pole->shape->SetPos(x + (i * 14.0f), 0.5f, z + (j * 12.0f));
				pole->shape->color.Set(1.0f, 0.0f, 0.0f);
				pole->body = App->physics->AddBody(*(Cube*)pole->shape, 0.0f);
				obstacles.add(pole);

				Obstacle* barrier = new Obstacle;
				barrier->shape = new Cube(14.0f, 2.0f, 1.0f);
				barrier->shape->SetPos(x - (i * 6.0f) + (i * 14.0f), 1.0f + 1.0f, z + (j * 12.0f));
				barrier->shape->color.Set(0.0f, 0.7f, 0.0f);
				barrier->dynamic = true;
				barrier->body = App->physics->AddBody(*(Cube*)barrier->shape, 50.0f);
				obstacles.add(barrier);

				App->physics->AddConstraintHinge(*pole->body, *barrier->body, poleAnchor, barrierAnchor, vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
			}
		}
	}
}

void ModuleSceneIntro::GenerateBreakableWall(float x, float z, bool xRoad)
{
	if (xRoad == true) {
		for (int j = 2.0f; j < 11.0f; j += 4.0f) {
			for (int i = -2; i < 3; i++) {
				Obstacle* box = new Obstacle;
				box->shape = new Cube(1.0f, 4.0f, 5.5f);
				box->shape->SetPos(x, j, z + (5.5f * i));
				box->shape->color.Set(0.0f, 1.0f, 0.0f);
				box->dynamic = true;
				box->body = App->physics->AddBody(*(Cube*)box->shape, 50.0f);
				obstacles.add(box);
			}
		}
	}
	else {
		for (int j = 2.0f; j < 11.0f; j += 4.0f) {
			for (int i = -2; i < 3; i++) {
				Obstacle* box = new Obstacle;
				box->shape = new Cube(5.5f, 4.0f, 1.0f);
				box->shape->SetPos(x + (5.5f * i), j, z);
				box->shape->color.Set(0.0f, 1.0f, 0.0f);
				box->dynamic = true;
				box->body = App->physics->AddBody(*(Cube*)box->shape, 50.0f);
				obstacles.add(box);
			}
		}
	}
}

void ModuleSceneIntro::GenereateMailbox(float x, float z, bool xRoad)
{
	if (xRoad == true) {
		for (int i = -1; i < 2; i += 2) {
			for (int j = -1; j < 2; j += 2) {
				Obstacle* mailBox = new Obstacle;
				mailBox->shape = new Cube(3.0f, 3.0f, 2.0f);
				mailBox->shape->color.Set(0.0f, 0.7f, 0.0f);
				mailBox->shape->SetPos(x + (8.0f * i), 1.5f, z + (6.0f * j));
				mailBox->dynamic = true;
				mailBox->body = App->physics->AddBody(*(Cube*)mailBox->shape, 50.0f);
				obstacles.add(mailBox);
			}
		}
	}
	else {
		for (int i = -1; i < 2; i += 2) {
			for (int j = -1; j < 2; j += 2) {
				Obstacle* mailBox = new Obstacle;
				mailBox->shape = new Cube(2.0f, 3.0f, 3.0f);
				mailBox->shape->color.Set(0.0f, 0.7f, 0.0f);
				mailBox->shape->SetPos(x + (6.0f * j), 1.5f, z + (8.0f * i));
				mailBox->dynamic = true;
				mailBox->body = App->physics->AddBody(*(Cube*)mailBox->shape, 50.0f);
				obstacles.add(mailBox);
			}
		}
	}
}

void ModuleSceneIntro::GenerateBoxes(float x, float z, bool xRoad)
{
	if (xRoad == true) {
		for (int i = -1; i < 2; i += 2) {
			Obstacle* box = new Obstacle;
			box->shape = new Cube(3.0f, 3.0f, 3.0f);
			box->shape->SetPos(x, 1.5f, z + (2.0f * i));
			box->shape->color.Set(0.0f, 0.5f, 0.0f);
			box->dynamic = true;
			box->body = App->physics->AddBody(*(Cube*)box->shape, 200.0f);
			obstacles.add(box);
		}

		Obstacle* box = new Obstacle;
		box->shape = new Cube(3.0f, 3.0f, 3.0f);
		box->shape->SetPos(x, 1.5f + 3.0f, z);
		box->shape->color.Set(0.0f, 0.5f, 0.0f);
		box->dynamic = true;
		box->body = App->physics->AddBody(*(Cube*)box->shape, 200.0f);
		obstacles.add(box);
	}
	else {
		for (int i = -1; i < 2; i += 2) {
			Obstacle* box = new Obstacle;
			box->shape = new Cube(3.0f, 3.0f, 3.0f);
			box->shape->SetPos(x + (2.0f * i), 1.5f, z);
			box->shape->color.Set(0.0f, 0.5f, 0.0f);
			box->dynamic = true;
			box->body = App->physics->AddBody(*(Cube*)box->shape, 200.0f);
			obstacles.add(box);
		}

		Obstacle* box = new Obstacle;
		box->shape = new Cube(3.0f, 3.0f, 3.0f);
		box->shape->SetPos(x, 1.5f + 3.0f, z);
		box->shape->color.Set(0.0f, 0.5f, 0.0f);
		box->dynamic = true;
		box->body = App->physics->AddBody(*(Cube*)box->shape, 200.0f);
		obstacles.add(box);
	}
}

void ModuleSceneIntro::GenerateSmallBarriers(float x, float z, bool xRoad)
{
	for (int i = -1; i < 2; i += 2) {
		Obstacle* barrier = new Obstacle;

		barrier->shape = new Cube(6.0f, 4.0f, 1.0f);
		barrier->shape->SetPos(x + (i * 5.0f), 2.0f, z + (i * 5.0f));
		barrier->shape->color.Set(0.0f, 0.3f, 0.0f);
		barrier->dynamic = true;
		barrier->shape->SetRotation(45.0f, vec3(0.0f, 1.0f, 0.0f));

		barrier->body = App->physics->AddBody(*(Cube*)barrier->shape, 200.0f);
		obstacles.add(barrier);
	}
}

void ModuleSceneIntro::GenerateSign(float x, float z, bool xRoad)
{
	Obstacle* pole = new Obstacle;
	Obstacle* sign = new Obstacle;

	if (xRoad == true) {
		pole->shape = new Cube(1.0f, 8.0f, 1.0f);
		sign->shape = new Cube(1.0f, 2.0f, 5.0f);
	}
	else {
		pole->shape = new Cube(1.0f, 8.0f, 1.0f);
		sign->shape = new Cube(5.0f, 2.0f, 1.0f);
	}
	pole->shape->SetPos(x + 6.0f, 4.0f, z + 6.0f);
	sign->shape->SetPos(x + 6.0f, 9.0f, z + 6.0f);

	pole->shape->color.Set(0.0f, 0.4f, 0.0f);
	sign->shape->color.Set(0.0f, 0.8f, 0.0f);

	pole->dynamic = sign->dynamic = true;

	pole->body = App->physics->AddBody(*(Cube*)pole->shape, 50.0f);
	sign->body = App->physics->AddBody(*(Cube*)sign->shape, 50.0f);
	obstacles.add(pole);
	obstacles.add(sign);

	App->physics->AddConstraintP2P(*pole->body, *sign->body, vec3(0.0f, 5.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f));
}

void ModuleSceneIntro::GenerateWreckingBall(float x, float z, bool xRoad)
{
	vec3 cord;

	Obstacle* support = new Obstacle;
	support->shape = new Sphere(0.1f);
	support->shape->SetPos(x, 44.3f, z);
	support->shape->color.Set(1.0f, 0.0f, 0.0f);

	Obstacle* ball = new Obstacle;
	ball->shape = new Sphere(4.0f);
	ball->shape->color.Set(0.0f, 0.1f, 0.0f);
	ball->dynamic = true;

	if (xRoad == true) {
		ball->shape->SetPos(x + 40.0f, 60.0f, z);
		cord.Set(-40.0f, 0.0f, 0.0f);
	}
	else {
		ball->shape->SetPos(x, 40.0f, z + 40.0f);
		cord.Set(0.0f, 0.0f, -40.0f);
	}
	
	support->body = App->physics->AddBody(*(Sphere*)support->shape, 0.0f);
	ball->body = App->physics->AddBody(*(Sphere*)ball->shape, 10000.0f);

	obstacles.add(support);
	obstacles.add(ball);

	App->physics->AddConstraintP2P(*support->body, *ball->body, vec3(0.0f, 0.0f, 0.0f), cord);
}

void ModuleSceneIntro::ChooseGameplayGoals()
{
	int indexes[5];
	for (int i = 0; i < 5; ++i)
	{
		indexes[i] = (rand() % goals.count());
	}

	for (int i = 0; i < 5; ++i)
	{
		for (int j = 0; j < 5; ++j)
		{
			if (!(indexes[i] != indexes[j]) && i != j)
			{
				indexes[i] = (rand() % goals.count());
				i = 0;
				j = 0;
				break;
			}
		}
	}

	for (int i = 0; i < 5; ++i)
	{
		goals.at(indexes[i], game_destinations[i]);
	}
}

void ModuleSceneIntro::StartNewGame()
{
	App->physics->CleanUp();
	App->scene_intro->CleanUp();
	App->physics->Start();
	App->scene_intro->Start();
	App->player->Start();
}
