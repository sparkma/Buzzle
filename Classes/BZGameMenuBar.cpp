
#include "BZGameMenuBar.h"
#include "AStageLayer.h"
#include "AWorld.h"

BZGameMenuBar::BZGameMenuBar(CAStageLayer* player) : BZGame(player)
{
	_state = MIS_NA;
}

BZGameMenuBar::~BZGameMenuBar()
{
}

void BZGameMenuBar::initialize(const char* btype, const char* labelSprite)
{
	_bubbletype = btype;
	CASprite* pspr = new BZSpriteCommon(layer(), labelSprite);
	pspr->setState("stand");
	//layer()->addSprite(pspr);
	_psprLabel = pspr;
}

void BZGameMenuBar::createNow()
{
	_state = MIS_Running;
}

void BZGameMenuBar::_doBornStrategy()
{
	if (MIS_Run != _state)
		return;
	_state = MIS_Running;

	int i;
	_Assert(_pboard->getColumns() > 1);
	for (i = 0; i < _pboard->getColumns(); i++)
	{
		BZBubble* pb = _pboard->createBubble(0, i, _bubbletype.c_str(), null, null);
	}
}

void BZGameMenuBar::onUpdate()
{
	BZGame::onUpdate();
}

