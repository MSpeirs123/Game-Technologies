#include "PhysicsNode.h"
#include "PhysicsEngine.h"


void PhysicsNode::IntegrateForVelocity(float dt)
{
	if (invMass > 0.0f)
		linVelocity += PhysicsEngine::Instance()->GetGravity() * dt;
	linvelocityprev = linVelocity;

	linVelocity += force * invMass * dt;

	linVelocity =
		linVelocity * PhysicsEngine::Instance()->GetDampingFactor();

	linvelocitytemp1 = linVelocity + (linVelocity - linvelocityprev) / 2;			//RK-4 integration assuming constant acceleration

	linvelocitytemp2 = linVelocity + (linvelocitytemp1 - linvelocityprev) / 2;

	linvelocitytemp3 = linVelocity + (linvelocitytemp2 - linvelocityprev) / 2;

	linvelocitytemp4 = linVelocity + (linvelocitytemp3 - linvelocityprev);

	angvelocityprev = angVelocity;

	angVelocity += invInertia * torque * dt;

	angVelocity =
		angVelocity * PhysicsEngine::Instance()->GetDampingFactor();

	angvelocitytemp1 = angVelocity + (angVelocity - angvelocityprev) / 2;

	angvelocitytemp2 = angVelocity + (angvelocitytemp1 - angvelocityprev) / 2;

	angvelocitytemp3 = angVelocity + (angvelocitytemp2 - angvelocityprev) / 2;

	angvelocitytemp4 = angVelocity + (angvelocitytemp3 - angvelocityprev);
}

/* Between these two functions the physics engine will solve for velocity
based on collisions/constraints etc. So we need to integrate velocity, solve
constraints, then use final velocity to update position.
*/

void PhysicsNode::IntegrateForPosition(float dt)
{
	position += (linVelocity + (linvelocitytemp1 + linvelocitytemp4) / 6 + (linvelocitytemp2 + linvelocitytemp3) / 3) * dt;

	orientation = orientation +
		Quaternion((angvelocitytemp1 * dt * 0.5f + angvelocitytemp4 * dt * 0.5f) / 6 + (angvelocitytemp2 * dt * 0.5f + angvelocitytemp3 * dt * 0.5f) / 3, 0.0f) * orientation;

	orientation.Normalise();

	//Finally: Notify any listener's that this PhysicsNode has a new world transform.
	// - This is used by GameObject to set the worldTransform of any RenderNode's. 
	//   Please don't delete this!!!!!
	FireOnUpdateCallback();
}