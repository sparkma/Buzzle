#if !defined(_BZGAME_LOGIC_H_)
#define _BZGAME_LOGIC_H_

#include "AObject.h"
#include "BZSpriteCommon.h"

typedef enum enumBlockState
{
	BS_NA,
	BS_Born,
	BS_Falling,
	BS_Blending,
	BS_Idle,
	BS_Dying,
}
EBlockState;

class BZGroup : public CAObject 
{
};

class BZGame;
class BZBlock : public BZSpriteCommon
{
protected:
	BZGame*		_pgame;
	BZGroup*	_pgroup;
	EBlockState	_state;
public:
	BZBlock(BZGame* pgame);
	virtual ~BZBlock();
	//bool canCombinedWith(BZBlock* pblock);

	virtual void onEnter();
	virtual void onUpdate();
	virtual void onExit();
};

#define BLOCK_TYPES		3
#define MAX_LEVELS		128

//	level		delay		range		star delay		
//	======		=======		=======		===========		
//	1			3			3			5
//	128			0.5			8			6
class BZLevelParams
{
public:
	float		_fDelayBlockBorn;
	float		_fRangeblockBorn;
	float		_fDelayStar;
};

class BZGame : public CAObject
{
protected:
	CAStageLayer* _pLayer;
	int			_col, _row;
	CCPoint		_ptLeftTopBorn;
	CCPoint		_ptLeftTopBoard;

	CCArray*	_blocks;
	CCArray*	_groups;

	BZBlock* _createBlock(const char* type, const CCPoint& pt);

	int			_level;
	BZLevelParams	_params;

	string		_lastBlock;
public:
	BZGame(CAStageLayer* player);
	virtual ~BZGame();

	void setDim(int row, int col) { _row = row; _col = col; }
	void setAnchor(const CCPoint& ptBorn, const CCPoint& ptBoard)
	{ _ptLeftTopBorn = ptBorn; _ptLeftTopBoard = ptBoard; }
	void setLevelParams(BZLeveParams& params) 
	{ _params = params; }

	virtual void onEnter();
	virtual void onUpdate();
	virtual void onExit();
};

class BZGameClassic : public BZGame
{
protected:
	//[0] level 1 params
	//[1] level 128 params
	BZLevelParams	_params[2];
public:
	BZGameClassic(CAStageLayer* player);
	virtual ~BZGameClassic();
};

#endif //_BZGAME_LOGIC_H_
