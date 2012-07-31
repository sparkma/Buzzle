#if !defined(_BZGAME_LOGIC_H_)
#define _BZGAME_LOGIC_H_

#include "AObject.h"
#include "BZSpriteCommon.h"

typedef enum enumBlockState
{
	BS_NA,
	BS_NAing,
	//when block is craeted.
	BS_Born,
	BS_Borning,
	//when user release this block
	BS_Release,
	//when falling speed is non-zero
	BS_Fall,
	BS_Falling,
	//when user drag this block
	BS_Drag,
	BS_Dragging,
	//when falling speed is zero
	BS_Stop,
	BS_Stopping,
	//0.3s after block stoped 
	//BS_Blending,
	//when blending finished
	//this can go back to BS_Falling state
	BS_Stand,
	BS_Standing,
	//after boooooom, when pose finished, ==> BS_NA, killMyself
	BS_Die,
	BS_Dying,
}
EBlockState;

typedef enum enumBlockNeighbour
{
	N_TOP	= 0,
	N_LEFT,
	N_BOTTOM,
	N_RIGHT,
}
EBlockNeighbour;

class BZGroup;
class BZGame;

#define DEFAULT_ACCELERATION (10.0f)
class BZBlock : public CAObject
{
protected:
	BZGame*		_pgame;
	BZGroup*	_pgroup;

	ccTime		_timeStateBegin;
	EBlockState	_state;
	void _setState(EBlockState s);

	string		_pose;
	BZBlock*	_neighbours[4];

	//virtual pos, block dim(1.0, 1.0)
	CCPoint		_pos;
	void _setPos(float x, float y);

	int			_row, _col;
	
	CASprite*	_psprBlock;
	ccTime		_lastFallingTime;
	float		_acceleration;
	float		_fallingspeed;
public: 
	BZBlock(BZGame* pgame);
	virtual ~BZBlock();

	void setBlock(const char* block);
	
	void setState(EBlockState s) { _setState(s); }
	EBlockState getState() const { return _state; }

	void setFallingAcceleration(float a = DEFAULT_ACCELERATION);

	virtual void attachTo(CAStageLayer* pl);

	//when a neighbour is added, we will calculate the blending
	void setNeighbour(EBlockNeighbour bn, BZBlock* pblock);

	void setInitPos(const CCPoint& p) { _setPos(p.x, p.y); }
	const CCPoint& getPos() const { return _pos; }
	//virtual void setPos(const CCPoint& pos);

	//void setIndexPos(int row, int col) { _row = row; _col = col; }
	int getIndexRow() const { return _row; }
	int getIndexColumn() const { return _col; }

 	virtual void onEnter();
	virtual void onUpdate();
	virtual void onExit();
};

class BZBlockStar : public BZBlock
{
protected:
	CASprite*	_psprStar;
public:
	BZBlockStar(BZGame* pgame);
	virtual ~BZBlockStar();

	void setStar(const char* star);
};

class BZBlockDoodads : public BZBlock
{
protected:
	//bombX, timecounter, ...
	//this can move out of bounds of this block
	CASprite*	_psprDoodads;
public:
	BZBlockDoodads(BZGame* pgame);
	virtual ~BZBlockDoodads();

	void setDoodads(const char* doodads);
};

class BZBlockTimecounter : public BZBlock
{
protected:
	CASprite* _psprNumber[2];
public:
	BZBlockTimecounter();
	virtual ~BZBlockTimecounter();
	void setTimeCounter(const char* number, float secs);
};

//blended blocks in one group!
class BZGroup : public CAObject 
{
protected:
	int			_stars;
	CCArray*	_pblocks;
public:
	BZGroup();
	virtual ~BZGroup();

	void addBlock(BZBlock* pblock);
	void removeBlock(BZBlock* plock);

	bool canBoom();
	int getScores();
	void booom();
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
	ccTime		_fDelayBlockBorn;
	ccTime		_fRangeblockBorn;
	ccTime		_fDelayStar;
};

class BZGame : public CAObject
{
protected:
	CAStageLayer* _pLayer;

	int			_rows, _cols;

	//blocks stopped in game boards
	BZBlock**	_blocksInBoards;
	void		_setBlock(int r, int c, BZBlock* pblock)
	{
		_Assert(null == _getBlock(r, c));
		_Assert(null == pblock || ((pblock->getIndexRow() == r) && (pblock->getIndexColumn() == c)));
		_blocksInBoards[r * _cols + c] = pblock;
	}
	BZBlock*	_getBlock(int r, int c) 
	{ 
		if ((r >= 0 && r < _rows) && (c >= 0 && c < _cols))
			return _blocksInBoards[r * _cols + c]; 
		//out of bounds, returns null
		return null;
	}

	//all blocks here: borned, falling, stoped, dying
	//CCArray*	_blocks;
	CCArray*	_groups;

	//doodads, star, bomb, timecounter etc,.
	CCArray*	_psprDoodads;

	BZBlock* _createUnmanagedBlock(const char* type, const CCPoint& pt);
	CCPoint _getPositionOfBorn(int col);

	float	_timeLastBorn;
	void _doBornStrategy();
	void _doBlocksUpdate();

	string		_types[BLOCK_TYPES];
	BZLevelParams	_params;

	string		_lastBlock;
	int			_nScores;
public:
	BZGame(CAStageLayer* player, int rows, int cols);
	virtual ~BZGame();

	CAStageLayer* layer() { return _pLayer; };
	int getRows() const { return _rows; } 
	int getColumns() const { return _cols; } 

	float getTimeNow();

	//void setAnchor(const CCPoint& ptBorn) { _ptLeftTopBorn = ptBorn; }
	void setLevelParams(BZLevelParams& params) { _params = params; }

	bool refineBlockPosition(BZBlock* pblock, float& x, float& y);
	bool verifyBlock(BZBlock* pblock);
	bool canFalling(BZBlock* pblock);

	virtual void onBlockPositionChanged(BZBlock* pblock, const CCPoint& posOld, CCPoint& posNew);
	virtual void onBlockStateChanged(BZBlock* pblock, EBlockState state);
	virtual void onEnter();
	virtual void onUpdate();
	virtual void onExit();
};

class BZGameClassic : public BZGame
{
protected:
	int			_level;
	//[0] level 1 params
	//[1] level 128 params
	BZLevelParams	_params[2];
public:
	BZGameClassic(CAStageLayer* player);
	virtual ~BZGameClassic();
};

#endif //_BZGAME_LOGIC_H_
