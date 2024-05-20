#pragma once
#include "LevelState.h"

class LevelOneState : public LevelState
{

public:

	void Initialise() override;
	void Update() override;
	int plswork = 300;
	std::vector<GameObject*> SpawnedEnemies;
private:

};