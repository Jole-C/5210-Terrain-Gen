#include "TerrainGenerator.h"
#include <iostream>
#include <tinyxml2.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include "PlaceholderPickup.h"
#include "../../../Engine/Renderer/RenderManager.h"
#include "../../MathHelpers.h"
#include "../../../Engine/System/GameState/GameState.h"
#include "../../../Engine/System/GameState/GameStateManager.h"
#include "../ParticleSystem/ParticleSystem.h"

//Initialise map generation settings and vectors
void TerrainGenerator::Initialise(int _birthLimit, int _deathLimit, int _initialChance, int _numberOfSteps, sf::Vector2f _size)
{

	//Initialise variables
	birthLimit = _birthLimit;
	deathLimit = _deathLimit;
	initialChance = _initialChance;
	numberOfSteps = _numberOfSteps;
	size = _size;

	srand((time(NULL)));
	
}

//Generate the data map (using Tile objects)
std::vector<std::vector<Tile>> TerrainGenerator::GenerateDataMap()
{

	//Initialise the vector
	for (int row = 0; row < size.x; row++)
	{

		std::vector<Tile> column;
		for (int col = 0; col < size.y; col++)
		{

			Tile newTile;
			newTile.InitialiseTile(FLOOR);
			newTile.SetVisualIndex(col + (row * size.x));

			if (rand() % 100 < initialChance)
			{
				newTile.InitialiseTile(WALL);
			}

			column.push_back(newTile);
			dataList.push_back(newTile);
		}

		dataMap.push_back(column);

	}

	//Repeat step for the given amount
	for (int i = 0; i < numberOfSteps-1; i++)
	{

		MapGenerationStep();

	}
	
	//Add wall texture
	for (int x = 0; x < size.x; x++)
	{

		for (int y = 1; y < size.y - 1; y++)
		{

			//If the tile is a wall and the tile below is a floor and the tile above is a wall, make the tile a wall front
			Tile tileAbove = dataMap[x][y - 1];
			Tile tileBelow = dataMap[x][y + 1];

			if (tileAbove.GetTileType() == WALL && tileBelow.GetTileType() == FLOOR)
			{
				dataMap[x][y].InitialiseTile(WALL_FRONT);
			}

		}

	}

	//Add indestructible area around the perimeter
	for(int row = 0; row < size.x; row++)
	{

		for(int col = 0; col < size.y; col++)
		{

			if (row == 0)dataMap[0][col].InitialiseTile(WALL_INDESTRUCTIBLE);
			if (row == size.x - 1)dataMap[size.x - 1][col].InitialiseTile(WALL_INDESTRUCTIBLE);
			if (col == 0)dataMap[row][0].InitialiseTile(WALL_INDESTRUCTIBLE);
			if (col == size.y - 1)dataMap[row][size.y - 1].InitialiseTile(WALL_INDESTRUCTIBLE);

		}

	}

	return dataMap;

}

//Perform a map generation step
void TerrainGenerator::MapGenerationStep()
{

	std::vector<std::vector<Tile>> newMap;
	

	//Initialise the vector
	for (int row = 0; row < size.x; row++)
	{

		std::vector<Tile> column;

		for (int col = 0; col < size.y; col++)
		{

			Tile newTile;

			newTile.InitialiseTile(FLOOR);
			newTile.SetVisualIndex(row + (col * size.x));

			column.push_back(newTile);

		}

		newMap.push_back(column);

	}

	//Fill map with data based on old map
	for (int row = 0; row < size.x; row++)
	{

		for (int col = 0; col < size.y; col++)
		{

			sf::Vector2f position = sf::Vector2f(row, col);

			int neighbours = CountNeighbours(position);

			if (dataMap[row][col].GetTileType() == WALL)
			{

				if (neighbours < deathLimit)
				{
					newMap[row][col].InitialiseTile(FLOOR);
				}
				else
				{
					newMap[row][col].InitialiseTile(WALL);
				}
				
			}
			else
			{

				if(neighbours > birthLimit)
				{
					newMap[row][col].InitialiseTile(WALL);
				}
				else
				{
					newMap[row][col].InitialiseTile(FLOOR);
				}

			}

		}

	}

	//Replace old map
	dataMap = newMap;

}

