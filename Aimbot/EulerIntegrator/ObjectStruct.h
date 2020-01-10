#include "Globals.h"
#include "p2Point.h"
#include "p2List.h"
#include "Module.h"
#include "p2SString.h"
#include "Application.h"

// Module --------------------------------------
//each object has a name, position, velocity and mass. It also has a force, which is set to 0 at the start of every loop
struct Object {

public:
	dPoint force; //In newtons WARNING, DO NOT ACCES THE VARIABLE WITHOUT EXPLICIT PERMISSION FROM THE PHYSICS GUYS, IF YOU NEED TO ADD A FORCE USE THE FUNCTION .AddForce()
					// Why dont you put it in fucking private you fck morons? Att: Jose :D
	p2SString name;
	dPoint pos; //In meters
	dPoint speed;  //In meters/second
	double radius; //In meters
	double mass; //In kg
	double friction_coefficient;
	bool noPhys; // If true, deactivates movement & speed updates


public:
	//Diferent object constructors
	Object();

	Object(dPoint aPos, double radius, dPoint aSpeed, dPoint aforce, double aMass, double afriction_coefficient, bool noPhys, p2SString aName);



	//Functions to calculate aerodinamic coeficients
	double CalculateAerodinamicCoeficientY();

	double CalculateAerodinamicCoeficientX();

	//Overloaded operator to compare objects
	bool operator==(Object& dt) const;

	//Checks collision w/ other objects
	bool QuickCheckCollision( const Object* c2) const;
	bool AccurateCheckCollision( const Object* c2) const;

	//Funcions to set forces into objects
	void AddForce(dPoint aForce);
	void AddSpeed(float speed, float angle);
	void ResetForces();

	~Object();

};