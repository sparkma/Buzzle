
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

void BZGameMenu::addBar(const char* id, 
	const char* label, float labscale, const char* bubbletype,
	const CCPoint& pos, int cols, 
	float bubblesize, float zorder)
{
	BZGameMenuBar* pbar = new BZGameMenuBar(id, _pLayer, this);

	pbar->initialize(label, labscale, bubbletype);
	pbar->createBoard(pos, 1, cols, bubblesize, zorder);
	pbar->createNow();
	pbar->onEnter();

	_bars->setObject(pbar, label);
}

void BZGameMenu::onBarClicked(BZGameMenuBar* pbar)
{
	if (null != _plistener)
	{
		_plistener->onBarClicked(pbar->id());
	}
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

