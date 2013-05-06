#if !defined(_BZ_BUBBLE_H_)
#define _BZ_BUBBLE_H_

#include "AObject.h"
#include "BZSpriteCommon.h"

class BZLoopArray
{
protected:
	CAObject** _data;
	int _size;
	int _index;
public:
	int pos() const { return _index + _size * 100 - 1; }
	int size() const { return _size; }
	void init(int size)
	{
		_data = new CAObject* [size];
		memset(_data, 0, sizeof(CAObject*) * size);
		_size = size;
		_index = 0;
	}
	BZLoopArray(int size = 0)
	{
		_data = null; _size = 0; _index = 0;
		if (size) init(size);
	}
	void clear()
	{
		int i;
		for (i = 0; i < _size; i++)
		{
			if (_data[i])
			{
				_data[i]->release();
				_data[i] = null;
			}
		}
		_index = 0;
	}
	
	virtual ~BZLoopArray()
	{
		clear();
		delete [] _data;
		_data = null;
		_size = 0;
	}
	void push(CAObject* pobj)
	{
		if (_size <= 0)
			return;

		int pos = _index % _size;
		if (_data[pos])
		{
			_data[pos]->release();
			_data[pos] = null;
		}
		if (pobj)
		{
			pobj->retain();
			_data[pos] = pobj;
		}
		_index = pos + 1;
	}
	CAObject* operator[](int index)
	{
		int n = index % _size;
		return _data[n];
	}
};

class BZTimedPos : public CAObject
{
public:
	double time;
	float timeA;
	CCPoint pos;
public:
	BZTimedPos(double time_, float timeA_, const CCPoint& pos_)
	{
		time = time_;
		timeA = timeA_;
		pos = pos_;
		autorelease();
	}
};

#define LAYER_BUBBLE		1
#define LAYER_DOODADS		2
#define LAYER_PROPS			3
#define LAYER_G_EFFECTS		4
#define LAYER_P_EFFECTS		5

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
	BS_DragRelease,
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
	//out of bounds
	BT_Invalid,
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
	bool		_lock;
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
	float		_fallingSpeed;
	//CCPoint		_posMangnent;
	float		_fallingOffset;

	//prop
	string		_propType;
	CASprite*	_psprProp;
	CCPoint		_posProp;
	bool		_hasStar;

	string		_doodadType;
	CASprite*	_psprDoodad;
	CCPoint		_posDoodad;

	//runtime vars
	//bool		_bRainfallMode;

	bool		_reborn;
	string		_rebornBubble;
	string		_rebornProp;

	CASprite*	_psprDeadEffect;

	float _zorder(float delta);

	BZLoopArray	_dragingPositions;
	void _onBubbleRelease();
	void _onBubbleMagneticForceRefine();

	bool _trySetPos(const CCPoint& pos);
	bool _adjustPosition(CCPoint& pos);

	float _getMagneticForce(int r, int c);
public: 
	BZBubble(BZBoard* pboard);
	virtual ~BZBubble();

	//void loadData(CADataBuf& data);
	//void saveData(CADataBuf& data);

	int debug_id() const { return _debug_id; }
	static const char* state2str(EBubbleState s);

	BZBlock* getBlock() { return _pblock; }
	void setBlock(BZBlock* pblock);

	void initialize(const char* bubble, const char* prop = null, const char* doodad = null);
	void addDoodad(const char* doodad, const char* pszPose = null);

	CASprite* getSpriteBubble() { return _psprBubble; }
	const string& getBubbleType() const { return _bubbleType; }
	const string& getPropType() const { return _propType;}
	
	void detach(CAStageLayer* player);

	void lock(bool l) { _lock = l; }
	void setState(EBubbleState s) { _setState(s); }
	EBubbleState getState() const { return _state; }
	bool isStoped() const;
	//void setRainfallMode(bool b) { _bRainfallMode = b; }

	void setFallingAcceleration(float a);

	void setPose(const string& pose);

	//born pos or dragging pos
	inline void setInitialPosition(const CCPoint& p) { _setPos(p.x, p.y); }
	void setPendingPosition(const CCPoint& p) { _posPending = p; }
	const CCPoint& getPendingPosition() const { return _posPending; }
	void setDraggingPos(const CCPoint& p, double time);
	bool collidAdjust(const CCPoint& posFrom, CCPoint& posTo);
	inline void getBoundBox(CCRect& rect) const { getBoundBox(rect, _pos); }
	inline static void getBoundBox(CCRect& rect, const CCPoint& pos) { return rect.setRect(pos.x - 0.5f, pos.y - 0.5f, 1.0f, 1.0f); }

	const CCPoint& getPos() const { return _pos; }
	//virtual void setPos(const CCPoint& pos);

	//void setIndexPos(int row, int col) { _row = row; _col = col; }
	int getIndexRow() const { return _row; }
	int getIndexColumn() const { return _col; }

	bool canMove() const;
	bool hasStar() const;

	void addEffect(const char* spr, const char* pose, bool bDeadEffect = false);

	void setRebornBubble(const char* bubble, const char* prop);
	void try2Reborn();

	virtual void onUpdate();
};

#endif //_BZ_BUBBLE_H_
