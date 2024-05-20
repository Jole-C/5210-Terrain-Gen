#pragma once
#include "LevelState.h"

class LevelTwoState : public LevelState
{
public:

	void Initialise() override;
	void Update() override;

	std::vector<GameObject*> SpawnedEnemies;
};

