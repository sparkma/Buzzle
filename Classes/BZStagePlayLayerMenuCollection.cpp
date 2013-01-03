
#include "BZStagePlayLayerMenuCollection.h"
#include "BZStageCommon.h"

#include "AWorld.h"
#include "AString.h"

BZStagePlayLayerMenuCollection::BZStagePlayLayerMenuCollection(CAStage* pstage, CAStageLayer* playerParent) : BZStageLayerCommon(pstage, playerParent)
{
	_Trace("%s allocated", __FUNCTION__);
	_fIndexTo = _fIndex = 0;
	memset(_psprs, 0, sizeof(_psprs));
}

BZStagePlayLayerMenuCollection::~BZStagePlayLayerMenuCollection(void)
{
	_Trace("%s destroyed", __FUNCTION__);
}

void BZStagePlayLayerMenuCollection::_onIdle()
{
	BZStageLayerCommon::_onIdle();
	memset(_psprs, 0, sizeof(_psprs));

	_gesture.setTimeProvider(_pstage);
	_gesture.addEventListener(this);
	_gesture.setAcc(0.93f);
}

void BZStagePlayLayerMenuCollection::_onRunning()
{
	BZStageLayerCommon::_onRunning();
	_fIndexTo = _fIndex = 0;
}

void BZStagePlayLayerMenuCollection::onUpdate()
{
	BZStageLayerCommon::onUpdate();
	if (CAMachine::getCurrentState()->getFullName() == "root.running")
	{
		_gesture.update();
		//_doSlideManual();
	}
}

void BZStagePlayLayerMenuCollection::_onButtonCommand(CASprite* pbutton)
{
	BZStageCommon* pstage = (BZStageCommon*)this->stage();
	string btn = pbutton->getModName();
	if (btn == "button_home")
	{
		replaceLayer("home");
		return;
	}

	int stages = 1;
	int levels = 5;
	for (int s = 0; s < stages; s++)
	{
		for (int l = 0; l < levels; l++)
		{
			char name[32];
			sprintf(name, "stage%02d_level%02d", s + 1, l + 1);
			if (btn == name)
			{
				replaceLayer("play_collection");
				//_buttonCommand = btn;
				//this->hide();
			}
		}
	}
}

void BZStagePlayLayerMenuCollection::onEvent(const CAEvent* pevt)
{
	BZStageLayerCommon::onEvent(pevt);

	switch (pevt->type())
	{
	case ET_Touch:
		if (null != _psprs[0])
		{
			_gesture.onTouch((CAEventTouch*)pevt);
		}
		break;
	case ET_Command:
		/*
		{
			CAEventCommand* pc = (CAEventCommand*)pevt;
			if (pc->getSenderType() == ST_Sprite)
			{
				CASprite* p = (CASprite*)pc->sender();
				if (pc->command() == "animStop")
				{
					if (CAString::startWith(p->getModName(), "help_p"))
					{
						const CASpriteModelPoseModifierKey& k = p->getCombinedKey();
						p->setPos(k.x, k.y);
					}
				}
			}
		}
		*/
		break;
	case ET_Gesture:
		{
		}
		break;
	}
}

#include "AWorld.h"
REG_CLASS(BZStagePlayLayerMenuCollection);
