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
	

	//Test Carles
	float cityRadius = 300.0f;
	float buildingDistance = buildData.baseSize + buildData.roadSize;
	float crossings = cityRadius + buildingDistance / 2;

	Cube* tmpCube;

	srand((uint)time(NULL));
	for (float currX = cityRadius; currX > -cityRadius; currX -= buildingDistance) {
		for (float currZ = cityRadius; currZ > -cityRadius; currZ -= buildingDistance) {
			tmpCube = GenerateBuilding(currX, currZ);
			buildings.add(tmpCube);
			App->physics->AddBody(*tmpCube, 0.0f);
		}
	}

	for (float currX = crossings; currX > -crossings; currX -= buildingDistance) {
		for (float currZ = crossings; currZ > -crossings; currZ -= buildingDistance) {
			CreateObstacle(currX, currZ);
		}
	}

	//Test carles 2
	/*c2 = new Cube(2, 1, 2);
	c2->SetPos(30.0f, 0.5f, 0);
	c2->color.Set(0, 200, 100);

	test = App->physics->AddBody(*c2, 1.0f);*/

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

	//City cleanup
	for (p2List_item<Cube*>* item = buildings.getFirst(); item != nullptr; item = item->next) {
		delete item->data;
	}
	buildings.clear();

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();

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
	vec3 objPos(15, 1, 30);

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
	if (body1 == test)
	{
		int workpls = 1;
	}
}

Cube* ModuleSceneIntro::GenerateBuilding(float x, float z)
{
	buildData.height = (float)(rand() % buildData.randHeight) + buildData.minHeight;

	red = (float)(rand() % 101) / 100.0f;
	green = (float)(rand() % 101) / 100.0f;
	blue = (float)(rand() % 101) / 100.0f;

	Cube* tmpBuilding = new Cube(buildData.baseSize, buildData.height, buildData.baseSize);
	tmpBuilding->color.Set(red, green, blue);
	tmpBuilding->SetPos(x, buildData.height/2, z);

	return tmpBuilding;
}

ObstacleType ModuleSceneIntro::CreateObstacle(float x, float z)
{
	Cube tmpBuilding(5.0f, 10.0f, 5.0f);
	tmpBuilding.color.Set(1.0f, 1.0f, 1.0f);
	tmpBuilding.SetPos(x, 10.0f / 2, z);

	App->physics->AddBody(tmpBuilding, 0.0f);

	return ObstacleType::NONE;
}