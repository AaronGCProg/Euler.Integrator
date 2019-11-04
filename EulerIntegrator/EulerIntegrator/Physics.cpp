#include "Physics.h"
/*
float Integrate(Object &object, int time) {

	float pos;
	float speed;

	speed = object.acceleration * time + object.speed;

	pos = speed * time + object.x;

	return pos;
}
*/
//we work in game units by frame



void Integrate2(Object &object, double force) {
	double acc;
	acc = force * (1 / object.mass);
	object.speed += acc; //60 fps, one iteration
	object.x += object.speed;

	cout<<(object.x);
	cout << "\n"; cout << "\n";
}
