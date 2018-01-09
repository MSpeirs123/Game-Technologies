#pragma once

#include <nclgl\NCLDebug.h>
#include <ncltech\Scene.h>
#include <ncltech\SceneManager.h>
#include <ncltech\CommonUtils.h>
#include <ncltech\PhysicsEngine.h>
#include "../ncltech/SpringConstraint.h"

int y = 0;
class Soft_body : public Scene
{
public:
	Soft_body(const std::string& friendly_name)
		: Scene(friendly_name)
	{

	};

	virtual void OnInitializeScene() override
	{
		//Create Ground
		this->AddGameObject(CommonUtils::BuildCuboidObject(
			"Ground",
			Vector3(0.0f, -1.0f, 0.0f),
			Vector3(20.0f, 1.0f, 20.0f),
			true,
			0.0f,
			true,
			false,
			Vector4(0.2f, 0.5f, 1.0f, 1.0f)));


		{
			GameObject* softbody[100];
			for (int i = 0; i < 20; ++i)
			{
				for (int j = 0; j < 5; ++j)
				{
					if (i == 19) {
						Vector4 color = Vector4(i * 0.025f, 0.7f, (2 - i) * 0.25f, 1.0f);
						GameObject* sphere = CommonUtils::BuildSphereObject(
							"",
							Vector3(-1.5 + j, 25, -i),
							0.4,
							true,
							0.f,
							true,
							true,
							color);
						sphere->Physics()->SetElasticity(0.0f);
						sphere->Physics()->SetFriction(0.5f);
						softbody[i * 5 + j] = sphere;
						//Initial push??
						//cube->Physics()->SetLinearVelocity(Quaternion::AxisAngleToQuaterion(Vector3(0.0f, 0.0f, 1.0f), 20.0f).ToMatrix3() * Vector3(-1.f, 0.f, 0.f));
						this->AddGameObject(sphere);
					}
					else {
						Vector4 color = Vector4(i * 0.025f, 0.7f, (2 - i) * 0.25f, 1.0f);
						GameObject* sphere = CommonUtils::BuildSphereObject(
							"",
							Vector3(-1.5 + j, 25, -i),
							0.4,
							true,
							1.f,
							true,
							true,
							color);
						sphere->Physics()->SetElasticity(0.0f);
						sphere->Physics()->SetFriction(0.5f);
						softbody[i * 5 + j] = sphere;
						this->AddGameObject(sphere);
					}
				}
			}
			SpringConstraint* constraintr[100];			//spring constraint to ball to the right
			SpringConstraint* constraintu[100];			//spring constrant to ball above
			SpringConstraint* constraintdur[100];		//spring constraint to upper right diagonal
			SpringConstraint* constraintdul[100];		//spring constraint to upper left diagonal


														//A bit of a mess to ensure funky things don't happen at edges, sorry :(
			for (int i = 0; i < 19; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					constraintr[i * 5 + j] = new SpringConstraint(
						softbody[i * 5 + j]->Physics(),					//Physics Object A
						softbody[i * 5 + j + 1]->Physics(),					//Physics Object B
						softbody[i * 5 + j]->Physics()->GetPosition(),	//Attachment Position on Object A	-> Currently the centre
						softbody[i * 5 + j + 1]->Physics()->GetPosition());	//Attachment Position on Object B	-> Currently the centre  
					PhysicsEngine::Instance()->AddConstraint(constraintr[i * 5 + j]);

					constraintr[19 * 5 + j] = new SpringConstraint(
						softbody[19 * 5 + j]->Physics(),					//Physics Object A
						softbody[19 * 5 + j + 1]->Physics(),					//Physics Object B
						softbody[19 * 5 + j]->Physics()->GetPosition(),	//Attachment Position on Object A	-> Currently the centre
						softbody[19 * 5 + j + 1]->Physics()->GetPosition());	//Attachment Position on Object B	-> Currently the centre  
					PhysicsEngine::Instance()->AddConstraint(constraintr[19 * 5 + j]);

					constraintu[i * 5 + j] = new SpringConstraint(
						softbody[i * 5 + j]->Physics(),					//Physics Object A
						softbody[i * 5 + j + 5]->Physics(),					//Physics Object B
						softbody[i * 5 + j]->Physics()->GetPosition(),	//Attachment Position on Object A	-> Currently the centre
						softbody[i * 5 + j + 5]->Physics()->GetPosition());	//Attachment Position on Object B	-> Currently the centre  
					PhysicsEngine::Instance()->AddConstraint(constraintu[i * 5 + j]);

					constraintu[i * 5 + 5] = new SpringConstraint(
						softbody[5 * i + 4]->Physics(),					//Physics Object A
						softbody[5 * i + 9]->Physics(),					//Physics Object B
						softbody[5 * i + 4]->Physics()->GetPosition(),	//Attachment Position on Object A	-> Currently the centre
						softbody[5 * i + 9]->Physics()->GetPosition());	//Attachment Position on Object B	-> Currently the centre  
					PhysicsEngine::Instance()->AddConstraint(constraintu[i * 5 + 5]);

					constraintdur[i * 5 + j] = new SpringConstraint(
						softbody[i * 5 + j]->Physics(),					//Physics Object A
						softbody[i * 5 + j + 6]->Physics(),					//Physics Object B
						softbody[i * 5 + j]->Physics()->GetPosition(),	//Attachment Position on Object A	-> Currently the centre
						softbody[i * 5 + j + 6]->Physics()->GetPosition());	//Attachment Position on Object B	-> Currently the centre  
					PhysicsEngine::Instance()->AddConstraint(constraintdur[i * 5 + j]);

					constraintdul[i * 5 + j] = new SpringConstraint(
						softbody[i * 5 + j + 1]->Physics(),					//Physics Object A
						softbody[i * 5 + j + 5]->Physics(),					//Physics Object B
						softbody[i * 5 + j + 1]->Physics()->GetPosition(),	//Attachment Position on Object A	-> Currently the centre
						softbody[i * 5 + j + 5]->Physics()->GetPosition());	//Attachment Position on Object B	-> Currently the centre  
					PhysicsEngine::Instance()->AddConstraint(constraintdul[i * 5 + j]);
				}
			}

		}
	}

	virtual void OnUpdateScene(float dt) override
	{
		Scene::OnUpdateScene(dt);

		uint drawFlags = PhysicsEngine::Instance()->GetDebugDrawFlags();


		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_J)) {
			++y;
			Vector4 colour = CommonUtils::GenColor(y * 0.2f, 1.0f);
			GameObject* p_Sphere = CommonUtils::BuildSphereObject("", Vector3(GraphicsPipeline::Instance()->GetCamera()->GetPosition()),
				0.5f, true, 1.f, true, true, colour);


			float y = GraphicsPipeline::Instance()->GetCamera()->GetYaw() *PI / 180;
			float p = GraphicsPipeline::Instance()->GetCamera()->GetPitch()*PI / 180;

			p_Sphere->Physics()->SetLinearVelocity(Vector3(-20 * sin(y) * cos(p), 20 * sin(p), -20 * cos(y) * cos(p)));
			p_Sphere->Physics()->SetElasticity(0.2f);
			p_Sphere->Physics()->SetFriction(0.5f);


			SceneManager::Instance()->GetCurrentScene()->AddGameObject(p_Sphere);
		}
	}
};

