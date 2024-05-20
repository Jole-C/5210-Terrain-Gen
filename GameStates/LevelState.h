#pragma once

#include "../../Engine/System/GameState/GameState.h"
#include "../GameObjects/TerrainGenerator/TerrainGenerator.h"
#include "..\GameObjects\Player\Player.h"
#include "..\GameObjects\UI\Transition.h"
#include "../GameObjects/Camera/CameraController.h"
#include "..\GameObjects\UI\HUD_Overlay.h"
#include "../GameObjects/ParticleSystem/ParticleSystem.h"
//Base level class for all levels
class LevelState : public GameState
{

public:
	void InputGeneratorParams(float _birthLimit, float _deathLimit, int _initialChance, int _numberOfSteps, sf::Vector2f _size);
	virtual void Initialise() override;
	void GenerateMap();
	virtual void Update() override;
	GameObject* GetPlayer() override;
	void MovePlayerToNewState(Player* _newPlayer);

	void RegisterEnemyKilled();

	Transition* transition;
	Player* player;

protected:

	sf::Texture tileMap;
	const char* xmlPath;
	Map* map;
	CameraController* camera;
	ParticleSystem* particleSystem;

	//Enemy tracking
	int totalEnemies = 10; //We set this to 10 so that if someone forgets to initialise it, the level doesn't instantly end
	int totalHEnemies = 5;
	int enemiesKilled = 0;

private:

	//Map and generator variables
	TerrainGenerator terrainGenerator;
	std::vector<sf::VertexArray> visualMap;
	std::vector<std::vector<Tile>> dataMap;

	//Generator params
	float birthLimit;
	float deathLimit;
	int initialChance;
	int numberOfSteps;
	sf::Vector2f size;

	//UI
	HUD_Overlay* HUD;
	WindowManager* pWindowManager = C_SysContext::Get<WindowManager>();
};

