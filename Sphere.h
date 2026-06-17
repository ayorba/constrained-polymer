#ifndef SPHERE_H
#define SPHERE_H

#include <iostream>
#include <cmath>
#include <numeric>
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
		Sphere(int resid, int atomid, double diam, double Mass, Vector3D& pos) : residueID{resid}, atomID{atomid}, diameter{diam}, mass{Mass}, position{pos}, velocity{{0,0,0}}, force{{0,0,0}}, velocity_old{{0,0,0}}, force_old{{0,0,0}}, displacement{{0,0,0}} {} 
		Sphere(int resid, int atomid, double diam, double Mass, Vector3D& pos, Vector3D& vel, Vector3D& force_) : residueID{resid}, atomID{atomid}, diameter{diam}, mass{Mass}, position{pos}, velocity{vel}, force{force_}, velocity_old{{0,0,0}}, force_old{force_}, displacement{{0,0,0}} {} 



		void setPosition(const Vector3D& newposition)
		{
			this->position = newposition;
		}
		void setPosition(double posx, double posy, double posz)
		{
			this->position.x = posx;
			this->position.y = posy;
			this->position.z = posz;
		}

		void setVelocity(const Vector3D& newvelocity)
		{
			this->velocity = newvelocity;
		}
		void setVelocity(double velx, double vely, double velz)
		{
			this->velocity.x = velx;
			this->velocity.y = vely;
			this->velocity.z = velz;
		}

		void setForce(const Vector3D& newforce)
		{
			this->force = newforce;
		}

		void setForce(double forcex, double forcey, double forcez)
		{
			this->force.x += forcex;
			this->force.y += forcey;
			this->force.z += forcez;
		}


		void addPosition(const Vector3D& disp)
		{
			this->position += disp;
			this->displacement += disp;
		}
		void addPosition(double dispx, double dispy, double dispz)
		{
			this->position.x += dispx;
			this->position.y += dispy;
			this->position.z += dispz;

			this->displacement.x += dispx;
			this->displacement.y += dispy;
			this->displacement.z += dispz;
		}


		void addVelocity(const Vector3D& newvel)
		{
			this->velocity += newvel;
		}
		void addVelocity(double velx, double vely, double velz)
		{
			this->velocity.x += velx;
			this->velocity.y += vely;
			this->velocity.z += velz;
		}



		void applyForce(const Vector3D& newforce)
		{
			this->force += newforce;
		}
		void applyForce(double newforcex, double newforcey, double newforcez)
		{
			this->force.x += newforcex;
			this->force.y += newforcey;
			this->force.z += newforcez;
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
		Vector3D vecij(const std::shared_ptr<Sphere>& sphb, bool unit) const
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
		inline double distij(const std::shared_ptr<Sphere>& sph, bool squared=0) const
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


