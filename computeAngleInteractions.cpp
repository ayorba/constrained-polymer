#include <memory>
#include <unordered_map>
#include "InteractionManager.h"
#include "Sphere.h"
#include "Residue.h"
#include "Bond.h"
#include "Vector3D.h"


void InteractionManager::computeAngleInteractions()
{

	double fixed_angle, angle, cos;
	double magij, magkj;


	// Loop over triples of residues
	for(auto& bond : sim->bond_angles)
	{
		// Recall fixed bond angle
		fixed_angle = bond.fixed_angle;

		// Get backbone atoms from residues
		auto iti = bond.sphereMap.find(bond.sphereIDs[0]);
		auto itj = bond.sphereMap.find(bond.sphereIDs[1]);
		auto itk = bond.sphereMap.find(bond.sphereIDs[2]);
		
		if(iti != bond.sphereMap.end() && itj != bond.sphereMap.end() && itk != bond.sphereMap.end())
		{
			const auto& parti = iti->second;
			const auto& partj = itj->second;
			const auto& partk = itk->second;


			// Get the unit vectors and current angle
			Vector3D rij;
			Vector3D rkj;
			Vector3D rperp;

			rij = parti->position - partj->position;
			rkj = partk->position - partj->position;

			magij = rij.norm();
			magkj = rkj.norm();

			rij.normalize();
			rkj.normalize();


			// Make a vector perpendicular to rij and rkj
			rperp = rkj.cross(rij);
			rperp.normalize();


			Vector3D rij_cross_rperp = rij.cross(rperp);
			Vector3D rperp_cross_rkj = rperp.cross(rkj);

			cos = rij.dot(rkj);

			// Making sure cos stays between -1.0 and 1.0
			// like using cos = min(max(cos,-1.0),1.0)
			// Only a problem when the angle is near pi
			//cos = ((cos > -1.0) ? cos : -1.0) < 1.0 ? cos : 1.0;

			angle = acos(cos);

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


			// Add potential energy from bond angle constraint
			this->PE += 0.5 * bond.stiffness * pow(bond.current_angle - bond.fixed_angle, 2.0);


			// Find the force on each particle
			double mult = (bond.stiffness)*(angle - fixed_angle); 


			// Force on parti
			parti->applyForce(mult*(rij_cross_rperp / magij));

			// Force on partj
			partj->applyForce(-1.0 * mult * ( (rij_cross_rperp/magij) + (rperp_cross_rkj/magkj) ));

			// Force on partk
			partk->applyForce(mult*(rperp_cross_rkj / magkj));


		}

	}


}


