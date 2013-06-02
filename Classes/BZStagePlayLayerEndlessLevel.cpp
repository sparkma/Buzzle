
#include "BZStagePlayLayerEndlessLevel.h"
#include "BZStageCommon.h"
#include "BZWorld.h"
#include "BZGame.h"

#include "ASettings.h"
#include "AString.h"

BZButtonLevelItem::BZButtonLevelItem()
{
	_beginlevel = 0;
	_level = _stars = 0;
	_locked = true;
	_psprBack = null;
	_psprStar1 = null;
	_psprStar2 = null;
	_psprStar3 = null;
	_psprNum1 = null;
	_psprNum2 = null;
	_psprNum3 = null;
	_psprLock = null;
}

BZButtonLevelItem::~BZButtonLevelItem()
{
	_psprBack = null;
	_psprStar1 = null;
	_psprStar2 = null;
	_psprStar3 = null;
	_psprNum1 = null;
	_psprNum2 = null;
	_psprNum3 = null;
	_psprLock = null;
}

void BZButtonLevelItem::bindSprites(
	CASprite* psprBack, 
	CASprite* psprStar1, 
	CASprite* psprStar2, 
	CASprite* psprStar3, 
	CASprite* psprNum1, 
	CASprite* psprNum2, 
	CASprite* psprNum3, 
	CASprite* psprLock)
{
	_psprBack = psprBack;				if (_psprBack )	_ptBack = _psprBack->getPos();
	_psprStar1 = psprStar1;				if (_psprStar1 )_ptStar1 = _psprStar1->getPos();
	_psprStar2 = psprStar2;				if (_psprStar2 )_ptStar2 = _psprStar2->getPos();
	_psprStar3 = psprStar3;				if (_psprStar3 )_ptStar3 = _psprStar3->getPos();
	_psprNum1 = psprNum1;				if (_psprNum1 )	_ptNum1 = _psprNum1->getPos();
	_psprNum2 = psprNum2;				if (_psprNum2 )	_ptNum2 = _psprNum2->getPos();
	_psprNum3 = psprNum3;				if (_psprNum3 )	_ptNum3 = _psprNum3->getPos();
	_psprLock = psprLock;				if (_psprLock )	_ptLock = _psprLock->getPos();
}		

void BZButtonLevelItem::setLevelInfo(int beginlevel, int level, int stars, bool locked)
{
	_Assert(_psprBack);
	_Assert(_psprStar1);
	_Assert(_psprStar2);
	_Assert(_psprStar3);
	_Assert(_psprNum1);
	_Assert(_psprNum2);
	_Assert(_psprNum3);
	_Assert(_psprLock);
	if (level <= 0)
	{
		//disable all
		_psprBack->setVisible(false);
		_psprStar1->setVisible(false);
		_psprStar2->setVisible(false);
		_psprStar3->setVisible(false);
		_psprNum1->setVisible(false);
		_psprNum2->setVisible(false);
		_psprNum3->setVisible(false);
		_psprLock->setVisible(false);
		return;
	}

	_psprBack->env().setInteger("level", level);

	_psprBack->setVisible(true);
	_psprStar1->setVisible(true);
	_psprStar2->setVisible(true);
	_psprStar3->setVisible(true);
	_psprNum1->setVisible(true);
	_psprNum2->setVisible(true);
	_psprNum3->setVisible(true);
	_psprLock->setVisible(true);

	_beginlevel = beginlevel;
	_level = level;
	char sz[8];
	sprintf(sz, "%03d", level);
	char szPose[4]; szPose[1] = 0;

	szPose[0] = sz[0]; _psprNum1->setState(szPose);
	szPose[0] = sz[1]; _psprNum2->setState(szPose);
	szPose[0] = sz[2]; _psprNum3->setState(szPose);

	_stars = stars;

	_locked = locked;
	BZSpriteButton* pbtnBack = (BZSpriteButton*)_psprBack;
	pbtnBack->enable(!locked);
	if (locked)
	{
		_psprLock->setVisible(true);
		_psprStar1->setVisible(false);
		_psprStar2->setVisible(false);
		_psprStar3->setVisible(false);
	}
	else
	{
		_psprLock->setVisible(false);
		if (_stars >= 1) _psprStar1->setState("on"); else _psprStar1->setState("off"); 
		if (_stars >= 2) _psprStar2->setState("on"); else _psprStar2->setState("off"); 
		if (_stars >= 3) _psprStar3->setState("on"); else _psprStar3->setState("off"); 
	}
}

