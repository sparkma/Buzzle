#if !defined(_BZBOARD_H_)
#define _BZBOARD_H_

#include "AObject.h"
#include "BZBlock.h"
#include "BZBlockBubble.h"
#include "BZBlockProp.h"
#include "BZDoodad.h"
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
	BZBlockBubble*	_bubblesInBoards[32][32];
#else
	BZBlockBubble**	_bubblesInBoards;
#endif
	void _setBubble(int r, int c, BZBlockBubble* pbubble);
	BZBlockBubble* _getBubble(int r, int c) const;
	BZBlockBubble* _getBubbleByPoint(const CCPoint& pos);

	//we can grab 4 bubbles at same time
	BZBlockBubble* _bubblesGrabbed[_MAX_GRABBED_BLOCKS];
	BZBlockBubble* _getGrabbedBubble(int finger);
	void _setGrabbedBubble(int finger, BZBlockBubble* pbubble);

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
#if defined(_BBBBUF_)
	CCArray*	_bubbles;
#endif
	CCArray*	_blocks;

	//game doodads: bubble light
	//CCArray*	_psprDoodads;
public: 
	BZBoard(BZGame* pgame);
	virtual ~BZBoard();

	BZGame*	game() { return _pgame; }

	void setParams(const CCPoint& ptLeftTop, int rows, int cols, float bubblesize);
	int getRows() const { return _rows; } 
	int getColumns() const { return _cols; } 
	unsigned int getBubblesCount() const;
	int getEmptyBornSlots(int* slots, int scount) const;

	ccTime getTimeNow() const;

	bool verifyBubble(BZBlockBubble* pbubble);

	inline void getBubbleRenderPos(CCPoint& pos) const { _bp2sp(pos); }
	EBubbleBlockerType getBubbleBlocker(BZBlockBubble* pbubble, CCPoint& pos);

	virtual void onBubblePositionChanged(BZBlockBubble* pbubble, const CCPoint& pos);
	virtual void onBubbleStateChanged(BZBlockBubble* pbubble, EBubbleState state);

	BZBlockBubble* createBubble(const char* bubble, int row, int col);
	BZBlockProp* attachProp(BZBlockBubble* pbubble, const char* prop);
	BZDoodad* attachDoodad(BZBlockBubble* pbubble, const char* doodad);

	virtual void onEnter();
	virtual void onUpdate();
	virtual void onEvent(CAEvent* pevt);
	virtual void onExit();
};

#endif //_BZBOARD_H_