//TerrainGenerator utility function, not needed outside of this
int TerrainGenerator::CountNeighbours(sf::Vector2f _position)
{

	int cellCount = 0;

	for (int x = -1; x < 2; x++)
	{
		
		for (int y = -1; y < 2; y++)
		{

			int neighbourX = _position.x + x;
			int neighbourY = _position.y + y;

			if (x == 0 && y == 0)
			{
				//Do nothing
			}
			else if (neighbourX < 0 || neighbourY < 0 || neighbourX > size.x - 2 || neighbourY > size.y - 2)
			{
				cellCount += 1;
			}
			else if (dataMap[neighbourX][neighbourY].GetTileType() == WALL)
			{
				cellCount += 1;
			}

		}

	}

	return cellCount;

}


//Initialise (or change) a tile - Classes damaging tiles do not need this
void Tile::InitialiseTile(TILE_TYPES _tileType)
{
	tileType = _tileType;
}


void Tile::Update()
{

}


void Tile::DamageTile(int _damage)
{
	tileHealth -= _damage;
}


//Get a tile's type
TILE_TYPES Tile::GetTileType()
{
	return tileType;
}


void Tile::SetVisualIndex(int _index)
{
	visualIndex = _index;
	//std::cout << visualIndex << std::endl;
}


int Tile::GetHealth()
{
	return tileHealth;
}


void Tile::SetHealth(int _health)
{
	tileHealth = _health;
}


int Tile::GetVisualMapIndex()
{
	return visualIndex;
}


void Map::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform = getTransform();
	states.texture = &tileMap;

	target.draw(visualMap, states);
}


std::vector<GameObject*> Map::SpawnObjectsFromVector(std::vector<EntityDefinition> _objects, int _amount, int _radiusOfTileCarving)
{

	std::vector<GameObject*> objectsSpawned;

	//Spawn an object
	for (int i = 0; i < _amount; i++)
	{

		//Get the total weight
		double totalWeight = 0;

		for (EntityDefinition entity : _objects)
		{

			totalWeight += entity.spawnChance;

		}

		int randomNumber = RandomRangeInteger(0, totalWeight);

		for (EntityDefinition entity : _objects)
		{

			if (entity.spawnChance > randomNumber)
			{

				sf::Vector2f position = PickEligibleAreaWithinRadius(_radiusOfTileCarving);

				objectsSpawned.push_back(entity.SpawnObject(position));

				break;

			}
			else
			{

				randomNumber -= entity.spawnChance;

			}

		}

	}

	return objectsSpawned;

}


//Initialise the map
void Map::Initialise()
{
	
	C_SysContext::Get<RenderManager>()->AddRenderObject(this, 1000);

}


//Pick a random eligible tile
sf::Vector2f Map::PickEligibleAreaWithinRadius(int _radius)
{

	//Pick a random tile
	sf::Vector2f position;

	position.x = RandomRangeInteger(0, mapSize.x - 1);
	position.y = RandomRangeInteger(0, mapSize.y - 1);

	//Clear out the area around it
	for (int x = position.x - _radius; x < position.x + _radius; x++)
	{

		for (int y = position.y - _radius; y < position.y + _radius; y++)
		{
			
			//Clamp the value to the bounds of the map
			sf::Vector2f clampedVector = ClampCoordinatesToBounds(sf::Vector2f(x, y));

			DamageTile(clampedVector, 5, true);

		}

	}

	//Return the position
	position.x *= tileSize;
	position.y *= tileSize;

	return position;

}


int Map::GetTileSize()
{
	return tileSize;
}


int Map::ChooseRandomFloor()
{

	//Returns a random floor index
	return RandomRangeInteger(0, 1);

}


int Map::ChooseRandomWall()
{

	//Returns a random wall index
	int index = RandomRangeInteger(2, 11);

	if(index % 2 == 1)
	{

		index -= 1;

	}

	return index;

}


void Map::SetTileSize(int _newSize)
{
	tileSize = _newSize;
}


Tile Map::SetTile(sf::Vector2f _tileCoordinates, Tile _tile)
{

	_tileCoordinates = ClampCoordinatesToBounds(_tileCoordinates);

	dataMap[_tileCoordinates.x][_tileCoordinates.y] = _tile;

	return dataMap[_tileCoordinates.x][_tileCoordinates.y];

}