void BZButtonLevelItem::movePosition(float delta)
{
	CCSize size = CAWorld::getScreenSize();
	
	float offset = delta * size.width;

	_ptBack.x += offset; 
	_ptStar1.x += offset; 
	_ptStar2.x += offset; 
	_ptStar3.x += offset; 
	_ptNum1.x += offset; 
	_ptNum2.x += offset; 
	_ptNum3.x += offset; 
	_ptLock.x += offset; 
}

//[0,3)
void BZButtonLevelItem::setOffset(float delta)
{
	CCSize size = CAWorld::getScreenSize();
	
	float offset = delta * size.width;

	//_Info("set p=%.4f", _ptBack.x + offset);

	_psprBack->setPos	(_ptBack.x + offset, _ptBack.y);
	_psprStar1->setPos	(_ptStar1.x + offset, _ptStar1.y);
	_psprStar2->setPos	(_ptStar2.x + offset, _ptStar2.y);
	_psprStar3->setPos	(_ptStar3.x + offset, _ptStar3.y);
	_psprNum1->setPos	(_ptNum1.x + offset, _ptNum1.y);
	_psprNum2->setPos	(_ptNum2.x + offset, _ptNum2.y);
	_psprNum3->setPos	(_ptNum3.x + offset, _ptNum3.y);
	_psprLock->setPos	(_ptLock.x + offset, _ptLock.y);
}

BZStagePlayLayerEndlessLevel::BZStagePlayLayerEndlessLevel(CAStage* pstage, CAStageLayer* playerParent) : BZStageLayerCommon(pstage, playerParent)
{
	_Trace("%s allocated", __FUNCTION__);
	_levels = 0;
	_page = -1;
	_ipage = -1;
	_state = ELS_Idle;
	memset(_stars, 0, sizeof(_stars));
}

BZStagePlayLayerEndlessLevel::~BZStagePlayLayerEndlessLevel(void)
{
}

