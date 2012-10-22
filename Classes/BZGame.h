#if !defined(_BZGAME_MODE_H_)
#define _BZGAME_MODE_H_

#include "AObject.h"
#include "BZBoard.h"

#define BLOCK_TYPES		5
#define MAX_LEVELS		128

//	level		delay		range		star delay		
//	======		=======		=======		===========		
//	1			3			3			5
//	128			0.5			8			6
// this should be read from xml config file
class BZLevelParams
{
public:
	float	fDelayOneRow;
	float	timeDelayBorn;
	int		nRangeBubbleBorn;
	float	fPercentStarBorn;
	int		nMinStarsInOneBubbleType;
};

class BZGame : public CAObject
{
protected:
	CAStageLayer*	_pLayer;
	string			_name;

	BZBoard*		_pboard;

	string			_types[BLOCK_TYPES];
	int				_nStarsUsed;
	BZLevelParams	_params;
	float			_timeLastBorn;
	string			_lastBubble;

	int				_nLevel;
	int				_nScore;
	int				_nCombo;

	virtual void _doBornStrategy() {};
	virtual void _onScoreChanged() {};
	virtual void _onBubbleBoomedChanged() {}
	virtual void _onBlockBoomedChanged() {};
	virtual void _onComboChanged() {};
	virtual void _onLevelChanged() {};
	virtual bool _canBoom(BZBlock* pblock) const { return false; };
public:
	BZGame(CAStageLayer* player);
	virtual ~BZGame();

	void loadData();
	void saveData();

	virtual string debuglog();

	CAStageLayer* layer() { return _pLayer; };
	virtual void createBoard(const CCPoint& ptLeftBottom, 
		int rows, int cols, float bubblesize, float zorder);

	ccTime getTimeNow() const;
	virtual bool boomBlock(BZBlock* pblock) ;

	int getLevel() const { return _nLevel; }
	int getScore() const { return _nScore; }
	virtual float getLevelPercent() const { return 0; };

	virtual void onBubbleClicked(BZBubble* pbubble) {};

	//void setAnchor(const CCPoint& ptBorn) { _ptLeftTopBorn = ptBorn; }
	void setLevelParams(BZLevelParams& params) { _params = params; }

	virtual bool canShowBoomScore() const { return false; }
	virtual int calculateScore(BZBlock* pblock) const { return 0; };

	void clear();

	virtual void onEnter();
	virtual void onUpdate();
	virtual void onEvent(CAEvent* pevt);
	virtual void onExit();
};

#endif //_BZGAME_MODE_H_
