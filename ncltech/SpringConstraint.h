#pragma once

#include "Constraint.h"
#include "PhysicsEngine.h"
#include <nclgl\NCLDebug.h>


class SpringConstraint : public Constraint
{
public:
	SpringConstraint(PhysicsNode* obj1, PhysicsNode* obj2,
		const Vector3& globalOnA, const Vector3& globalOnB)
	{
		pnodeA = obj1;
		pnodeB = obj2;

		//Set the preferred distance of the constraint to enforce 
		// (ONLY USED FOR BAUMGARTE)
		// - Ideally we only ever work at the velocity level, so satifying (velA-VelB = 0)
		//   is enough to ensure the distance never changes.
		Vector3 ab = globalOnB - globalOnA;
		targetLength = ab.Length();

		//Get the local points (un-rotated) on the two objects where the constraint should
		// be attached relative to the objects center. So when we rotate the objects
		// the constraint attachment point will rotate with it.
		Vector3 r1 = (globalOnA - pnodeA->GetPosition());
		Vector3 r2 = (globalOnB - pnodeB->GetPosition());
		relPosA = Matrix3::Transpose(pnodeA->GetOrientation().ToMatrix3()) * r1;
		relPosB = Matrix3::Transpose(pnodeB->GetOrientation().ToMatrix3()) * r2;
	}

	virtual void ApplyImpulse() {}


	//Solves the constraint and applies a velocity impulse to the two
	// objects in order to satisfy the constraint.
	virtual void PreSolverStep(float dt) override
	{
		Vector3 r1 = pnodeA->GetOrientation().ToMatrix3() * relPosA;
		Vector3 r2 = pnodeB->GetOrientation().ToMatrix3() * relPosB;

		Vector3 globalOnA = r1 + pnodeA->GetPosition();
		Vector3 globalOnB = r2 + pnodeB->GetPosition();

		Vector3 ab = globalOnB - globalOnA;
		Vector3 abn = ab;

		abn.Normalise();

		Vector3 v0 = pnodeA->GetLinearVelocity()
			+ Vector3::Cross(pnodeA->GetAngularVelocity(), r1);

		Vector3 v1 = pnodeB->GetLinearVelocity()
			+ Vector3::Cross(pnodeB->GetAngularVelocity(), r2);

		float abnVel = Vector3::Dot(v0 - v1, abn);

		float invConstraintMassLin = pnodeA->GetInverseMass()
			+ pnodeB->GetInverseMass();

		float invConstraintMassRot = Vector3::Dot(abn,
			Vector3::Cross(pnodeA->GetInverseInertia()
				* Vector3::Cross(r1, abn), r1)
			+ Vector3::Cross(pnodeB->GetInverseInertia()
				* Vector3::Cross(r2, abn), r2));

		float constraintMass = invConstraintMassLin + invConstraintMassRot;

		if (constraintMass > 0.0f) {
			float b = 0.0f;

			float distance_offset = ab.Length() - targetLength;
			float baumgarte_scalar = 0.1f;
			b = -(baumgarte_scalar
				/ PhysicsEngine::Instance()->GetDeltaTime())
				* distance_offset;

			float jn = -(abnVel + b) / constraintMass;

			float force = 112.f * distance_offset - 0.5 * abnVel;
			float vel = force*dt;

			pnodeA->SetLinearVelocity(pnodeA->GetLinearVelocity()
				+ abn * (pnodeA->GetInverseMass() * vel
					));

			pnodeB->SetLinearVelocity(pnodeB->GetLinearVelocity()
				- abn * (pnodeB->GetInverseMass() * vel));

			pnodeA->SetAngularVelocity(pnodeA->GetAngularVelocity()
				+ pnodeA->GetInverseInertia()
				* Vector3::Cross(r1, abn * vel));

			pnodeB->SetAngularVelocity(pnodeB->GetAngularVelocity()
				- pnodeB->GetInverseInertia()
				* Vector3::Cross(r2, abn * vel));
		}
	}

	//Draw the constraint visually to the screen for debugging
	virtual void DebugDraw() const
	{
		Vector3 globalOnA = pnodeA->GetOrientation().ToMatrix3() * relPosA + pnodeA->GetPosition();
		Vector3 globalOnB = pnodeB->GetOrientation().ToMatrix3() * relPosB + pnodeB->GetPosition();

		NCLDebug::DrawThickLine(globalOnA, globalOnB, 0.02f, Vector4(0.0f, 0.0f, 0.0f, 1.0f));
		NCLDebug::DrawPointNDT(globalOnA, 0.05f, Vector4(1.0f, 0.8f, 1.0f, 1.0f));
		NCLDebug::DrawPointNDT(globalOnB, 0.05f, Vector4(1.0f, 0.8f, 1.0f, 1.0f));
	}

protected:
	PhysicsNode *pnodeA, *pnodeB;

	float   targetLength;

	Vector3 relPosA;
	Vector3 relPosB;
};