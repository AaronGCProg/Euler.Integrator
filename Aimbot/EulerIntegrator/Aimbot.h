#include "Globals.h"
#include "p2Point.h"
#include "p2List.h"
#include "Module.h"


struct Object;

struct Trajectory
{
	float speed;
	float angle;
};

enum class AimbotStates {
	AIMBOT_IDLE,
	AIMBOT_CALCULATE_MONTECARLO,
	AIMBOT_SHOOT,
	AIMBOT_TARGET_IMPACT,
	AIMBOT_RESET
};

// Module --------------------------------------
class ModuleAimbot : public Module
{
public:
	ModuleAimbot(Application* app, bool start_enabled = true);
	~ModuleAimbot();

	bool Start();
	update_status Update();
	bool CleanUp();

private:
	void HandleInput();
	Trajectory CalculateTrajectory(float speed, float angle);

private:
	AimbotStates state;
	Object* aimbot;
	Object* propagationObj;
};
