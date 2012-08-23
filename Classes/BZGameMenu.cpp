
#include "BZGameMenu.h"
#include "AStageLayer.h"
#include "AWorld.h"

BZGameMenu::BZGameMenu(CAStageLayer* player, IBZGameMenuListener* plistener)
{
	_pLayer = player;
	_bars = new CAStringMap<BZGameMenuBar>();
	_plistener = plistener;
}

BZGameMenu::~BZGameMenu()
{
	_bars->release();
	_bars = null;
	_pLayer = null;
	_plistener = null;
}

void BZGameMenu::addBar(
	const char* label, const char* bubbletype,
	const CCPoint& pos, int cols, float bubblesize)
{
	BZGameMenuBar* pbar = new BZGameMenuBar(_pLayer);

	pbar->initialize(bubbletype, label);
	pbar->createBoard(pos, 1, cols, bubblesize);
	pbar->createNow();
	pbar->onEnter();

	_bars->setObject(pbar, label);
}

void BZGameMenu::onUpdate()
{
	CCDictElement* pElement = NULL;
	CCDICT_FOREACH(_bars, pElement)
	{
		BZGameMenuBar* pbar = (BZGameMenuBar*)pElement->getObject();
		_Assert(pbar);
		pbar->onUpdate();
	}
}

void BZGameMenu::onEvent(CAEvent* pevt)
{
	CCDictElement* pElement = NULL;
	CCDICT_FOREACH(_bars, pElement)
	{
		BZGameMenuBar* pbar = (BZGameMenuBar*)pElement->getObject();
		_Assert(pbar);
		pbar->onEvent(pevt);
	}
}

