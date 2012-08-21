#if !defined(_BZGAME_MODE_H_)
#define _BZGAME_MODE_H_

#include "AObject.h"
#include "BZBoard.h"

#define BLOCK_TYPES		3
#define MAX_LEVELS		128

//	level		delay		range		star delay		
//	======		=======		=======		===========		
//	1			3			3			5
//	128			0.5			8			6
// this should be read from xml config file
class BZLevelParams
{
public:
	ccTime		_fDelayBlockBorn;
	ccTime		_fRangeblockBorn;
	ccTime		_fPercentStar;
};

class BZGame : public CAObject
{
protected:
	CAStageLayer* _pLayer;

	BZBoard*	_pboard;

	string			_types[BLOCK_TYPES];
	BZLevelParams	_params;
	float			_timeLastBorn;
	string			_lastBubble;

	virtual void _doBornStrategy() {};

	int			_nScores;
public:
	BZGame(CAStageLayer* player);
	virtual ~BZGame();

	virtual string debuglog();

	CAStageLayer* layer() { return _pLayer; };
	virtual void createBoard(const CCPoint& ptLeftTop, int rows, int cols, float bubblesize);

	ccTime getTimeNow() const;

	//void setAnchor(const CCPoint& ptBorn) { _ptLeftTopBorn = ptBorn; }
	void setLevelParams(BZLevelParams& params) { _params = params; }

	virtual void onEnter();
	virtual void onUpdate();
	virtual void onEvent(CAEvent* pevt);
	virtual void onExit();
};

class BZGameClassic : public BZGame
{
protected:
	int			_level;
	//[0] level 1 params
	//[1] level 128 params
	BZLevelParams	_paramsPreloaded[2];

	virtual void _doBornStrategy();
public:
	BZGameClassic(CAStageLayer* player);
	virtual ~BZGameClassic();
};

class BZGameTapBoom : public BZGame
{
protected:
	int			_level;
	//[0] level 1 params
	//[1] level 128 params
	BZLevelParams	_params[2];
public:
	BZGameTapBoom(CAStageLayer* player);
	virtual ~BZGameTapBoom();
};

typedef enum enumMenuItemState
{
	MIS_NA,
	MIS_Run,
	MIS_Running,
	MIS_Gone,
}
EMenuItemState;

//menu item with board
class BZMenuItem : public BZGame
{
protected:
	string			_bubbletype;
	CASprite*		_psprLabel;
	EMenuItemState	_state;

	virtual void _doBornStrategy();
public:
	BZMenuItem(CAStageLayer* player);
	virtual ~BZMenuItem();

	void initialize(const char* btype, const char* labelSprite);
	void createNow();
	
	virtual void onUpdate();
};

#endif //_BZGAME_MODE_H_
