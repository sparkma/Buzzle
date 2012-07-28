
#include "BZGameLogic.h"

///Block
BZBlock::BZBlock(BZGame* pgame)
{
	_pgame = pgame;
	_pgroup = null;
	_state = BS_NA;
}

BZBlock::~BZBlock()
{
}

void BZBlock::onEnter()
{
}

void BZBlock::onUpdate()
{
}

void BZBlock::onExit()
{
}


/// Game
BZGame::BZGame(CAStageLayer* player)
{
	_pLayer = player;
}

BZGame::~BZGame()
{
}

void BZGame::onEnter()
{
}

void BZGame::onUpdate()
{
	//create some blocks if ness
	ccTime time = _pLayer->getTimeNow();
	int type = (int)CAUtils::Rand(0, BLOCK_TYPES);
}

void BZGame::onExit()
{
}

