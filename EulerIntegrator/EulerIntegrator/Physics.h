#include "Globals.h"
#include "p2Point.h"
#include "p2List.h"
#include "Module.h"
#include "p2SString.h"
#include "Application.h"
// Module --------------------------------------
//each object has a name, position, velocity and mass. It also has a force, which is set to 0 at the start of every loop
struct Object {

	p2SString name;
	dPoint pos;
	dPoint speed;
	dPoint force;
	double width;
	double height;
	double mass;
	double friction_coefficient;

	//Collision Control
	movement_type type; //In which "collisions state" is the object located
	collision current_collision;

	Object() {

		pos = { 0, 0 };
		speed = { 0, 0 };
		force = { 0, 0 };
		mass = 1;
		name = "";
		friction_coefficient = 0.5;

		type = COLL_DYNAMIC;
	}

	Object(dPoint aPos, dPoint aSpeed, dPoint aforce, double aMass, double afriction_coefficient, p2SString aName) {

		pos = aPos;
		speed = aSpeed;
		force = aforce;
		mass = aMass;
		name = aName;
		friction_coefficient = afriction_coefficient;

		type = COLL_DYNAMIC;
	}

	Object(dPoint aPos, dPoint aSpeed, dPoint aforce, double aMass, double afriction_coefficient, p2SString aName, collision_type cat, collision_type Mask) {

		pos = aPos;
		speed = aSpeed;
		force = aforce;
		mass = aMass;
		friction_coefficient = afriction_coefficient;
		name = aName;

		type = cat;
	}

	bool CheckCollisionRect(Object& obj);


	~Object() {};

};

struct World
{
	dPoint gravity;

	p2SString name;

	p2List<Object*>* objects_list;

	World()
	{
		name = "";
		gravity = { 0,9.81f };
		objects_list = new p2List<Object*>;
	}

	World(dPoint aGravity, p2SString aName)
	{
		gravity = aGravity;
		name = aName;
		objects_list = new p2List<Object*>;

	}
	~World() {}
};

class ModulePhysics : public Module
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Start();
	update_status Update();
	bool CleanUp();
	void Integrate(Object &object, dPoint gravity);

	//Temp var to test collisions
	World* world;

private:

	
};












