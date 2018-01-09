#pragma once

#include <nclgl\NCLDebug.h>
#include <ncltech\Scene.h>
#include <ncltech\SceneManager.h>
#include <ncltech\CommonUtils.h>
#include <ncltech\PhysicsEngine.h>
#include "ObjectPlayer.h"

int a = 0;
class Score : public Scene
{
public:
	Score(const std::string& friendly_name)
		: Scene(friendly_name)
	{};
	
	virtual void OnInitializeScene() override
	{
		score = 0;
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


		GameObject* gsphere = CommonUtils::BuildSphereObject(
			"Good Sphere",
			Vector3(7.5f, 2.6f, 0.0f),
			2.5f,
			true,
			0.0f,
			true,
			false,
			Vector4(0.2f, 1.0f, 0.5f, 1.0f));
		this->AddGameObject(gsphere);
		gsphere->Physics()->SetOnCollisionCallback(std::bind(&Score::GoodCallbackFunction, this,
			std::placeholders::_1,
			std::placeholders::_2));

		GameObject* bsphere = CommonUtils::BuildSphereObject(
			"Bad Sphere",
			Vector3(-7.5f, 2.6f, 0.0f),
			2.5f,
			true,
			0.0f,
			true,
			false,
			Vector4(1.0f, 0.2f, 0.5f, 1.0f));
		this->AddGameObject(bsphere);
		bsphere->Physics()->SetOnCollisionCallback(std::bind(&Score::BadCallbackFunction, this,
			std::placeholders::_1,
			std::placeholders::_2));
	}

	virtual void OnUpdateScene(float dt) override
	{
		Scene::OnUpdateScene(dt);

		uint drawFlags = PhysicsEngine::Instance()->GetDebugDrawFlags();

		NCLDebug::AddStatusEntry(Vector4(1.0f, 0.9f, 0.8f, 1.0f), "Hit Green box to increase score by 100");
		NCLDebug::AddStatusEntry(Vector4(1.0f, 0.9f, 0.8f, 1.0f), "Hit Red box to decrease score by 50");
		NCLDebug::AddStatusEntry(Vector4(1.0f, 0.9f, 0.8f, 1.0f), "Current Score: %d", score);

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_J)) {
			++a;
			Vector4 colour = CommonUtils::GenColor(a * 0.2f, 1.0f);
			GameObject* p_Sphere = CommonUtils::BuildSphereObject("", Vector3(GraphicsPipeline::Instance()->GetCamera()->GetPosition()),
				0.5f, true, 1.f, true, true, colour);


			float y = GraphicsPipeline::Instance()->GetCamera()->GetYaw() *PI / 180;
			float p = GraphicsPipeline::Instance()->GetCamera()->GetPitch()*PI / 180;

			p_Sphere->Physics()->SetLinearVelocity(Vector3(-20 * sin(y) * cos(p), 20 * sin(p), -20 * cos(y) * cos(p)));
			p_Sphere->Physics()->SetElasticity(0.2f);
			p_Sphere->Physics()->SetFriction(0.5f);


			SceneManager::Instance()->GetCurrentScene()->AddGameObject(p_Sphere);
			Vector3 position = p_Sphere->Physics()->GetPosition();
		}
		
	}

	bool GoodCallbackFunction(PhysicsNode* self, PhysicsNode* collidingObject)
	{
		score = score + 100;
		NCLDebug::Log(Vector3(0.3f, 1.0f, 0.3f), "Good hit!");
		NCLDebug::Log(Vector3(0.3f, 1.0f, 0.3f), "Current Score: %d", score);
		return true;
	}

	bool BadCallbackFunction(PhysicsNode* self, PhysicsNode* collidingObject)
	{
		score = score - 50;
		NCLDebug::Log(Vector3(0.3f, 1.0f, 0.3f), "Bad hit!");
		NCLDebug::Log(Vector3(0.3f, 1.0f, 0.3f), "Current Score: %d", score);
		return true;
	}
protected:
	int score;
};

