#if !defined(_BZ_BUBBLE_H_)
#define _BZ_BUBBLE_H_

#include "AObject.h"
#include "BZSpriteCommon.h"

typedef enum enumBubbleState
{
	BS_NA,
	BS_NAing,
	//when block is craeted.
	BS_Born,
	BS_Borning,
	BS_Borned,
	//when bubble with spec prop gen
	BS_Gen,
	//when pose finished, nav to BS_Release
	BS_Gening,
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
	BS_BlockBlend,
	BS_BlockBlending,
	BS_PoseBlend,
	BS_PoseBlending,
	//when blending finished
	//this can go back to BS_Falling state
	BS_Stand,
	BS_Standing,
	//
	BS_Die,
	BS_Dying,
	BS_Died,
}
EBubbleState;

typedef enum enumBubbleNeighbour
{
	N_TOP	= 0,
	N_LEFT,
	N_BOTTOM,
	N_RIGHT,
}
EBubbleNeighbour;

typedef enum enumBubbleBlockerType 
{
	//there is a falling blocker
	BT_FallingBlock,
	//there is a stopped blocker
	BT_StoppingBlock,
	//we reach the bottom of the board
	BT_Bottom,
}
EBubbleBlockerType;

class BZBlock;
class BZBoard;

#define _ROW(a)		(((a) > 0) ? ((int)((a) + 0.5f)) : ((int)((a) - 0.5f)))
#define _COL(a)		_ROW(a)

class BZBubble : public CAObject
{
protected:
	BZBoard*	_pboard;
	BZBlock*	_pblock;

	int _debug_id;

	//bubble
	ccTime		_timeStateBegin;
	EBubbleState	_state;
	void _setState(EBubbleState s);
	string		_bubbleType;
	CASprite*	_psprBubble;
	string		_pose;

	//we will create this bubble on this pos
	CCPoint		_posPending;
	//virtual pos, block dim(1.0, 1.0)
	CCPoint		_pos;
	void _setPos(float x, float y);
	int			_row, _col;
	ccTime		_lastFallingTime;
	float		_acceleration;
	float		_fallingspeed;

	//prop
	string		_propType;
	CASprite*	_psprProp;

	string		_doodadType;
	CASprite*	_psprDoodad;

	//runtime vars
	bool		_bRainfallMode;

	bool		_reborn;
	string		_rebornBubble;
	string		_rebornProp;

	CASprite*	_psprDeadEffect;
public: 
	BZBubble(BZBoard* pboard);
	virtual ~BZBubble();

	void loadData(CADataBuf& data);
	void saveData(CADataBuf& data);

	int debug_id() const { return _debug_id; }

	BZBlock* getBlock() { return _pblock; }
	void setBlock(BZBlock* pblock);

	void initialize(const char* bubble, 
		const char* prop = null, 
		const char* doodad = null,
		float zorder = 50.0f);
	CASprite* getSpriteBubble() { return _psprBubble; }
	const string& getBubbleType() const { return _bubbleType; }
	const string& getPropType() const { return _propType;}
	
	void detach(CAStageLayer* player);

	void setState(EBubbleState s) { _setState(s); }
	EBubbleState getState() const { return _state; }
	bool isStoped() const;
	void setRainfallMode(bool b) { _bRainfallMode = b; }

	void setFallingAcceleration(float a);

	void setPose(const string& pose);

	//born pos or dragging pos
	inline void setInitialPosition(const CCPoint& p) { _setPos(p.x, p.y); }
	void setPendingPosition(const CCPoint& p) { _posPending = p; }
	const CCPoint& getPendingPosition() const { return _posPending; }

	inline void setDraggingPos(const CCPoint& p) { _setPos(p.x, p.y); }

	const CCPoint& getPos() const { return _pos; }
	//virtual void setPos(const CCPoint& pos);

	//void setIndexPos(int row, int col) { _row = row; _col = col; }
	int getIndexRow() const { return _row; }
	int getIndexColumn() const { return _col; }

	bool canMove() const;

	void addEffect(const char* spr, const char* pose, bool bDeadEffect = false);
	void setRebornBubble(const char* bubble, const char* prop);
	void try2Reborn();

	virtual void onUpdate();
};

#endif //_BZ_BUBBLE_H_
