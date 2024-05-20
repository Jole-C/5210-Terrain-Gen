#pragma once

#include "../../../Engine/Core/GameObject.h"
#include <SFML/Graphics/Color.hpp>
#include "../../../Engine/Renderer/Sprite.h"


class ParticleEntity;

class ParticleSystem : public GameObject
{

public:

	void Initialise() override;
	void Update() override;
	void AddParticle(int _x, int _y, sf::Color _colour, int _life, sf::Vector2f _sizeRange, int _sizeIncrease, sf::Vector2f _speedRange, int _speedIncrease);
	void Explosion(int _x, int _y, int _amount, int _screenShake = 5);
	void RemoveAllParticles();
	void Destroy();

	std::vector<ParticleEntity*> assignedParticles; //The particles assigned to this particle system


	sf::Vector2f GetPosition() override;
	void SetPosition(float px, float py) override;
	void SetPosition(sf::Vector2f ppos) override;
	void GetPosition(float& x, float& y) override;
	GameObject* ReturnNewCopy() override;

private:

};

class ParticleEntity : public GameObject
{
public:

	void Update() override;

private:

	void Initialise() override;
	void SetParams(int _x, int _y, sf::Color _colour, int _life, int _speed, int _size, int _speedIncrease, int _sizeIncrease);
	sf::Vector2f GetPosition() override;
	void SetPosition(float px, float py) override;
	void SetPosition(sf::Vector2f ppos) override;
	void GetPosition(float& x, float& y) override;
	GameObject* ReturnNewCopy() override;

	int x;
	int y;
	sf::Color colour;
	int life;
	int speed;
	int speedIncrease;
	int size;
	int sizeIncrease;
	double angle;
	Sprite sprite;

	friend class ParticleSystem;

};