sf::Vector2f Map::ClampCoordinatesToBounds(sf::Vector2f _tileCoordinates)
{

	int clampedx = Mid(0, _tileCoordinates.x, mapSize.x - 1);
	int clampedy = Mid(0, _tileCoordinates.y, mapSize.y - 1);

	return sf::Vector2f(clampedx, clampedy);

}

EntityDefinition Map::CreateEntityDefinition(GameObject* _object, int _spawnChance)
{

	EntityDefinition newEntity;

	newEntity.object = _object;
	newEntity.spawnChance = _spawnChance;

	return newEntity;
}


void Map::GenerateVisualMap(const char* _fileName, const sf::Texture _tileMap)
{

	GenerateTileDefinitions(_fileName);

	//Set the map texture
	tileMap = _tileMap;
	mapSize = sf::Vector2f(dataMap.size(), dataMap[0].size());

	//Generate the visual map
	visualMap.setPrimitiveType(sf::Quads);

	visualMap.resize((mapSize.x) * (mapSize.y) * 4);

	for(int x=0; x < mapSize.x; x++)
	{

		for(int y=0; y < mapSize.y;y++)
		{


			//Flatten the datamap index into a flat 1 dimensional array for the visual map index
			int flattenedDatamapIndex = x + y * mapSize.x;

			int tileNumber = GetTile(sf::Vector2f(x, y)).GetTileType();

			//Get the x and y from the tile definition
			int tileX = tileDefinitions[tileNumber].x;
			int tileY = 0;
			int tileWidth = tileDefinitions[tileNumber].width;
			
			//If the tile is a wall front, randomise it
			if (tileNumber == WALL_FRONT)
			{

				tileX = tileDefinitions[ChooseRandomWall()].x;

			}
			else if(tileNumber == FLOOR)
			{

				tileX = tileDefinitions[ChooseRandomFloor()].x;

			}

			//Get the current quad and set the values to that map
			sf::Vertex* quad = &visualMap[(flattenedDatamapIndex) * 4];

			//Calculate the position on the map for the tiles
			int posX = x * tileSize;
			int posY = y * tileSize;
			int posX2 = (x + 1) * tileSize;
			int posY2 = (y + 1) * tileSize;
			
			quad[0].position = sf::Vector2f(posX, posY);
			quad[1].position = sf::Vector2f(posX2, posY);
			quad[2].position = sf::Vector2f(posX2, posY2);
			quad[3].position = sf::Vector2f(posX, posY2);

			//Calculate the position on the sprite sheet for the texture coordinates
			int posXT = tileX;
			int posYT = tileY;
			int posX2T = tileX + tileWidth;
			int posY2T = tileY + tileWidth;

			quad[0].texCoords = sf::Vector2f(posXT, posYT);
			quad[1].texCoords = sf::Vector2f(posX2T, posYT);
			quad[2].texCoords = sf::Vector2f(posX2T, posY2T);
			quad[3].texCoords = sf::Vector2f(posXT, posY2T);

		}

	}

}


void Map::GenerateTileDefinitions(const char* _fileName)
{

	tinyxml2::XMLDocument doc;

	if (doc.LoadFile(_fileName) != tinyxml2::XMLError::XML_SUCCESS)
	{

		doc.LoadFile("backgrounds/bg_desert.xml");

	}

	tinyxml2::XMLElement* textureAtlas = doc.FirstChildElement("TextureAtlas");

	for (tinyxml2::XMLNode* node = textureAtlas->FirstChild(); node; node = node->NextSibling())
	{

		TileDefinition tileDefinition;

		//Extract the elements to form the tile definition
		for(const tinyxml2::XMLAttribute* attribute = ((tinyxml2::XMLElement*)node)->FirstAttribute(); attribute; attribute = attribute->Next())
		{

			if(strcmp(attribute->Name(), "n") == 0)
			{

				tileDefinition.name = attribute->Value();

			}
			if (strcmp(attribute->Name(), "x") == 0)
			{

				tileDefinition.x = attribute->IntValue();

			}
			if (strcmp(attribute->Name(), "y") == 0)
			{

				tileDefinition.y = attribute->IntValue();

			}
			if(strcmp(attribute->Name(), "w") == 0)
			{
				tileDefinition.width = attribute->IntValue();
			}

		}

		tileDefinitions.push_back(tileDefinition);

	}

}


