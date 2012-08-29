#if !defined(_BZ_BLOCK_H_)
#define _BZ_BLOCK_H_

#include "AObject.h"

class BZBoard;
class BZBubble;

typedef enum enumBlockState
{
	Block_NA,
	Block_Running,
	Block_Boooming,
	Block_Died,
}
EBlockState;

//blended bubbles in one group!
class BZBlock : public CAObject 
{
protected:
	BZBoard*	_pboard;

	CCArray*	_bubbles;

	EBlockState	_state;
	bool		_bDirty;
	mutable int _standing;

	int			_stars;
	int			_debug_id;
public:
	BZBlock(BZBoard* pboard);
	virtual ~BZBlock();

	void setDirty(bool b) { _bDirty = b; }
	EBlockState getState() const { return _state; };

	void loadData(const CADataBuf& data);
	void saveData(CADataBuf& data);
	
	int debug_id() const { return _debug_id; }
	void verify();

	void reset();
	void append(BZBlock* pblock);

	void attachBubble(BZBubble* pbubble);
	void detachBubble(BZBubble* pbubble);

	int getStars() const { return _stars; }
	int getStandingCount() const;
	bool isAllStanding() const;

	CCArray* getBubbles() { return _bubbles; }

	void booom();

	//void onBubblePositionChanged(BZBubble* pbubble, const CCPoint& posOld, const CCPoint& posNew);
};

#endif //_BZ_BLOCK_H_
