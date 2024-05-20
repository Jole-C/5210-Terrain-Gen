#include "LevelState.h"
#include <iostream>
void LevelState::InputGeneratorParams(float _birthLimit, float _deathLimit, int _initialChance, int _numberOfSteps, sf::Vector2f _size)
{

	birthLimit = _birthLimit;
	deathLimit = _deathLimit;
	initialChance = _initialChance;
	numberOfSteps = _numberOfSteps;
	size = _size;

}

//Call this after the child's Initialise to initialise level params, player, transition, etc
void LevelState::Initialise()
{

	particleSystem = new ParticleSystem();
	particleSystem->Initialise();
	m_GameObjects.push_back(particleSystem);

	/*transition = new Transition();
	transition->Initialise();
	m_GameObjects.push_back(transition);*/

	camera = new CameraController();
	camera->Initialise();
	C_SysContext::Set<CameraController>(camera);
	m_GameObjects.push_back(camera);

	camera = C_SysContext::Get<CameraController>();

	HUD = new HUD_Overlay;
	HUD->Initialise();
	HUD->referencePlayer = player;
	m_GameObjects.push_back(HUD);
	//HUD->SetPosition(camera->GetPosition());

}

void LevelState::GenerateMap()
{

	//Initialise the terrain generator
	terrainGenerator.Initialise(birthLimit, deathLimit, initialChance, numberOfSteps, size);

	//Generate the map
	map = new Map();

	dataMap = terrainGenerator.GenerateDataMap();

	//Initialise the map object
	map->Initialise();

	map->SetDataMap(dataMap);
	map->GenerateVisualMap(xmlPath, tileMap);

	map->SetPosition(0, 0);
	m_GameObjects.push_back(map);

	//Add the map to the context provider
	C_SysContext::Set<Map>(map);

	//Get the map from the context provider
	map = C_SysContext::Get<Map>();

}

void LevelState::Update()
{

	//Call the base gamestate update
	GameState::Update();
	
	HUD->SetPosition(pWindowManager->GetWindow()->getView().getCenter());
	HUD->hpBar->SetMaxHealth(player->Maxhealth);
	HUD->hpBar->SetHealth(player->health);

}

GameObject* LevelState::GetPlayer()
{
	return player;
}

void LevelState::MovePlayerToNewState(Player* _newPlayer)
{

	
	player = C_SysContext::Get<Player>();
	player->PrepareForNextLevel();

}

void LevelState::RegisterEnemyKilled()
{

	enemiesKilled += 1;

}