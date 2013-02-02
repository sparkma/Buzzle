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
	//available stars for open-blocks
	int		nAvailableStars;
	int		nRangeBubbleBorn;
	float	fPrebornLines;
};

class BZGame : public BZBoard
{
protected:
	string			_name;

	//BZBoard*		_pboard;

	string			_types[BLOCK_TYPES];
	int				_typesCount;

	//int				_nStarsUsed;
	BZLevelParams	_params;
	float			_timeLastBorn;
	string			_lastBubble;

	int _nScore;

	virtual void _doBornStrategy() {};
	virtual void _onScoreChanged() {};
	virtual void _onBubbleBoomedChanged() {}
	virtual void _onBlockBoomedChanged() {};
	virtual void _onComboChanged() {};
	virtual void _onLevelChanged() {};
	virtual bool _canBoom(BZBlock* pblock) const { return false; };

	virtual BZBubble* _onUpdateBlock(BZBlock* pblock);

	virtual void _onDetachBubbleSprite(BZBubble* pbubble);

	int _indexOfType(const char* type) const;
public:
	BZGame(CAStageLayer* player);
	virtual ~BZGame();

	//void loadData();
	//void saveData();

	virtual string debuglog();

	virtual void createBoard(const CCPoint& ptLeftBottom, int rows, int cols, float bubblesize);

	int getScore() const { return _nScore; }

	virtual void onBubbleClicked(BZBubble* pbubble) {};

	//void setAnchor(const CCPoint& ptBorn) { _ptLeftTopBorn = ptBorn; }
	void setLevelParams(BZLevelParams& params) { _params = params; }

	virtual bool isGameOver() const { return false; };

	virtual void clear();

	virtual void onEnter();
	virtual void onUpdate();
	virtual bool onEvent(const CAEvent* pevt);
	virtual void onExit();
};

#endif //_BZGAME_MODE_H_
