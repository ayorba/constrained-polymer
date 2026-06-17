#ifndef SPHERE_H
#define SPHERE_H

#include <iostream>
#include <cmath>
#include "Vector3D.h"


class Sphere
{

	public:
		
		int residueID;
		int atomID;

		double diameter;
		double mass;


		Vector3D position;
		Vector3D velocity;
		Vector3D force;
		Vector3D velocity_old;
		Vector3D force_old;
		Vector3D displacement;

	
		// Constructor
		Sphere(int resid, int atomid, double diam, double Mass, Vector3D& pos) : residueID{resid}, atomID{atomid}, diameter{diam}, mass{Mass}, position{pos}, velocity{}, force{}, velocity_old{}, force_old{}, displacement{} {}
		Sphere(int resid, int atomid, double diam, double Mass, Vector3D& pos, Vector3D& vel, Vector3D& force_) : residueID{resid}, atomID{atomid}, diameter{diam}, mass{Mass}, position{pos}, velocity{vel}, force{force_}, velocity_old{}, force_old{force_}, displacement{} {} 


		void setVelocity(double velx, double vely, double velz)
		{
			this->velocity.x = velx;
			this->velocity.y = vely;
			this->velocity.z = velz;
		}

		void applyForce(const Vector3D& newforce)
		{
			this->force += newforce;
		}


		Vector3D vecij(Sphere* sphb, bool unit) const
		{
			Vector3D vec = sphb->position - this->position;

			if(unit)
			{
				double mag = vec.norm();

				vec /= mag;
			}

			return vec;
		}


		double distij(Sphere* sph, bool squared=0) const
		{
			Vector3D vec = this->position - sph->position;

			return squared ? vec.dot(vec) : vec.norm();
		}
		double distij(Vector3D& ptb, bool squared=0) const
		{
			Vector3D vec = this->position - ptb;

			return squared ? vec.dot(vec) : vec.norm();
		}


		void print() const
		{
			std::cout<<"Residue ID: "<<this->residueID<<"\n";
			std::cout<<"atom ID: "<<this->atomID<<"\n";
			std::cout<<"diameter: "<<this->diameter<<"\n";
			std::cout<<"position: ("<<this->position.x<<", "<<this->position.y<<", "<<this->position.z<<")\n";
			std::cout<<"velocity: ("<<this->velocity.x<<", "<<this->velocity.y<<", "<<this->velocity.z<<")\n";
			std::cout<<"force: ("<<this->force.x<<", "<<this->force.y<<", "<<this->force.z<<")\n";

			std::cout<<"\n";
		}

};


#endif


