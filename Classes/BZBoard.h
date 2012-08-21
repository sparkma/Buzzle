#if !defined(_BZBOARD_H_)
#define _BZBOARD_H_

#include "AObject.h"
#include "BZBlock.h"
#include "BZBubble.h"
#include "BZSpriteCommon.h"

#define _MAX_GRABBED_BLOCKS 4

class BZGame;
class BZBoard : public CAObject
{
protected:
	BZGame*		_pgame;
	
	int			_rows, _cols;
	//bubbles stopped in game boards
#if defined(_DEBUG)
	BZBubble*	_bubblesInBoards[16][16];
#else
	BZBubble**	_bubblesInBoards;
#endif
	void _setBubble(int r, int c, BZBubble* pbubble);
	BZBubble* _getBubble(int r, int c) const;
	BZBubble* _getBubbleByPoint(const CCPoint& pos);

	//we can grab 4 bubbles at same time
	BZBubble* _bubblesGrabbed[_MAX_GRABBED_BLOCKS];
	BZBubble* _getGrabbedBubble(int finger);
	void _setGrabbedBubble(int finger, BZBubble* pbubble);

	CCPoint		_ptLeftTop;
	float		_fBubbleSize;

	//bubble position to screen position
	void _bp2sp(CCPoint& pos) const;
	//screen position to bubble position
	void _sp2bp(CCPoint& pos) const;

	void _onTouchUngrabbed(CAEventTouch* ptouch);
	void _onTouchMoving(CAEventTouch* ptouch);
	void _onTouchGrabbed(CAEventTouch* ptouch);


	//all bubbles here: borned, falling, stoped, dying
	CCArray*	_blocksRunning;
	CCArray*	_blocksIdle;

	BZBlock* _newBlockHolder(BZBubble* pbubble);
	void _onUpdateBlock(BZBlock* pblock);

	//game doodads: bubble light
	//CCArray*	_psprDoodads;

	void _doPoseBlend(BZBubble* pbubble);
	void _doLeaveBlock(BZBubble* pbubble);
	void _doBlockBlend(BZBubble* pbubble);
public: 
	BZBoard(BZGame* pgame);
	virtual ~BZBoard();

	BZGame*	game() { return _pgame; }

	//for debugging
	virtual string debuglog();
	void verify();

	void setParams(const CCPoint& ptLeftTop, int rows, int cols, float bubblesize);
	int getRows() const { return _rows; } 
	int getColumns() const { return _cols; } 
	unsigned int getBubblesCount() const;
	int getEmptyBornSlots(int* slots, int scount) const;

	ccTime getTimeNow() const;

	bool verifyBubble(BZBubble* pbubble);

	virtual bool canFall(const BZBubble* pbubble) const;
	virtual bool canMove(const BZBubble* pbubble) const;

	inline void getBubbleRenderPos(CCPoint& pos) const { _bp2sp(pos); }
	virtual EBubbleBlockerType getBubbleBlocker(BZBubble* pbubble, CCPoint& pos);

	virtual void onBubblePositionChanged(BZBubble* pbubble, const CCPoint& posOld, const CCPoint& posNew);
	virtual void onBubbleStateChanged(BZBubble* pbubble, EBubbleState state);
	//virtual void onBlockStateChanged(BZBlock* pblock);

	BZBubble* createBubble(int row, int col, const char* bubble, const char* prop = null, const char* doodad = null);

	virtual void onEnter();
	virtual void onUpdate();
	virtual void onEvent(CAEvent* pevt);
	virtual void onExit();
};

#endif //_BZBOARD_H_
