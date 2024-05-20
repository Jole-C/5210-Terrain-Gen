#pragma once

#include <SFML/Graphics/View.hpp>
#include "../../../../Source/Engine/Core/GameObject.h"


enum TARGET_MODE
{

	TARGET_OBJECT,
	TARGET_POSITION

};

class CameraController : public GameObject
{

public:

	void Initialise() override;
	void Update() override;
	void SetTargetMode(TARGET_MODE _targetMode);
	void SetTargetObject(GameObject* _object);
	void SetTargetPosition(sf::Vector2f _position);
	void ScreenShake(double _amount);

	sf::Vector2f GetPosition() override;
	void SetPosition(float px, float py) override;
	void SetPosition(sf::Vector2f ppos) override;
	void GetPosition(float& x, float& y) override;
private:
	sf::View view;
	sf::Vector2f viewPosition;
	sf::Vector2f offsetPosition;

	GameObject* targetObject;
	sf::Vector2f targetPosition;
	TARGET_MODE targetMode;

	double screenshakeAmount;
	double screenshakeFade = 0.9;

};

