#if !defined(_BZBOARD_H_)
#define _BZBOARD_H_

#include "AObject.h"
#include "BZBlock.h"
#include "BZBubble.h"
#include "BZSpriteCommon.h"

//base on board layer Z
#define VZ_BUBBLE			1
#define VZ_DOODADS			2
#define VZ_PROPS			3
#define VZ_G_EFFECTS		4
#define VZ_P_EFFECTS		5
#define VZ_DYN_SCORE		7
#define VZ_FLY_ITEM			8

#define VZ_DIALOG_PAUSE		20
#define VZ_DIALOG_GAMEOVER	20
#define VZ_DIALOG_LEVELUP	20

class BZGame;
class BZBoard : public CAObject, public CAEventDispatcher
{
protected:
	//BZGame*		_pgame;
	CAStageLayer*	_pLayer;
	//float			_baseZ; 
	
	//float		_zorder;
	int			_rows, _cols;
#define _IS_IN_BOARD(_row_, _col_)	(((_row_) >= 0 && (_row_) < _rows) && ((_col_) >= 0 && (_col_) < _cols))
	bool _isInBoard(BZBubble* pb) const { return _IS_IN_BOARD(pb->getIndexRow(), pb->getIndexColumn()); }

	//bubbles stopped in game boards
	BZBubble*	_aryBubblesBorned[32];
	void _setBornBubble(int col, BZBubble* pbubble);
	BZBubble* _getBornBubble(int col) const;
	void _removeFromBornedLine(BZBubble* pbubble);
#if defined(_DEBUG)
	BZBubble*	_bubblesInBoards[32][32];
#else
	BZBubble**	_bubblesInBoards;
#endif
	void _setBubble(int r, int c, BZBubble* pbubble);
	BZBubble* _getBubble(int r, int c) const;
	BZBubble* _getBubbleByPoint(const CCPoint& pos);
	BZBubble* _bindBubble(BZBubble* pb, const CCPoint& pos, BZBlock* pholder, EBubbleState state);

	CCPoint		_ptLeftBottom;
	float		_fBubbleSize;

	//bubble position to screen position
	void _bp2sp(CCPoint& pos) const;
	//screen position to bubble position
	void _sp2bp(CCPoint& pos) const;

	//all bubbles here: borned, falling, stoped, dying
	CCArray*	_blocksRunning;
	CCArray*	_blocksIdle;

	BZBlock* _newBlockHolder();

	//if booomed, return center bubble
	virtual BZBubble*  _onUpdateBlock(BZBlock* pblock) = 0;

	//game doodads: bubble light
	//CCArray*	_psprDoodads;

	virtual void _doPoseBlend(BZBubble* pbubble);
	virtual void _doLeaveBlock(BZBubble* pbubble);
	virtual void _doBlockBlend(BZBubble* pbubble);
	virtual void _doBubbleDied(BZBubble* pbubble);

	virtual void _onDetachBubbleSprite(BZBubble* pbubble) = 0;
public: 
	BZBoard(CAStageLayer* player);
	virtual ~BZBoard();

	//BZGame*	game() { return _pgame; }
	CAStageLayer* layer() { return _pLayer; };
	ccTime getTimeNow() const;

	//void loadData(CADataBuf& data);
	//void saveData(CADataBuf& data);
	
	//for debugging
	virtual string debuglog();
	void verify();

	void setParams(const CCPoint& ptBoardAnchor, int rows, int cols, float bubblesize);
	int getRows() const { return _rows; } 
	int getColumns() const { return _cols; } 
	unsigned int getBubblesCount() const;
	unsigned int getStarsCount(const char* type = null) const;
	//void getCounts(int& bubblecount, int& blockcount, int& stars, int& props) const;

	int getEmptyBornSlots(int* slots, int scount) const;
	int getBornBubbles(BZBubble** ppbubbles, int count) const;

	float getBaseZOrder() const;

	//ccTime getTimeNow() const;

	bool verifyBubble(BZBubble* pbubble);

	virtual bool canFall(const BZBubble* pbubble) const;
	virtual bool canMove(const BZBubble* pbubble) const;
	bool isHeaderLineFull() const;
	void fallOneRow();

	inline float getBubbleSize() const { return _fBubbleSize; }
	void getBubbleRenderPos(CCPoint& pos) const;
	virtual EBubbleBlockerType getBubbleBlocker(int r, int c, CCPoint& pos, BZBubble** ppBubble = null);
	BZBubble* getBubble(int r, int c) const { return _getBubble(r, c); };

	virtual void onBubblePositionChanged(BZBubble* pbubble, const CCPoint& posOld, const CCPoint& posNew);
	virtual void onBubbleStateChanged(BZBubble* pbubble, EBubbleState state);
	virtual void onBlockStarsChanged(BZBlock* pblock, int stars) {};
	//virtual void onBlockStateChanged(BZBlock* pblock);

	BZBubble* createBubble1(const char* bubble, const CCPoint& pos, const char* prop = null, const char* doodad = null, BZBlock* pholder = null);
	BZBubble* createBornBubble(const char* bubble, int col, const char* prop = null, const char* doodad = null, BZBlock* pholder = null);
#if 0
	void pushPropBubble(const CCPoint& pos, const char* type, const char* prop);
#endif

	BZSpriteCommon* addGlobalEffect(const CCPoint& pos, const char* effect, const char* pose);
	int getEffectedBlock(BZBubble* pbCheck, int r, BZBubble** pbEffected, int esize);

	bool hasBeenOccupied(int r, int c, BZBubble* pbExclude);

	virtual void clear();

	virtual void onEnter();
	virtual void onUpdate();
	virtual bool onEvent(const CAEvent* pevt);
	virtual void onExit();
};

#endif //_BZBOARD_H_
