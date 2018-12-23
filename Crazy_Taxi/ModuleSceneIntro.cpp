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


	//Stuff to test
	Cube* CubeTest = new Cube(2, 1, 2);
	CubeTest->SetPos(0, 2.5f, 10);
	CubeTest->color.Set(0, 200, 100);

	c1 = CubeTest;
	
	test = App->physics->AddBody(*c1,0.0f);

	//TESTING Didac
	test->SetAsSensor(true);
	test->collision_listeners.add(this);
	vec3 testPosition = test->GetPosition();

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
		/*game_destinations[i]->pole->color.Set(1.0f, 1.0f, 0);
		game_destinations[i]->sign->color.Set(1.0f, 1.0f, 0);*/

		//Place Test sensor in all of the Taxi Stops
		taxiStop_positions[i] = game_destinations[i]->pole->GetPosition();
	}
	game_destinations[0]->pole->color.Set(1.0f,1.0f,0.0f);
	game_destinations[0]->sign->color.Set(1.0f, 1.0f, 0.0f);
	//Place Test sensor in all of the Taxi Stops
	Cube sensor(8, 4, 8);
	sensor.SetPos(taxiStop_positions[nextStop].x, taxiStop_positions[nextStop].y - 2.5, taxiStop_positions[nextStop].z);
	taxiStop_sensor = App->physics->AddBody(sensor, 0.0f);
	taxiStop_sensor->SetAsSensor(true);
	taxiStop_sensor->collision_listeners.add(App->scene_intro);

	//Music:
	App->audio->PlayMusic("audio/Yellow_Line.ogg");
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
	for (p2List_item<Primitive*>* item = obstacles.getFirst(); item != nullptr; item = item->next) {
		delete item->data;
	}
	obstacles.clear();

	//Goals CleanUp
	for (p2List_item<TaxiStop*>* item = goals.getFirst(); item != nullptr; item = item->next) {
		delete item->data;
	}
	goals.clear();

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

	c1->Render();

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
	for (p2List_item<Primitive*>* item = obstacles.getFirst(); item != nullptr; item = item->next) {
		item->data->Render();
	}

	//Goals Rendering
	for (p2List_item<TaxiStop*>* item = goals.getFirst(); item != nullptr; item = item->next) {
		item->data->pole->Render();
		item->data->sign->Render();
	}

	Cube c(2,1,2);
	c.SetPos(15,1,30);
	c.color.Set(0,255,0,200.0f);
	c.Render();

	//CHANGE/FIX Dídac: Testing some kind of arrow pointing to the destination
	Cylinder arrowtest(0.5,3);
	arrowtest.color.Set(128, 128, 0, 200.0f);
	
	vec3 arrowpos = App->player->vehicle->GetPosition() + App->player->vehicle->GetForwardVec() + vec3(0,5,0);
	vec3 test(1,0,0);

	
	/*arrowtest.transform.look(vec3(arrowpos.x, arrowpos.y, arrowpos.z), vec3(15, 1, 30),  vec3(0,1.0f,0));
	
	arrowtest.SetPos(arrowpos.x, arrowpos.y, arrowpos.z);*/
	//arrowtest.SetRotation(90, test);
	

	//arrowtest.SetRotation();
	//arrowtest.LookAt(arrowpos,vec3(15, 1, 30));
	//arrowtest.SetPos(arrowpos.x, arrowpos.y, arrowpos.z);

	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)
	{
		x += 1;
	}
	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)
	{
		z += 1;
	}
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		y += 1;
	}

	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT)
	{
		angle+= 1;
	}

	

	arrowtest.SetPos(arrowpos.x, arrowpos.y, arrowpos.z);
	//arrowtest.SetRotation(angle,vec3(x,y,z));
	//arrowtest.transform.M[4] = angle;




	//HERE WE MIGHT HAVE IT BOIIIIIIIIIIIIIIII
	vec3 objPos(taxiStop_positions[nextStop].x, taxiStop_positions[nextStop].y, taxiStop_positions[nextStop].z);

	distance_x = objPos.x - arrowpos.x;
	
	distance_z = objPos.z - arrowpos.z;
	
	float tan = M_PI/2;
	if (distance_x != 0)
	{
		tan = float(distance_z / distance_x);
	}
	
	//angle = atan2(distance_z , distance_x);
	angle = - RADTODEG * atanf(tan);

	
	arrowtest.SetRotation(angle,vec3(0,1,0));
	
	//arrowtest.transform.M[6] = angle;
	arrowtest.Render();


	//CHANGE/FIX ARROWTEST ENDS HERE

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	if (body1 == taxiStop_sensor)
	{
		nextStop++;
		if (nextStop < 5)
		{
			game_destinations[nextStop - 1]->pole->color.Set(1.0f, 1.0f, 1.0f);
			game_destinations[nextStop - 1]->sign->color.Set(1.0f, 1.0f, 1.0f);

			taxiStop_sensor->SetPos(taxiStop_positions[nextStop].x, taxiStop_positions[nextStop].y-2.5f, taxiStop_positions[nextStop].z);
			game_destinations[nextStop]->pole->color.Set(1.0f, 1.0f, 0.0f);
			game_destinations[nextStop]->sign->color.Set(1.0f, 1.0f, 0.0f);
		}
	}
	int workpls = 1;
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
		GenerateBarrier(x, z, xRoad);
		break;
	
	//Movable
	case ObstacleType::BENCHES:
		GenerateBenches(x, z, xRoad);
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
	case ObstacleType::CONES:
		GenerateCones(x, z, xRoad);
		break;
	case ObstacleType::SIGN:
		GenerateSign(x, z, xRoad);
		break;
	}

	return obstacle;
}

