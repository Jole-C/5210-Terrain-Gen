#include "ParticleSystem.h"
#include "../../MathHelpers.h"
#include "../../../Engine/System/GameState/GameStateManager.h"
#include "../../../Engine/System/Context/SysContextProvider.h"
#include "../../../Engine/System/GameState/GameState.h"
#include "../Camera/CameraController.h"

void ParticleSystem::Initialise()
{

	C_SysContext::Set<ParticleSystem>(this);

}

void ParticleSystem::Update()
{
}

void ParticleSystem::AddParticle(int _x, int _y, sf::Color _colour, int _life, sf::Vector2f _sizeRange, int _sizeIncrease, sf::Vector2f _speedRange, int _speedIncrease)
{

	ParticleEntity* particleEntity = new ParticleEntity();

	int randomSpeed = RandomRangeInteger(_speedRange.x, _speedRange.y);
	int randomSize = RandomRangeInteger(_sizeRange.x, _sizeRange.y);

	particleEntity->SetParams(_x, _y, _colour, _life, randomSpeed, randomSize, _speedIncrease, _sizeIncrease);
	particleEntity->Initialise();
	C_SysContext::Get<GameStateManager>()->GetCurrentGameState()->AddGameObject(particleEntity);

}

void ParticleSystem::Explosion(int _x, int _y, int _amount, int _screenShake)
{

	for(int i = 0; i < _amount; i++)
	{

		AddParticle(_x, _y, sf::Color(177, 62, 83), RandomRangeInteger(10, 15), sf::Vector2f(1, 3), 0, sf::Vector2f(5, 15), 0);
		AddParticle(_x, _y, sf::Color(239, 125, 87), RandomRangeInteger(15, 15), sf::Vector2f(3, 10), 0, sf::Vector2f(5, 15), 0);
		AddParticle(_x, _y, sf::Color(244, 244, 244), RandomRangeInteger(1, 10), sf::Vector2f(5, 17), 0, sf::Vector2f(5, 15), 0);

	}

	//Screenshake
	C_SysContext::Get<CameraController>()->ScreenShake(_screenShake);

}

void ParticleSystem::RemoveAllParticles()
{

	for(ParticleEntity* particle : assignedParticles)
	{

		C_SysContext::Get<GameStateManager>()->GetCurrentGameState()->RemoveGameObject(particle);

	}

}

void ParticleSystem::Destroy()
{

	RemoveAllParticles();
	assignedParticles.clear();
	delete(this);

}


sf::Vector2f ParticleSystem::GetPosition()
{
	return sf::Vector2f();
}

void ParticleSystem::SetPosition(float px, float py)
{
}

void ParticleSystem::SetPosition(sf::Vector2f ppos)
{
}

void ParticleSystem::GetPosition(float& x, float& y)
{
}

GameObject* ParticleSystem::ReturnNewCopy()
{
	return nullptr;
}

void ParticleEntity::Initialise()
{

	//Pick a random sprite
	int chosenSprite = RandomRangeInteger(0, 2);

	switch(chosenSprite)
	{

	case 0:
		sprite.Initialise("sprites/particle1.png", -2);
		break;

	case 1:
		sprite.Initialise("sprites/particle2.png", -2);
		break;

	case 2:
		sprite.Initialise("sprites/particle3.png", -2);
		break;

	default:
		sprite.Initialise("sprites/particle1.png", -2);
		break;

	}

	sprite.setPosition(x, y);
	sprite.setScale(size, size);
	sprite.setColor(colour);

}

void ParticleEntity::SetParams(int _x, int _y, sf::Color _colour, int _life, int _speed, int _size, int _speedIncrease, int _sizeIncrease)
{

	x = _x;
	y = _y;
	colour = _colour;
	life = _life;
	speed = _speed;
	speedIncrease = _speedIncrease;
	size = _size;
	sizeIncrease = _sizeIncrease;
	angle = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 3.14159265359 * 2;

}

void ParticleEntity::Update()
{

	life -= 1;

	if(life <= 0)
	{

		C_SysContext::Get<GameStateManager>()->GetCurrentGameState()->RemoveGameObject(this);

	}

	x += cos(angle) * speed;
	y += sin(angle) * speed;

	if(speed > 0)
	{

		speed += speedIncrease;

	}

	if(size > 0)
	{

		size += sizeIncrease;

	}

	sprite.setScale(size, size);
	sprite.setPosition(x, y);

}

sf::Vector2f ParticleEntity::GetPosition()
{
	return sf::Vector2f();
}

void ParticleEntity::SetPosition(float px, float py)
{
}

void ParticleEntity::SetPosition(sf::Vector2f ppos)
{
}

void ParticleEntity::GetPosition(float& x, float& y)
{
}

GameObject* ParticleEntity::ReturnNewCopy()
{
	return nullptr;
}
