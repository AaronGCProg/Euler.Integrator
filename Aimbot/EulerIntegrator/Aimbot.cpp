#include "Aimbot.h"
#include "Application.h"
#include "Physics.h"
#include "Scene.h"
#include "Collisions.h"
#include "Input.h"
#include "Render.h"

#include <time.h>


#define MONTECARLO_ITERATION 100
#define PROPAGATION 100

ModuleAimbot::ModuleAimbot(Application * app, bool start_enabled) : Module(app, start_enabled)
{
	realTimeMC = false;
	nTrace = 0u;
}

ModuleAimbot::~ModuleAimbot() {}

bool ModuleAimbot::Start() {

	double radius = 0.5f;
	aimbot = new Object({ PIXEL_TO_METERS(SCREEN_WIDTH / 3), PIXEL_TO_METERS(SCREEN_HEIGHT - radius) }, radius, { 0.0f, 0.0f }, { 0.0f, 0.0f }, 5.0f, 0.1f, true, COLLISION_BACK, "aimbot");
	App->physics->AddObject(aimbot);
	state = AimbotStates::AIMBOT_IDLE;

	srand(time(NULL));

	target = nullptr;

	propagationObj = nullptr;

	return true;
}

update_status ModuleAimbot::Update(float dt) {

	HandleInput();

	switch (state) {

	case AimbotStates::AIMBOT_IDLE:

		break;

	case AimbotStates::AIMBOT_CALCULATE_MONTECARLO:

		if (propagationObj == nullptr) {
			double propagationRadius = 0.2f;
			propagationObj = new Object({ 5.0f, SCREEN_HEIGHT - propagationRadius }, propagationRadius, { 0.0f, 0.0f }, { 0.0f, 0.0f }, 5.0f, 0.5f, false, COLLISION_FRONT, "propagation");
			App->physics->AddObject(propagationObj);
		}

		if (TargetExists()) {
			trajectory = CalculateTrajectory(dt);
			LOG("Ready to shoot, baby");
			propagationObj->pos.x = aimbot->pos.x;
			propagationObj->pos.y = aimbot->pos.y;
			state = AimbotStates::AIMBOT_CALCULATED_MONTECARLO;
		}

		break;

	case AimbotStates::AIMBOT_CALCULATED_MONTECARLO:

		if (nTrace > 0) //Check if MonteCarlo has been found
		{
			for (int i = 1; i < nTrace - 1; i++)
			{
				App->renderer->DrawLine(trajectory.trace[i].x, trajectory.trace[i].y, trajectory.trace[i + 1].x, trajectory.trace[i + 1].y, 255, 0, 0, 255, false);
			}
		}

		//Do not log here. It does it every frame
		propagationObj->pos.x = aimbot->pos.x;
		propagationObj->pos.y = aimbot->pos.y;

		break;

	case AimbotStates::AIMBOT_SHOOT:

		propagationObj->SetSpeed(trajectory.speed, trajectory.angle);
		trajectory.angle = 0; trajectory.speed = 0;

		state = AimbotStates::AIMBOT_IDLE;
		break;
	}

	return UPDATE_CONTINUE;
}


void ModuleAimbot::HandleInput() {

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN) {

		TargetLogic();

		ResetProjectile();

		state = AimbotStates::AIMBOT_CALCULATE_MONTECARLO;
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && state == AimbotStates::AIMBOT_CALCULATED_MONTECARLO) {

		state = AimbotStates::AIMBOT_SHOOT;
	}
}


bool ModuleAimbot::CleanUp()
{
	aimbot = nullptr;
	propagationObj = nullptr;
	target = nullptr;

	return true;
}

// Trajectory with Montecarlo method
Trajectory ModuleAimbot::CalculateTrajectory(float dt) {

	dPoint auxPos = aimbot->pos;
	dPoint auxSpeed(0, 0);

	Trajectory result;
	Trajectory AuxResult;

	int TimeCompare = 100;
	int AuxTimeCompare = 100;

	ClearTrace();

	for (int i = 0; i < MONTECARLO_ITERATION; i++)
	{

		float seedSpeed = 13 + rand() % 5;
		float seedAngle = 180 + rand() % 180;

		propagationObj->SetSpeed(seedSpeed, seedAngle);
		propagationObj->pos = auxPos;

		for (int j = 0; j < PROPAGATION; j++)
		{
			App->physics->Integrate(propagationObj, App->physics->world->gravity, dt);

			if (realTimeMC)
				RealTimePropagation();

			AuxResult.trace[j].x = METERS_TO_PIXELS(propagationObj->pos.x);
			AuxResult.trace[j].y = METERS_TO_PIXELS(propagationObj->pos.y);

			if (propagationObj->AccurateCheckCollision(target))
			{
				AuxResult.angle = seedAngle;
				AuxResult.speed = seedSpeed;
				AuxTimeCompare = j;

				if (AuxTimeCompare < TimeCompare)
				{
					result = AuxResult;
					TimeCompare = nTrace = j;
				}
			}
		}
	}

	propagationObj->pos = auxPos;
	propagationObj->speed = auxSpeed;

	return result;
}


void ModuleAimbot::TargetLogic()
{
	if (App->scene->GetMouseJointActive() == false) {

		if (TargetExists())
			DeleteTarget();

		CreateTarget();
	}
}


bool ModuleAimbot::TargetExists() {

	if (target != nullptr) { return true; }
	return false;
}

Object* ModuleAimbot::GetTarget() {

	return target;
}


void ModuleAimbot::CreateTarget()
{

	dPoint position = { PIXEL_TO_METERS((double)App->input->GetMouseX()), PIXEL_TO_METERS((double)App->input->GetMouseY()) };
	target = new Object(position, 0.5f, { 0.0f, 0.0f }, { 0.0f, 0.0f }, 10.0f, 0.1f, true, COLLISION_FRONT, "target");
	App->physics->AddObject(target);
}


void ModuleAimbot::DeleteTarget() {

	App->physics->DeleteObject(target);
	target = nullptr;
}


void ModuleAimbot::ResetProjectile() {

	if (propagationObj != nullptr) {

		App->physics->DeleteObject(propagationObj);
		propagationObj = nullptr;
	}
}

void ModuleAimbot::RealTimePropagation()
{
	//Check Inputs inside MonteCarlo
	App->input->PreUpdate();

	//Clean the screen
	App->renderer->PreUpdate();

	//Draw the Objects
	App->physics->PostUpdate();

	//Change the screen
	App->renderer->PostUpdate();

	//Skip watching Monte
	if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_REPEAT) 
	{
		App->aimbot->SetRealTimeMC();
	}
}

void ModuleAimbot::SetRealTimeMC()
{
	realTimeMC = !realTimeMC;
}

void ModuleAimbot::ClearTrace()
{
	for (uint i = 0; i < MAX_TRACE_POINTS; i++)
	{
		trajectory.trace[i] = { 0,0 };
	}

	nTrace = 0u;
}