void Map::SetDataMap(std::vector<std::vector<Tile>> _dataMap)
{
	dataMap = _dataMap;
}


Tile Map::GetTile(sf::Vector2f _tileCoordinates)
{

	sf::Vector2f clampedBounds = ClampCoordinatesToBounds(_tileCoordinates);

	return dataMap[clampedBounds.x][clampedBounds.y];

}


//Convert worldspace coordinates to tilespace
sf::Vector2f Map::WorldspaceToTilespace(sf::Vector2f _worldCoordinates)
{
	return sf::Vector2f(_worldCoordinates.x/ tileSize, _worldCoordinates.y/ tileSize);
}


//Damage a tile - Used by other classes
//Uses bitmasking to calculate which tile should be set
void Map::DamageTile(sf::Vector2f _position, int _damage, bool _noExplosion)
{

	//Clamp the position to the bounds of the map
	_position = ClampCoordinatesToBounds(_position);

	//Get the tile at the position
	Tile tile = GetTile(_position);

	//If the tile is indestructible, return
	if(tile.GetTileType() == WALL_INDESTRUCTIBLE)
	{

		return;

	}

	//Damage the tile if it's a wall
	if(tile.GetTileType() == WALL)
	{

		//Flash the tile white
		TileFlash* flash = new TileFlash();
		flash->Initialise();
		flash->setPosition((_position.x * tileSize) + tileSize / 2, (_position.y * tileSize) + tileSize / 2);
		flash->tileScale = sf::Vector2f(tileSize, tileSize);

		GameState* currentState = C_SysContext::Get<GameStateManager>()->GetCurrentGameState();
		currentState->AddGameObject(flash);

		//Flash the tile below that white if it's a wall front
		if(GetTile(sf::Vector2f(_position.x, _position.y + 1)).GetTileType() == WALL_FRONT)
		{

			TileFlash* flash = new TileFlash();
			flash->Initialise();
			flash->setPosition((_position.x * tileSize) + tileSize / 2, ((_position.y + 1) * tileSize) + tileSize / 2);
			flash->tileScale = sf::Vector2f(tileSize, tileSize);

			GameState* currentState = C_SysContext::Get<GameStateManager>()->GetCurrentGameState();
			currentState->AddGameObject(flash);

		}

		tile.DamageTile(_damage);

		//Add the tile back into the map with the updated damage, then get it again
		SetTile(_position, tile);

		if(tile.GetHealth() <= 0)
		{


			//Damage the tile
			if(_noExplosion == false)C_SysContext::Get<ParticleSystem>()->Explosion(_position.x * tileSize + tileSize/2, _position.y * tileSize + tileSize/2, 10);

			int north = 1;
			int south = 2;
			int bitValue = 0;

			//Check tiles above and below
			for(int y = -1; y <= 1; y++)
			{

				//Clamp the value to be within the bounds of the map
				int tileY = Mid(0, _position.y + y, mapSize.y-1);

				Tile tileChecked = GetTile(sf::Vector2f(_position.x, tileY));

				//Check the tile type
				if(y != 0)
				{

					if(y == -1)
					{

						if(tileChecked.GetTileType() == WALL)
						{

							bitValue += north;

						}

					}
					else if(y == 1)
					{


						if (tileChecked.GetTileType() == WALL)
						{

							bitValue += south;

						}
						//If the tile below is a wall front, we know we want to set it to a floor, so we set it here as the tile is already stored in memory
						if(tileChecked.GetTileType() == WALL_FRONT)
						{
							tileChecked.InitialiseTile(FLOOR);
							UpdateVisualMapIndex(tileChecked.GetVisualMapIndex(), FLOOR);
							SetTile(sf::Vector2f(_position.x, tileY), tileChecked);
						}

					}

				}

			}

			//Use the calculated bitvalue to set the tile that was originally damaged
			switch(bitValue)
			{

			case 0:

				tile.InitialiseTile(FLOOR);
				UpdateVisualMapIndex(tile.GetVisualMapIndex(), FLOOR);

				break;

			case 1:

				tile.InitialiseTile(WALL_FRONT);
				UpdateVisualMapIndex(tile.GetVisualMapIndex(), WALL_FRONT);

				break;

			case 2:

				tile.InitialiseTile(FLOOR);
				UpdateVisualMapIndex(tile.GetVisualMapIndex(), FLOOR);

				break;

			case 3:

				tile.InitialiseTile(WALL_FRONT);
				UpdateVisualMapIndex(tile.GetVisualMapIndex(), WALL_FRONT);

				break;

			default:

				tile.InitialiseTile(FLOOR);
				UpdateVisualMapIndex(tile.GetVisualMapIndex(), WALL);

			}

			SetTile(sf::Vector2f(_position.x, _position.y), tile);

		}

	}

	

}


