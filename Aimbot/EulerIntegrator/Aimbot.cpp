#include "Aimbot.h"
#include "Application.h"
#include "Physics.h"
#include "Scene.h"
#include "Collisions.h"
#include "Input.h"

#include <time.h>


#define MONTECARLO_ITERATION 300
#define PROPAGATION 100

ModuleAimbot::ModuleAimbot(Application* app, bool start_enabled) : Module(app, start_enabled) {}

ModuleAimbot::~ModuleAimbot() {}

bool ModuleAimbot::Start() {

	double radius = 0.5f;
	aimbot = new Object({ PIXEL_TO_METERS(SCREEN_WIDTH / 3), PIXEL_TO_METERS(SCREEN_HEIGHT - radius) }, radius, { 0.0f, 0.0f }, { 0.0f, 0.0f }, 5.0f, 0.1f, true, COLLISION_BACK, "aimbot");
	App->physics->AddObject(aimbot);
	state = AimbotStates::AIMBOT_IDLE;

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
			double propagationRadius = 0.1f;
			propagationObj = new Object({ 5.0f, SCREEN_HEIGHT - propagationRadius }, propagationRadius, { 0.0f, 0.0f }, { 0.0f, 0.0f }, 5.0f, 0.1f, false, COLLISION_FRONT, "propagation");
			App->physics->AddObject(propagationObj);
		}

		if (App->scene->TargetExists()) {
			trajectory = CalculateTrajectory();
			LOG("Ready to shoot, baby");
			propagationObj->pos.x = aimbot->pos.x;
			propagationObj->pos.y = aimbot->pos.y;
			state = AimbotStates::AIMBOT_CALCULATED_MONTECARLO;
		}

		break;

	case AimbotStates::AIMBOT_CALCULATED_MONTECARLO:

		//Do not log here. It does it every frame
		propagationObj->pos.x = aimbot->pos.x;
		propagationObj->pos.y = aimbot->pos.y;

		break;

	case AimbotStates::AIMBOT_SHOOT:
		
		propagationObj->AddSpeed	(trajectory.speed, trajectory.angle);

		state = AimbotStates::AIMBOT_IDLE;
		break;
	}

	return UPDATE_CONTINUE;
}


void ModuleAimbot::HandleInput() {

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		if (propagationObj != nullptr) {
		
			App->physics->DeleteObject(propagationObj);
			propagationObj = nullptr;
		}
		
		state = AimbotStates::AIMBOT_CALCULATE_MONTECARLO;
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && state == AimbotStates::AIMBOT_CALCULATED_MONTECARLO)
	{
		state = AimbotStates::AIMBOT_SHOOT;
	}
}


bool ModuleAimbot::CleanUp() {

	return true;
}

// Trajectory with Montecarlo method
Trajectory ModuleAimbot::CalculateTrajectory() {

	dPoint auxPos = propagationObj->pos;
	dPoint auxSpeed = propagationObj->speed;
	

	Trajectory result;
	result.angle = 0;
	result.speed = 0;
	Trajectory AuxResult;
	AuxResult.angle = 0;
	AuxResult.speed = 0;

	int TimeCompare = 100;
	int AuxTimeCompare = 100;

	float seedSpeed[MONTECARLO_ITERATION];
	float seedAngle[MONTECARLO_ITERATION];

	srand(time(NULL));

	for (int i = 0; i < MONTECARLO_ITERATION; i++) 
	{
		seedSpeed[i] = 50 + rand() % 50 + 1;	
		seedAngle[i] = 180 + rand() % 180 + 1;

		float seedAngleaux = DEG_TO_RAD(seedAngle[i]);

		propagationObj->speed = { seedSpeed[i] * cos(seedAngleaux), seedSpeed[i] * sin(seedAngleaux) };
		propagationObj->pos = auxPos;

		for (int j = 0; j < PROPAGATION; j++)
		{
			App->physics->Integrate(*propagationObj, GRAVITY, App->dt);

			if (propagationObj->AccurateCheckCollision(App->scene->Target())) 
			{
				AuxResult.angle = seedAngle[i];
				AuxResult.speed = seedSpeed[i];
				AuxTimeCompare = j;

				if (AuxTimeCompare < TimeCompare) {
					result = AuxResult;
					TimeCompare = j;
				}
			}
		}
	}

	propagationObj->pos = auxPos;
	propagationObj->speed = auxSpeed;

	return result;
}
