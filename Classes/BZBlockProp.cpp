
#include "BZBlockProp.h"
#include "BZBoard.h"
#include "BZGame.h"
#include "AStageLayer.h"
#include "AWorld.h"

BZBlockProp::BZBlockProp(BZBoard* pboard)
{
	_pboard = pboard;
	_pblock = null;
	_psprProp = null;
}

BZBlockProp::~BZBlockProp()
{
}

//init sprite with name
void BZBlockProp::initialize(const char* prop)
{
	_type = prop;
}

void BZBlockProp::onEnter()
{
}

void BZBlockProp::onUpdate()
{
}

void BZBlockProp::onExit()
{
}

