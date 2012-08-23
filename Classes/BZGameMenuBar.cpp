
#include "BZGameMenuBar.h"
#include "AStageLayer.h"
#include "AWorld.h"

BZGameMenuBar::BZGameMenuBar(CAStageLayer* player) : BZGame(player)
{
	_state = MIS_NA;
	_psprLabel = null;
	autorelease();
}

BZGameMenuBar::~BZGameMenuBar()
{
	if (null != _psprLabel)
	{
		layer()->removeSprite(_psprLabel);
		_psprLabel->release();
		_psprLabel = null;
	}
}

void BZGameMenuBar::initialize(const char* btype, const char* labelSprite)
{
	_bubbletype = btype;
	CASprite* pspr = new BZSpriteCommon(layer(), labelSprite);
	pspr->setState("stand");
	pspr->setPos(ccp(-100000, -100000));
	//layer()->addSprite(pspr);
	_psprLabel = pspr;
	_psprLabel->retain();
}

void BZGameMenuBar::createNow()
{
	_state = MIS_Run;
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

void BZGameMenuBar::onBubbleClicked(BZBubble* pbubble)
{
	if (MIS_Standing != _state)
		return;

	_state = MIS_Booming;

	BZBlock* pblock = pbubble->getBlock();
	_Assert(null != pblock);
	layer()->removeSprite(_psprLabel);
	pblock->booom();
}

void BZGameMenuBar::onUpdate()
{
	BZGame::onUpdate();
	if (MIS_Running == _state)
	{
		int row = this->_pboard->getRows() - 1;
		int cols = this->_pboard->getColumns();
		BZBubble* pb0 = this->_pboard->getBubbleByGridPos(row, 0);
		BZBubble* pb1 = this->_pboard->getBubbleByGridPos(row, cols - 1);
		if (pb0 && pb1)
		{
			BZBlock* pblock = pb0->getBlock();
			if (pblock->isAllStanding())
			{
				_state = MIS_Standing;
				//show label here
				CCPoint pos0 = pb0->getPos();
				CCPoint pos1 = pb1->getPos();
				CCPoint pos = pos0;
				pos.x += pos1.x;
				pos.y += pos1.y;
				pos.x /= 2;
				pos.y /= 2;
				_pboard->getBubbleRenderPos(pos);
				_psprLabel->setZOrder(60);
				_psprLabel->setPos(pos);

				layer()->addSprite(_psprLabel);
			}
		}
	}
}