void BZStagePlayLayerEndlessLevel::_onInit()
{
	BZStageLayerCommon::_onInit();

	_mode = CAWorld::sharedWorld()->gameenv().getString("mode");
	_difficulty = CAWorld::sharedWorld()->gameenv().getString("difficulty");
	
	_levels = BZWorld::getStars(_mode.c_str(), _difficulty.c_str(), _stars, SIZE_OF_ARRAY(_stars));
	if (_levels < 1) _levels = 1;
	if (_levels > MAX_LEVELS) _levels = MAX_LEVELS;

	int ip = (int)(_levels / (ITEMS_ROWS * ITEMS_COLS));
	_page = 0;
	_pageTo = (float)ip;
	_state = ELS_Adjusting;

	//initialize level items
	CCArray* psprsBack = this->_getNamedSprites("game_level_item_back");
	_Assert(psprsBack->count() == ITEMS_ROWS * ITEMS_COLS * PAGES);

	CCArray* psprsLock = this->_getNamedSprites("game_level_item_lock");
	_Assert(psprsLock->count() == ITEMS_ROWS * ITEMS_COLS * PAGES);

	CCArray* psprsStar1 = this->_getNamedSprites("game_level_item_star1");
	_Assert(psprsStar1->count() == ITEMS_ROWS * ITEMS_COLS * PAGES);

	CCArray* psprsStar2 = this->_getNamedSprites("game_level_item_star2");
	_Assert(psprsStar2->count() == ITEMS_ROWS * ITEMS_COLS * PAGES);

	CCArray* psprsStar3 = this->_getNamedSprites("game_level_item_star3");
	_Assert(psprsStar3->count() == ITEMS_ROWS * ITEMS_COLS * PAGES);

	CCArray* psprsNum1 = this->_getNamedSprites("game_level_item_num1");
	_Assert(psprsNum1->count() == ITEMS_ROWS * ITEMS_COLS * PAGES);

	CCArray* psprsNum2 = this->_getNamedSprites("game_level_item_num2");
	_Assert(psprsNum2->count() == ITEMS_ROWS * ITEMS_COLS * PAGES);

	CCArray* psprsNum3 = this->_getNamedSprites("game_level_item_num3");
	_Assert(psprsNum3->count() == ITEMS_ROWS * ITEMS_COLS * PAGES);

	//int blevel = ((int)_page) * ITEMS_ROWS * ITEMS_COLS;
	int blevel = 0;
	int level = blevel;
	int page, r, c;
	for (page = 0; page < PAGES; page++)
	{
		for (r = 0; r < ITEMS_ROWS; r++)
		{
			for (c = 0; c < ITEMS_COLS; c++)
			{
				int index = level - blevel;
				_levelItems[page][r][c].bindSprites(
					(CASprite*)psprsBack->objectAtIndex(index),
					(CASprite*)psprsStar1->objectAtIndex(index),
					(CASprite*)psprsStar2->objectAtIndex(index),
					(CASprite*)psprsStar3->objectAtIndex(index),
					(CASprite*)psprsNum1->objectAtIndex(index),
					(CASprite*)psprsNum2->objectAtIndex(index),
					(CASprite*)psprsNum3->objectAtIndex(index),
					(CASprite*)psprsLock->objectAtIndex(index)
					);
				_levelItems[page][r][c].setLevelInfo(blevel, level + 1, _stars[level + 1], level + 1 > _levels);
				level++;
			}
		}
	}
}

void BZStagePlayLayerEndlessLevel::_updatePage()
{
	int ipage = (int)(_page + 0.5f);
	if (ipage != _ipage)
	{
		//roll column items
		//re-set levels stars lock state, or invisible them
		_ipage = ipage;
	}
	//float delta = fmod(_page, 3.0f);
	float delta = _page;
	int page, r, c;
	for (page = 0; page < PAGES; page++)
	{
		for (r = 0; r < ITEMS_ROWS; r++)
		{
			for (c = 0; c < ITEMS_COLS; c++)
			{
				_levelItems[page][r][c].setOffset(-delta);
			}
		}
	}
}

void BZStagePlayLayerEndlessLevel::_onRunning()
{
	BZStageLayerCommon::_onRunning();
	postGameEvent("ui", "game_endless_select_level");
	_gesture.addEventListener(this);
	_gesture.setTimeProvider(this->stage());
}

void BZStagePlayLayerEndlessLevel::_onClean()
{
	int page, r, c;
	for (page = 0; page < PAGES; page++)
	{
		for (r = 0; r < ITEMS_ROWS; r++)
		{
			for (c = 0; c < ITEMS_COLS; c++)
			{
				_levelItems[page][r][c].bindSprites(null, null, null, null, null, null, null, null);
			}
		}
	}
	BZStageLayerCommon::_onClean();
}

void BZStagePlayLayerEndlessLevel::onUpdate()
{
	BZStageLayerCommon::onUpdate();
	if (CAMachine::getCurrentState()->getFullName() == "root.running")
	{
		_gesture.update();

		switch(_state)
		{
		case ELS_Draging:
			break;
		case ELS_Adjusting:
			{
				_page += (_pageTo - _page) / 3.5f;
				if (CAUtils::almostEqual(_page, _pageTo, 0.02f))
				{
					_page = _pageTo;
					_state = ELS_Idle;
				}
				_updatePage();
				//_Info("page=%.2f", _page);
			}
			break;
		case ELS_Idle:
			break;
		}
	}
}

