#if !defined(_STAGE_PLAY_LAYER_ENDLESS_LEVEL_H_)
#define _STAGE_PLAY_LAYER_ENDLESS_LEVEL_H_

#include "BZStagePlayLayerDialog.h"
#include "BZSpriteButton.h"
#include "BZSpriteCommon.h"

class BZButtonLevelItem
{
protected:
	int _beginlevel;
	int _level;
	int _stars;
	bool _locked;

	CASprite* _psprBack;	CCPoint _ptBack;
	CASprite* _psprStar1;	CCPoint _ptStar1;
	CASprite* _psprStar2;	CCPoint _ptStar2;
	CASprite* _psprStar3;	CCPoint _ptStar3;
	CASprite* _psprNum1;	CCPoint _ptNum1;
	CASprite* _psprNum2;	CCPoint _ptNum2;
	CASprite* _psprNum3;	CCPoint _ptNum3;
	CASprite* _psprLock;	CCPoint _ptLock;
public:
	BZButtonLevelItem();
	virtual ~BZButtonLevelItem(void);

	void bindSprites(
		CASprite* psprBack, 
		CASprite* psprStar1, 
		CASprite* psprStar2, 
		CASprite* psprStar3, 
		CASprite* psprNum1, 
		CASprite* psprNum2, 
		CASprite* psprNum3, 
		CASprite* psprLock);

	void setLevelInfo(int beginlevel, int level, int stars, bool locked);

	void movePosition(float delta);
	//[0, 3.0)
	void setOffset(float delta);
};


/*
1	get current level, read settings, calcuate current page index.
2	create level items of 3 pages
3	drag, release, base the end point, use gesture to update buttons position
4	stop if it is nav to new page, create&destroy some level items
*/

#define PAGES 5
class BZStagePlayLayerEndlessLevel :
	public BZStageLayerCommon
{
protected:
	string	_mode;
	string	_difficulty;
	int		_levels;
	int		_stars[512];
	float	_page;
	float	_pageTo;
	float	_pageBase;
	int		_ipage;

#define ITEMS_COLS	3
#define ITEMS_ROWS	4
	BZButtonLevelItem _levelItems[PAGES][ITEMS_ROWS][ITEMS_COLS];

	virtual void _onClean();
	virtual void _onInit();
	virtual void _onRunning();
	virtual void _onButtonCommand(CASprite* pbutton);

	void _updatePage();

typedef enum enumELState
{
	ELS_Idle,
	ELS_Draging,
	ELS_Adjusting,
}
EELState;
	int _state;

	CAGestureGenarator _gesture;
public:
	BZStagePlayLayerEndlessLevel(CAStage* pstage = null, CAStageLayer* playerParent = null);
	virtual ~BZStagePlayLayerEndlessLevel(void);

	virtual void onUpdate();

	bool onEvent(const CAEvent* pevt);
};

#endif //_STAGE_PLAY_LAYER_ENDLESS_LEVEL_H_