void ModuleSceneIntro::GenerateHoleRamp(float x, float z, bool xRoad)
{
	Cube* ramp;
	Cube* wall;
	vec3 rampVector;

	if (xRoad == true) {
		rampVector.Set(0.0f, 0.0f, 1.0f);
		for (int i = -1; i < 2; i += 2) {
			ramp = new Cube(30.0f, 3.0f, 10.0f);
			ramp->SetPos(x, 2.0f, z + (i * 10.0f));
			ramp->SetRotation(15.0f, rampVector);
			ramp->color.Set(1.0f, 0.0f, 0.0f);
			obstacles.add(ramp);
			App->physics->AddBody(*ramp, 0.0f);

			wall = new Cube(1.0f, 2.0f, 10.0f);
			wall->SetPos(x + 8.0f, 1.0f, z + (i * 10.0f));
			wall->color.Set(1.0f, 0.0f, 0.0f);
			obstacles.add(wall);
			App->physics->AddBody(*wall, 0.0f);
		}
	}
	else {
		rampVector.Set(1.0f, 0.0f, 0.0f);
		for (int i = -1; i < 2; i += 2) {
			ramp = new Cube(10.0f, 3.0f, 30.0f);
			ramp->SetPos(x + (i * 10.0f), 2.0f, z);
			ramp->SetRotation(15.0f, rampVector);
			ramp->color.Set(1.0f, 0.0f, 0.0f);
			obstacles.add(ramp);
			App->physics->AddBody(*ramp, 0.0f);

			wall = new Cube(10.0f, 2.0f, 1.0f);
			wall->SetPos(x + (i * 10.0f), 1.0f, z - 8.0f);
			wall->color.Set(1.0f, 0.0f, 0.0f);
			obstacles.add(wall);
			App->physics->AddBody(*wall, 0.0f);
		}
	}
}

void ModuleSceneIntro::GenerateRamp(float x, float z, bool xRoad)
{
	Cube* ramp1 = new Cube(20.0f, 3.0f, 20.0f);
	Cube* ramp2 = new Cube(20.0f, 3.0f, 20.0f);
	vec3 vector;

	if (xRoad == true) {
		vector.Set(0.0f, 0.0f, 1.0f);
		ramp1->SetPos(x + 10.0f, 0.0f, z);
		ramp2->SetPos(x - 10.0f, 0.0f, z);
	}
	else {
		vector.Set(1.0f, 0.0f, 0.0f);
		ramp1->SetPos(x, 0.0f, z - 10.0f);
		ramp2->SetPos(x, 0.0f, z + 10.0f);
	}
	ramp1->color.Set(1.0f, 0.0f, 0.0f);
	ramp2->color.Set(1.0f, 0.0f, 0.0f);

	ramp1->SetRotation(350.0f, vector);
	ramp2->SetRotation(10.0f, vector);

	obstacles.add(ramp1);
	App->physics->AddBody(*ramp1, 0.0f);
	obstacles.add(ramp2);
	App->physics->AddBody(*ramp2, 0.0f);
}

void ModuleSceneIntro::GenerateWall(float x, float z, bool xRoad)
{
	Cube* tmpCube = new Cube;
	vec3 vector;

	if (xRoad == true) {
		vector.Set(1.0f, 10.0f, 25.0f);
		tmpCube->size = vector;
		tmpCube->SetPos(x + 24.5f, 5.0f, z - 2.5f);
	}
	else {
		vector.Set(25.0f, 10.0f, 1.0f);
		tmpCube->size = vector;
		tmpCube->SetPos(x - 2.5f, 5.0f, z + 24.5f);
	}

	tmpCube->color.Set(1.0f, 0.0f, 0.0f);

	obstacles.add(tmpCube);
	App->physics->AddBody(*tmpCube, 0.0f);
}

