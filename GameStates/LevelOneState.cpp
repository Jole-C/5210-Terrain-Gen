#include "LevelOneState.h"

#include <iostream>

#include "LevelTwoState.h"
#include "../GameObjects/Pickups/SpeedPickup.h"
#include "../GameObjects/Pickups/AmmoPickup.h"
#include "../GameObjects/Pickups/FullHealthPickup.h"
#include "../GameObjects/Pickups/AddHealthPickup.h"
#include "../GameObjects/Pickups/ShotgunPickup.h"
#include "../GameObjects/Pickups/DrillPickup.h"
#include "../GameObjects/Pickups/RocketLauncherPickup.h"
#include "../GameObjects/Pickups/AssaultRiflePickup.h"
#include "../GameObjects/TerrainGenerator/PlaceholderEnemy.h"
#include "../GameObjects/Enemy/EnemyNorm.h"
#include "../GameObjects/Enemy/EnemyHeavy.h"

void LevelOneState::Initialise()
{

	//Initialise the generator variables
	InputGeneratorParams(4.5, 4, 50, 7, sf::Vector2f(128, 128));

	//Set the XML filepath and load the texture
	tileMap.loadFromFile("backgrounds/bg_desert.png");
	xmlPath = "backgrounds/bg_desert.xml";

	//Generate the map
	GenerateMap();

	//Initialise the player
	//We delete the old player because the first level will always be initialised with a new player
	delete(player);
	player = new Player();
	player->Initialise();
	player->SetCurrentLevel(1);
	m_GameObjects.push_back(player);

	//Call the parent initialise to initialise all common level elements
	LevelState::Initialise();

	//Set up the player with the camera
	camera->SetTargetMode(TARGET_OBJECT);
	camera->SetTargetObject(player);

	//Set the position of the player
	sf::Vector2f position = map->PickEligibleAreaWithinRadius(5);

	player->SetPosition(position);

	//Pickup spawning
	std::vector<EntityDefinition> pickups;
	EntityDefinition speedPickup = map->CreateEntityDefinition(new SpeedPickup(), 50);
	EntityDefinition ammoPickup = map->CreateEntityDefinition(new AmmoPickup(), 50);
	EntityDefinition healthPickup = map->CreateEntityDefinition(new FullHealthPickup(), 50);
	EntityDefinition addhealthPickup = map->CreateEntityDefinition(new AddHealthPickup(), 50);
	EntityDefinition shotgunPickup = map->CreateEntityDefinition(new ShotgunPickup(), 50);
	EntityDefinition drillPickup = map->CreateEntityDefinition(new DrillPickup(), 50);
	EntityDefinition rocketLauncherPickup = map->CreateEntityDefinition(new RocketLauncherPickup(), 50);
	EntityDefinition assaultRiflePickup = map->CreateEntityDefinition(new AssaultRiflePickup(), 50);

	pickups.push_back(speedPickup);
	pickups.push_back(ammoPickup);
	pickups.push_back(healthPickup);
	pickups.push_back(addhealthPickup);
	pickups.push_back(shotgunPickup);
	pickups.push_back(drillPickup);
	pickups.push_back(rocketLauncherPickup);
	pickups.push_back(assaultRiflePickup);

	map->SpawnObjectsFromVector(pickups, 100, 2);

	//Enemy spawning
	std::vector<EntityDefinition> enemies;
	EntityDefinition enemy = map->CreateEntityDefinition(new EnemyNorm(), 100);
	EntityDefinition enemyH = map->CreateEntityDefinition(new EnemyHeavy(), 100);
	enemies.push_back(enemy);
	enemies.push_back(enemyH);
	SpawnedEnemies = map->SpawnObjectsFromVector(enemies, 10, 2);
	C_SysContext::Set<std::vector<GameObject*>>(&SpawnedEnemies);
	totalEnemies = SpawnedEnemies.size();
	//totalHEnemies = map->SpawnObjectsFromVector(enemies, 5, 2).size();
	player->SetTotalEnemies(totalEnemies);

	
	
}

void LevelOneState::Update()
{

	//Call the base levelstate update
	LevelState::Update();

	map->Update();

	sf::Vector2f mousePos = C_SysContext::Get<WindowManager>()->GetWindow()->mapPixelToCoords(sf::Mouse::getPosition());
	sf::Vector2f mousePosTilespace = map->WorldspaceToTilespace(mousePos);


	//Manually add enemies with input
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::O) && enemiesKilled < totalEnemies)
	{
		enemiesKilled++;
	}
	//Change level if total enemies = max enemies. We do this here because it's better to let the developer manually manipulate it in case different level goal conditions
	//are required
	
	player->SetEnemiesKilled(enemiesKilled);

	/*if(enemiesKilled == totalEnemies)
	{

		EndGameState* ScoreScreen = new EndGameState();
		
		ScoreScreen->SetEnemiesKilled(enemiesKilled);
		
		
		C_SysContext::Get<GameStateManager>()->QueueGameState(ScoreScreen);

	}*/

}
