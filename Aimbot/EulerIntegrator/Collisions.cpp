#include "Collisions.h"
#include"Application.h"
#include"Physics.h"
#include "Globals.h"
#include "p2Defs.h"
#include "p2Point.h"

ModuleCollisions::ModuleCollisions(Application* app, bool start_enabled) : Module(app, start_enabled) 
{
	LOG("Contructor of Module Physics has been called");

	collBewtweenObjectsActive = true;
}

// Destructor
ModuleCollisions::~ModuleCollisions() 
{
	LOG("Destructor of Module Physics has been called");
}

void ModuleCollisions::OnCollision(Object& object) 
{
	Object* c1;

	if(collBewtweenObjectsActive)
	for (int i = 0; i < MAX_OBJECTS && App->physics->world->objects_array[i] != NULL; i++) {
		c1 = App->physics->world->objects_array[i];

		if (&object == c1) continue;

		if (c1->CheckCollision(object))
		{
			ResolveCollision(c1, &object);
		}
	}

	if ((object.pos.x + object.radius) > PIXEL_TO_METERS(SCREEN_WIDTH)  || (object.pos.x - object.radius) < 0)
	{
		object.speed.x = -object.speed.x * object.friction_coefficient;

		if ((object.pos.x + object.radius) > PIXEL_TO_METERS(SCREEN_WIDTH))
			object.pos.x = PIXEL_TO_METERS(SCREEN_WIDTH) - object.radius;

		else if((object.pos.x - object.radius) < 0)
			object.pos.x = object.radius;

	}

	if ( (object.pos.y + object.radius) > PIXEL_TO_METERS(SCREEN_HEIGHT) || (object.pos.y-object.radius) < 0)
	{
		object.speed.y = -object.speed.y * object.friction_coefficient;

		if ((object.pos.y + object.radius) > PIXEL_TO_METERS(SCREEN_HEIGHT))
			object.pos.y = PIXEL_TO_METERS(SCREEN_HEIGHT) - object.radius;

		else if (object.pos.y - object.radius < 0)
			object.pos.y = object.radius;
	}
}

bool ModuleCollisions::CleanUp()
{
	LOG("Collisions CleanUp has been called");
	return true;
}

void ModuleCollisions::ResolveCollision(Object* c1, Object* c2)
{
	//Calculates the normal dir
	double modul;
	dPoint normaldir = c2->speed + c1->speed;
	
	if (normaldir.x <= 0.001f && normaldir.y <= 0.001f)
		normaldir = { 0,-1 };//default normal

	normaldir.Normalize();

	//Calculate relative velocity
	dPoint rv = c2->speed - c1->speed;

	// Calculate relative velocity in terms of the normal direction
	double velAlongNormal = rv.Dot(normaldir);//dot product

	// Do not resolve if velocities are separating
	if (velAlongNormal > 0)
	{
		return;
	}
	else
	{
		dPoint p1 = c1->pos + c1->radius;
		dPoint p2 = c2->pos + c2->radius;
		dPoint vel1 = c1->speed;
		dPoint vel2 = c1->speed;
		double m_vel1=vel1.Module();
		double m_vel2=vel2.Module();

		dPoint centersdir= p2 - p1;
		centersdir.Normalize();
		
		vel1 = centersdir.GetInverse() * m_vel1 * c1->friction_coefficient;
		vel2 = centersdir * m_vel2 * c2->friction_coefficient;

		c1->speed = vel1;
		c2->speed = vel2;
	}

}

void ModuleCollisions::ChangeCollBetweenObj()
{
	collBewtweenObjectsActive = !collBewtweenObjectsActive;
}