void ModuleSceneIntro::GenerateGroundBumps(float x, float z, bool xRoad)
{
	if (xRoad == true) {
		vec3 vector(0.0f, 1.0f, 0.0f);
		for (int i = -1; i < 2; i++) {
			Cylinder* tmpCylinder = new Cylinder(2.0f, 23.0f);
			tmpCylinder->color.Set(1.0f, 0.0f, 0.0f);
			tmpCylinder->SetPos(x + (15.0f * i), -0.9f, z);
			tmpCylinder->transform.rotate(90.0, vector);
			obstacles.add(tmpCylinder);
			App->physics->AddBody(*tmpCylinder, 0.0f);
		}
	}
	else {
		for (int i = -1; i < 2; i++) {
			Cylinder* tmpCylinder = new Cylinder(2.0f, 23.0f);
			tmpCylinder->color.Set(1.0f, 0.0f, 0.0f);
			tmpCylinder->SetPos(x, -0.9f, z + (15.0f * i));
			obstacles.add(tmpCylinder);
			App->physics->AddBody(*tmpCylinder, 0.0f);
		}
	};
}

void ModuleSceneIntro::GenerateGroundBarriers(float x, float z, bool xRoad)
{
	if (xRoad == true) {
		for (int i = -1; i < 2; i += 2) {
			Cube* tmpCube = new Cube(2.0f, 3.0f, 14.0f);
			tmpCube->color.Set(1.0f, 0.0f, 0.0f);
			tmpCube->SetPos(x + (10.0f * i), 1.5f, z + (8.0f * i));
			obstacles.add(tmpCube);
			App->physics->AddBody(*tmpCube, 0.0f);
		}
	}
	else {
		for (int i = -1; i < 2; i += 2) {
			Cube* tmpCube = new Cube(14.0f, 3.0f, 2.0f);
			tmpCube->color.Set(1.0f, 0.0f, 0.0f);
			tmpCube->SetPos(x + (8.0f * i), 1.5f, z + (10.0f * i));
			obstacles.add(tmpCube);
			App->physics->AddBody(*tmpCube, 0.0f);
		}
	}
}

void ModuleSceneIntro::GenerateLampPosts(float x, float z, bool xRoad)
{
	float lampWidth = 1.0f;

	if (xRoad == true) {
		for (int i = -1; i < 2; i++) {
			for (int j = -1; j < 2; j += 2) {
				Cube* tmpCube = new Cube(lampWidth, 14.0f, lampWidth);
				tmpCube->color.Set(1.0f, 0.0f, 0.0f);
				tmpCube->SetPos(x + (14.0f * i), 7.0f, z + (8.0f * j));
				obstacles.add(tmpCube);
				App->physics->AddBody(*tmpCube, 0.0f);

				Cube* lampTop = new Cube(2.5f, 1.0f, 4.0f);
				lampTop->color.Set(1.0f, 0.0f, 0.0f);
				lampTop->SetPos(x + (14.0f * i), 14.0f - 0.5f, z + (8.0f * j) - (j * 2.0f + j * lampWidth / 2));
				obstacles.add(lampTop);
				App->physics->AddBody(*lampTop, 0.0f);
			}
		}
	}
	else {
		for (int i = -1; i < 2; i++) {
			for (int j = -1; j < 2; j += 2) {
				Cube* tmpCube = new Cube(lampWidth, 14.0f, lampWidth);
				tmpCube->color.Set(1.0f, 0.0f, 0.0f);
				tmpCube->SetPos(x + (8.0f * j), 7.0f, z + (14.0f * i));
				obstacles.add(tmpCube);
				App->physics->AddBody(*tmpCube, 0.0f);

				Cube* lampTop = new Cube(4.0f, 1.0f, 2.5f);
				lampTop->color.Set(1.0f, 0.0f, 0.0f);
				lampTop->SetPos(x + (8.0f * j) - (j * 2.0f + j * lampWidth / 2), 14.0f - 0.5f, z + (14.0f * i));
				obstacles.add(lampTop);
				App->physics->AddBody(*lampTop, 0.0f);
			}
		}
	}

	Cube* tmpCube = new Cube(5.0f, 10.0f, 5.0f);
	tmpCube->color.Set(1.0f, 1.0f, 1.0f);
	tmpCube->SetPos(x, 10.0f / 2, z);
}

void ModuleSceneIntro::GenerateBarrier(float x, float z, bool xRoad)
{

}

void ModuleSceneIntro::GenerateBenches(float x, float z, bool xRoad)
{


}
void ModuleSceneIntro::GenereateMailbox(float x, float z, bool xRoad)
{

}

void ModuleSceneIntro::GenerateBoxes(float x, float z, bool xRoad)
{

}

void ModuleSceneIntro::GenerateSmallBarriers(float x, float z, bool xRoad)
{

}

void ModuleSceneIntro::GenerateCones(float x, float z, bool xRoad)
{

}

void ModuleSceneIntro::GenerateSign(float x, float z, bool xRoad)
{

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