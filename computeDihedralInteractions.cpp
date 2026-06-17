#include <memory>
#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <unordered_map>
#include "InteractionManager.h"
#include "Sphere.h"
#include "Residue.h"
#include "Bond.h"
#include "Vector3D.h"


void InteractionManager::computeDihedralInteractions()
{


	// coefficients for dihedral potential from O'Hern et al. paper 'Calibrated 
	// Langevin-Dynamics simulations of intrinsically disordered proteins' Eq.4
	//**************************

	std::vector<double> A = {0.0, 0.705, -0.313, -0.079, 0.041};
	std::vector<double> B = {0.0, -0.175, -0.093, 0.03, 0.03};

	//**************************

	double angle, cos;
	double magt, magu;
	bool angle_neg;
	double dV_dphi=0.0;

	Vector3D rij;
	Vector3D rkj;
	Vector3D rlk;
	Vector3D rkj_unit;

	Vector3D t;
	Vector3D u;
	Vector3D tcrossu;
	Vector3D gradphi_t;
	Vector3D gradphi_u;

	Vector3D newforce;


	// Loop over triples of residues
	for(auto& bond : sim->bond_dihedrals)
	{

		// Collect pointers to sphere objects for spheres included in dihedral angle
		auto iti = bond.sphereMap.find(bond.sphereIDs[0]);
		auto itj = bond.sphereMap.find(bond.sphereIDs[1]);
		auto itk = bond.sphereMap.find(bond.sphereIDs[2]);
		auto itl = bond.sphereMap.find(bond.sphereIDs[3]);
		
		if(iti != bond.sphereMap.end() && itj != bond.sphereMap.end() && itk != bond.sphereMap.end() && itl != bond.sphereMap.end())
		{
			auto* parti = iti->second.get();
			auto* partj = itj->second.get();
			auto* partk = itk->second.get();
			auto* partl = itl->second.get();


			// Get the unit vectors and current angle
			rij = parti->position - partj->position;
			rkj = partk->position - partj->position;
			rlk = partl->position - partk->position;


			// t = rkj.cross(rij);
			t = rij.cross(rkj);
			u = rlk.cross(rkj);
			
			tcrossu = t.cross(u);

			magt = t.norm();
			magu = u.norm();

			// Make unit vectors
			rkj_unit = rkj.normalized();
			t.normalize();
			u.normalize();

			gradphi_t = (t.cross(rkj_unit)) / magt;
			gradphi_u = -(u.cross(rkj_unit)) / magu;


			// current angle
			cos = t.dot(u);

			// Making sure cos stays between -1.0 and 1.0
			// like using cos = min(max(cos,-1.0),1.0)
			// Only a problem when the angle is near pi
			//cos = ((cos > -1.0) ? cos : -1.0) < 1.0 ? cos : 1.0;

			angle = std::fabs(acos(cos));
			angle_neg = std::signbit(rkj.dot(tcrossu));

			if(angle_neg)
			{
				angle *= -1.0;
			}

			if(fabs(cos - 1.0) < 1e-15)
			{
				angle = 0.0;
			}
			else if(fabs(cos + 1.0) < 1e-15)
			{
				angle = M_PI;
			}


			// update the current angle
			bond.current_angle = angle;
			

			// Dihedral potential from O'Hern et al. paper 'Calibrated 
			// Langevin-Dynamics simulations of intrinsically disordered proteins' Eq.4
			//**************************

			dV_dphi = 0.0;
			for(int el=1; el<5; ++el)
			{
				dV_dphi += -el*A[el]*std::sin(el*angle) + el*B[el]*std::cos(el*angle);

				// Add potential energy from dihedral constraint
				this->PE += bond.stiffness * ( A[el]*std::cos(el*angle) + B[el]*std::sin(el*angle) );
			}

			dV_dphi *= bond.stiffness;
			
			//**************************


			// Force on parti
			parti->force += dV_dphi * (gradphi_t.cross(rkj));

			// Force on partj
			partj->force += dV_dphi * ( (gradphi_t.cross(rij - rkj)) + (gradphi_u.cross(rlk)) );

			// Force on partk
			partk->force += -dV_dphi * ( (gradphi_u.cross(rlk + rkj)) + (gradphi_t.cross(rij)) );

			// Force on partl
			partl->force += dV_dphi * (gradphi_u.cross(rkj));	

		}

	}


}


