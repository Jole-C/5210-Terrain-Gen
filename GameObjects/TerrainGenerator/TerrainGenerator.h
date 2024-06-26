#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <vector>
#include <random>
#include <SFML/Graphics/Transformable.hpp>
#include <tinyxml2.h>

#include "../../../Engine/System/Context/SysContextProvider.h"
#include "../../../Engine/Renderer/WindowManager.h"
#include "../../../Engine/Core/GameObject.h"
#include "../../../Engine/Renderer/Sprite.h"

/**
 * \brief Enum holding each possible tile type Useful for collisions! Seriously! Use it! Please!\n
 * \n
 * In all seriousness, a tile's visual representation may be randomised slightly - For example, different wall front tiles under the same WALL_FRONT index.\n
 * \n
 * Therefore, it is always preferred to use this over manually checking the visual index of a tile, which will bring you nothing but pain.
*/
enum TILE_TYPES
{
	WALL = 17,
	FLOOR = 0,
	WALL_FRONT = 2,
	WALL_INDESTRUCTIBLE = 18
};

/**
 * \brief An entity definition is a useful class that is required by the map to spawn objects. The entitydefinition holds a pointer to a class and a chance for the class to be spawned.\n
 * \n
 * To use, simply call the map's CreateEntityDefinition class, and it will return one constructed for you from your input arguments.
 */
class EntityDefinition
{

public:

	GameObject* object;
	int spawnChance;

private:

	GameObject* SpawnObject(sf::Vector2f _position); //Spawn a copy of the object at the given position

	friend class Map;
};

/**
 * \brief Holds the data on an individual tile in the data map. This class DOES NOT need to be messed with by other classes or developers, except for using\n
 * GetTileType()! Use the Map class instead!
 */
class Tile
{

public:

	//Utility
	TILE_TYPES GetTileType(); //Returns the tile type
	int GetHealth(); //Returns health of tile
	int GetVisualMapIndex(); //Returns visual map index for ease of access


private:

	TILE_TYPES tileType;
	sf::Vector2f position;
	
	int tileHealth = 5;
	int visualIndex;

	//Functional
	void InitialiseTile(TILE_TYPES _tileType); //Sets initial tile type, and can be used to reset a tile
	void Update(); //Update the tile each frame if required
	void DamageTile(int _damage); //Damages the tile
	void SetVisualIndex(int _index); //Used to set the position in the vertex array for the tile, for ease of access later.
	void SetHealth(int _health); //Sets health of tile

	friend class Map;
	friend class TerrainGenerator;

};

/**
 * \brief A tile definition holds all data taken from the xml of the given file when generating a new visual map. This is not needed by most people. It just makes my life easier.
 */
class TileDefinition
{

public:
	int x;
	int y;
	int width;
	const char* name;

};

class TileFlash : public GameObject, public sf::Transformable
{

public:
	sf::Vector2f tileScale;

private:
	void Initialise() override;
	void Update() override;
	sf::Vector2f GetPosition() override;
	void SetPosition(float px, float py) override;
	void SetPosition(sf::Vector2f ppos) override;
	void GetPosition(float& x, float& y) override;

	Sprite sprite;
	int flashTime = 5;

	friend class Map;

};

/**
 * \brief The map class holds both the data map generated by the terrain generator, and the visual representation referred to as a visual map.
 *
 * The map class contains methods for interacting with the map, including getting tiles, damaging tiles, explosions, and even object spawning.
 * You do not ever have to manually edit either the visual map or the data map, as these can lead to messy code or hard to solve bugs.
 * The map class handles clamping all values to the map, manually editing and randomising tiles around destroyed tiles, etc.\n
 *\n
 * Methods useful to you from this class include:\n
 * WorldspaceToTilespace() - Converts coordinates in world space to the position of a tile in the same location on the map.\n
 * GetTileSize() & GetTile() - Used to get the size in pixels of all tiles and a copy of a specific tile at a specific index (NOT A REFERENCE).\n
 * DamageTile() & Explosion() - Used to damage tiles. The explosion method allows you to define a radius of tiles to destroy.\n
 * \n
 * The Map class will always be in the Context Provider, provided a LevelState has been set up correctly.\n
 * \n
 * Have fun!
 */