void BZStagePlayLayerEndlessLevel::_onButtonCommand(CASprite* pbutton)
{
	BZStageCommon* pstage = (BZStageCommon*)this->stage();
	string btn = pbutton->getModName();
	if ("game_level_item_back" == btn)
	{
		int level = pbutton->env().getInteger("level");
		float dp = _page - _pageBase;
		_Info("STATE=%d, level %d selected, dp=%.2f", _state, level, dp);
		if (CAUtils::Absf(dp) < 0.015f)
		{
			CAWorld::sharedWorld()->gameenv().setString("mode", _mode);
			CAWorld::sharedWorld()->gameenv().setString("difficulty", _difficulty);
			CAWorld::sharedWorld()->gameenv().setInteger("levelfrom", level);
			string layer = "game_";
			layer += _mode;
			replaceLayer(layer.c_str());
		}
	}
	else if ("game_button_home" == btn)
	{
		replaceLayer("home");
	}
}

bool BZStagePlayLayerEndlessLevel::onEvent(const CAEvent* pevt)
{
	bool ret = BZStageLayerCommon::onEvent(pevt);

	switch (pevt->type())
	{
	case ET_Touch:
		if (CAMachine::getCurrentState()->getFullName() == "root.running")
		{
			CAEventTouch* ptouch = (CAEventTouch*)pevt;
			if (kTouchStateGrabbed == ptouch->state())
			{
				_pageBase = _page;
				_state = ELS_Draging;
			}
			_gesture.onTouch(ptouch);
		}
		break;
	case ET_Key:
		{
			CAEventKey* pek = (CAEventKey*)pevt;
			if (CAMachine::getCurrentState()->getFullName() == "root.running")
			{
				if (KE_Back == pek->key() /*|| KE_Menu == pek->key() */)
				{
					replaceLayer("home");
				}
			}
		}
		break;
	case ET_Gesture:
		{
			CAEventGesture* pges = (CAEventGesture*)pevt;
			const string& command = pges->command();
			const CATimedTouchPoint* pointFirst = pges->pointFirst();
			const CATimedTouchPoint* point = pges->point();
			if (command == GESTURE_Draging)
			{
				//_Info("delta = %.2f,%.2f", point->pt.x - pointFirst->pt.x, point->pt.y - pointFirst->pt.y);
				float dx = point->pt.x - pointFirst->pt.x;
				CCSize size = CAWorld::getScreenSize();
				_page = _pageBase + (-dx / (float)size.width);
				_updatePage();
				_Info("drgpage=%.2f curpage=%.2f", _pageBase, _page);
			}
			else if (command == GESTURE_Slithering)
			{
				//_Info("delta = %.2f,%.2f", point->pt.x - pointFirst->pt.x, point->pt.y - pointFirst->pt.y);
			}
			else if (command == GESTURE_DragingEnd)
			{
				_Info("delta = %.2f,%.2f, speed=%.2f,%.2f", point->pt.x - pointFirst->pt.x, point->pt.y - pointFirst->pt.y, point->xv, point->yv);
				_state = ELS_Adjusting;
				{
					CCSize size = CAWorld::getScreenSize();
					float dp = -point->xv / (float)size.width * 0.26f;
					float to = _page + dp;
					float f = floor(to);
					float c = ceil(to);

					if (to - f > 0.5f) 
					{
						_pageTo = c;
					}
					else
					{
						_pageTo = f;
					}
					if (_pageTo < 0)
					{
						postGameEvent("ui", "page_ad0");
						_pageTo = 0;
					}
					else if (_pageTo > PAGES - 1)
					{
						postGameEvent("ui", "page_ad1");
						_pageTo = PAGES - 1;
					}
				}
				_updatePage();
			}
		}
		break;
	case ET_Command:
		break;
	}
	return ret;
}

REG_CLASS(BZStagePlayLayerEndlessLevel);
