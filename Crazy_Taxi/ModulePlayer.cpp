#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "Primitive.h"
#include "PhysVehicle3D.h"
#include "PhysBody3D.h"

//CHANGE/FIX DIDAC
#include"ModuleSceneIntro.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled), vehicle(NULL)
{
	turn = acceleration = brake = 0.0f;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	CreatePlayer(0,4,10);
	
	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

// Update: draw background
update_status ModulePlayer::Update(float dt)
{
	turn = acceleration = brake = 0.0f;

	if (App->scene_intro->lost == App->scene_intro->won)
	{
		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		{
			acceleration = MAX_ACCELERATION;
		}

		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		{
			if (turn < TURN_DEGREES)
				turn += TURN_DEGREES;
		}

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		{
			if (turn > -TURN_DEGREES)
				turn -= TURN_DEGREES;
		}

		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		{
			if (vehicle->GetKmh() > 0.0f)
				brake = BRAKE_POWER;
			else
				acceleration = -MAX_ACCELERATION / 2;
		}

		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT)
		{
			if (vehicle->GetKmh() > 0.0f)
				brake = BRAKE_POWER;
			else {
				acceleration = MAX_ACCELERATION;
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
		{
			vehicle->SetPos(vehicle->GetPosition().x, vehicle->GetPosition().y + 4, vehicle->GetPosition().z);
			App->physics->SetVehicleRotation(vehicle,vec3(0,0,1), -90);
		}
	}
	else
	{
		brake = BRAKE_POWER;
	}

	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		camera_debug = !camera_debug;
	}

	if (vehicle->GetKmh() < 0.2 && vehicle->GetKmh() > -0.2f) {
		stopped = true;
	}
	else {
		stopped = false;
	}

	vehicle->ApplyEngineForce(acceleration);
	vehicle->Turn(turn);
	vehicle->Brake(brake);

	vehicle->Render();

	char title[140];
	if (App->scene_intro->lost == false && App->scene_intro->won == false)
	{
		sprintf_s(title, "Crazy Taxi! %.1f Km/h, Stopped = %d, Time left= %d", vehicle->GetKmh(), (int)stopped, App->scene_intro->time_left);
	}
	else if (App->scene_intro->lost == true)
	{
		sprintf_s(title, "Crazy Taxi! 0.0 Km/h, Stopped = %d,Time left= %d, GAME OVER You LOST! Press R to Restart the game", (int)stopped, App->scene_intro->time_left);
	}
	else if (App->scene_intro->won == true)
	{
		sprintf_s(title, "Crazy Taxi! 0.0 Km/h, Stopped = %d,Time left= %d, CONGRATULATIONS You WON! Press R to Restart the game", (int)stopped, App->scene_intro->time_left);
	}

	App->window->SetTitle(title);

	if (camera_debug == false)
	{
		App->camera->LookAt(vehicle->GetPosition());
		//App->camera->Position = vehicle->GetPosition() + vehicle->GetForwardVec() * -1;

		App->camera->Position = (vehicle->GetPosition() - vehicle->GetForwardVec() * 20) + vec3(0, 6, 0);

		if (App->camera->Position.y < 0.5f)
			App->camera->Position.y = 0.5f;
	}
	
	
	return UPDATE_CONTINUE;
}



void ModulePlayer::CreatePlayer(float x, float y, float z)
{
	VehicleInfo car;

	// Car properties ----------------------------------------
	car.chassis_size.Set(3.0f, 1.0f, 4.0f);
	car.chassis_offset.Set(0.0f, 1.0f, 0.0f);
	car.sign_size.Set(0.7f, 0.7f, 1.7f);
	car.sign_offset.Set(0.0f, 2.0, 0.0f);
	car.top_size.Set(0.5f, 1.0f, 2.0f);
	car.top_offset.Set(0.0f, 2.0f, 0.0f);
	car.mass = 650.0f;
	car.suspensionStiffness = 30.0f;
	car.suspensionCompression = 1.0f;
	car.suspensionDamping = 3.0;
	car.maxSuspensionTravelCm = 500.0f;
	car.frictionSlip = 10.0;
	car.maxSuspensionForce = 12000.0f;

	// Wheel properties ---------------------------------------
	float connection_height = 1.2f;
	float wheel_radius = 0.6f;
	float wheel_width = 0.5f;
	float suspensionRestLength = 1.2f;

	// Don't change anything below this line ------------------

	float half_width = car.chassis_size.x*0.5f;
	float half_length = car.chassis_size.z*0.5f;

	vec3 direction(0, -1, 0);
	vec3 axis(-1, 0, 0);

	car.num_wheels = 4;
	car.wheels = new Wheel[4];

	// FRONT-LEFT ------------------------
	car.wheels[0].connection.Set(half_width - 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[0].direction = direction;
	car.wheels[0].axis = axis;
	car.wheels[0].suspensionRestLength = suspensionRestLength;
	car.wheels[0].radius = wheel_radius;
	car.wheels[0].width = wheel_width;
	car.wheels[0].front = true;
	car.wheels[0].drive = true;
	car.wheels[0].brake = false;
	car.wheels[0].steering = true;

	// FRONT-RIGHT ------------------------
	car.wheels[1].connection.Set(-half_width + 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[1].direction = direction;
	car.wheels[1].axis = axis;
	car.wheels[1].suspensionRestLength = suspensionRestLength;
	car.wheels[1].radius = wheel_radius;
	car.wheels[1].width = wheel_width;
	car.wheels[1].front = true;
	car.wheels[1].drive = true;
	car.wheels[1].brake = false;
	car.wheels[1].steering = true;

	// REAR-LEFT ------------------------
	car.wheels[2].connection.Set(half_width - 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[2].direction = direction;
	car.wheels[2].axis = axis;
	car.wheels[2].suspensionRestLength = suspensionRestLength;
	car.wheels[2].radius = wheel_radius;
	car.wheels[2].width = wheel_width;
	car.wheels[2].front = false;
	car.wheels[2].drive = false;
	car.wheels[2].brake = true;
	car.wheels[2].steering = false;

	// REAR-RIGHT ------------------------
	car.wheels[3].connection.Set(-half_width + 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[3].direction = direction;
	car.wheels[3].axis = axis;
	car.wheels[3].suspensionRestLength = suspensionRestLength;
	car.wheels[3].radius = wheel_radius;
	car.wheels[3].width = wheel_width;
	car.wheels[3].front = false;
	car.wheels[3].drive = false;
	car.wheels[3].brake = true;
	car.wheels[3].steering = false;

	vehicle = App->physics->AddVehicle(car);
	vehicle->SetPos(x, y, z);
}