class Map : public GameObject, public sf::Drawable, public sf::Transformable
{
protected:

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
	
	//Overrides
	void Initialise() override;
	void Update() override;
	sf::Vector2f GetPosition();
	void SetPosition(float px, float py);
	void SetPosition(sf::Vector2f ppos);
	void GetPosition(float& x, float& y);

	//Utility
	sf::Vector2f  PickEligibleAreaWithinRadius(int _radius); //Picks a random area within the map, and digs out tiles within the specified radius
	Tile GetTile(sf::Vector2f _tileCoordinates); //Get the tile at the given tilespace coordinate
	Tile SetTile(sf::Vector2f _tileCoordinates, Tile _tile); //Sets the tile at the given tilespace coordinate with the given tile, then returns the updated tile
	sf::Vector2f ClampCoordinatesToBounds(sf::Vector2f _tileCoordinates); //Clamps the given coordinates to the bounds of the map
	int GetTileSize(); //Returns the current tile size
	void SetTileSize(int _newSize); //Sets a new tile size (usually not required)
	EntityDefinition CreateEntityDefinition(GameObject* _object, int _spawnChance); //Creates an entity definition holding a gameobject and a weighted chance

	//Functional
	sf::Vector2f WorldspaceToTilespace(sf::Vector2f _worldCoordinates); //Converts from coordinates where 1 = a pixel to 1 = a tile
	void GenerateVisualMap(const char* _fileName, const sf::Texture _tileMap); //Generate the visual representation of the data map
	std::vector<GameObject*> SpawnObjectsFromVector(std::vector<EntityDefinition> _objects, int _amount, int _radiusOfTileCarving); //Spawns objects from a given vector
	void DamageTile(sf::Vector2f _position, int _damage, bool _noExplosion = false); //Damage a tile at the given coordinate
	void Explosion(sf::Vector2f _position, int _radius, int _damage);//Create an explosion in a square radius at the given coordinate
	void SetDataMap(std::vector<std::vector<Tile>> _dataMap); //Set the datamap to the vector parameter


private:

	//Utility
	int ChooseRandomFloor(); //Chooses a random floor tile from the tile definitions
	int ChooseRandomWall(); //Chooses a random wall tile from the wall definitions
	void GenerateTileDefinitions(const char* _fileName); //Generates a tile definition class for each tile in an xml file

	//Functional
	void UpdateVisualMapIndex(int _visualMapIndex, TILE_TYPES _tileType); //Update a tile's visual index at the given index to the given tile type, randomising it if required

	sf::VertexArray visualMap;
	std::vector<std::vector<Tile>> dataMap;
	std::vector<TileDefinition> tileDefinitions;
	sf::Vector2f mapSize;
	sf::Texture tileMap;

	int tileSize = 48;

};

/**
 * \brief A class used to generate a 2 dimensional vector referred to as a data map through the use of cellular automata.
 * The data map is a representation of all tiles in the level, and is fed into a Map class container which contains all methods and helper functions used
 * for interacting with that data map.
 *
 * You should not have to ever manually edit the data map, as the map class is both easier to use and contains all methods you may need.
 */
class TerrainGenerator
{

public:

	void Initialise(int _birthLimit, int _deathLimit, int _initialChance, int _numberOfSteps, sf::Vector2f _size); //Initialises the terrain generator with the given parameters, and reseeds the rng
	std::vector<std::vector<Tile>> GenerateDataMap(); //Generates the data equivalent of the map - Used for all map logic, fed into Map class to generate visual equivalent

private:

	void MapGenerationStep(); //Performs a map step, smoothing out the map. Repeated for number of steps defined in initialisation
	int CountNeighbours(sf::Vector2f _position); //Counts the number of neighbours surrounding a tile

	int birthLimit;
	int deathLimit;
	int initialChance;
	int numberOfSteps;
	sf::Vector2f size;

	std::vector<std::vector<Tile>> dataMap;
	std::vector<Tile> dataList;

};