void Map::Explosion(sf::Vector2f _position, int _radius, int _damage)
{

	//Clamp the explosion position to the map
	_position = ClampCoordinatesToBounds(_position);

	for(int x = _position.x - _radius / 2; x < _position.x + _radius / 2; x++)
	{

		for (int y = _position.y - _radius / 2; y < _position.y + _radius / 2; y++)
		{

			sf::Vector2f clampedBounds = ClampCoordinatesToBounds(sf::Vector2f(x, y));

			DamageTile(sf::Vector2f(clampedBounds), _damage);

		}

	}

}


void Map::UpdateVisualMapIndex(int _visualMapIndex, TILE_TYPES _tileType)
{


	//Randomise floor and wallfront tile visuals
	int tile = _tileType;

	if(_tileType == FLOOR)
	{

		tile = ChooseRandomFloor();

	}
	else if(_tileType == WALL_FRONT)
	{

		tile = ChooseRandomWall();

	}

	//Update the texture coordinates for a given tile
	sf::Vertex* quad = &visualMap[_visualMapIndex * 4];

	int posXT = tileDefinitions[tile].x;
	int posYT = 0;
	int posX2T = posXT + tileDefinitions[tile].width;
	int posY2T = 64;

	quad[0].texCoords = sf::Vector2f(posXT, posYT);
	quad[1].texCoords = sf::Vector2f(posX2T, posYT);
	quad[2].texCoords = sf::Vector2f(posX2T, posY2T);
	quad[3].texCoords = sf::Vector2f(posXT, posY2T);

}


void Map::Update()
{
	

}


GameObject* EntityDefinition::SpawnObject(sf::Vector2f _position)
{

	//Add the object to the current state
	//Initialise the object
	GameObject* newObject = object->ReturnNewCopy();
	newObject->SetPosition(_position);
	newObject->Initialise();

	GameState* currentState = C_SysContext::Get<GameStateManager>()->GetCurrentGameState();

	currentState->AddGameObject(newObject);

	return newObject;

}

void TileFlash::Initialise()
{

	sprite.Initialise("sprites/tile_flash.png");

}

void TileFlash::Update()
{

	sprite.setPosition(GetPosition());
	sprite.setScale(tileScale.x, tileScale.y);
	sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
	sprite.setColor(sf::Color(255, 255, 255, 100));


	flashTime -= 1;

	if(flashTime <= 0)
	{

		GameState* currentState = C_SysContext::Get<GameStateManager>()->GetCurrentGameState();

		currentState->RemoveGameObject(this);

	}

}

//-------------IMPLEMENTATION OF GAME OBJECT INTERFACE-------------------------------------------------------

void Map::SetPosition(float px, float py)
{
	this->setPosition(px, py);
}

void Map::SetPosition(sf::Vector2f ppos)
{
	this->setPosition(ppos);
}

void Map::GetPosition(float& x, float& y)
{
	x = getPosition().x;
	y = getPosition().y;
}

sf::Vector2f Map::GetPosition()
{
	return this->getPosition();
}

void TileFlash::SetPosition(float px, float py)
{
	this->setPosition(px, py);
}

void TileFlash::SetPosition(sf::Vector2f ppos)
{
	this->setPosition(ppos);
}

void TileFlash::GetPosition(float& x, float& y)
{
	x = getPosition().x;
	y = getPosition().y;
}

sf::Vector2f TileFlash::GetPosition()
{
	return this->getPosition();
}

