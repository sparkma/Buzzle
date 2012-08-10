#if !defined(_BZ_BLOCK_H_)
#define _BZ_BLOCK_H_

#include "AObject.h"

class BZBoard;
class BZBlockBubble;

//blended bubbles in one group!
class BZBlock : public CAObject 
{
protected:
	BZBoard*	_pboard;
	CCArray*	_bubbles;
	int			_stars;

	int _debug_id;
public:
	BZBlock(BZBoard* pboard);
	virtual ~BZBlock();

	int debug_id() const { return _debug_id; }
	void verify();

	void append(BZBlock* pblock);

	void attachBubble(BZBlockBubble* pbubble);
	void detachBubble(BZBlockBubble* pbubble);

	int getStars() const { return _stars; }

	CCArray* getBubbles() { return _bubbles; }

	void onBubblePositionChanged(BZBlockBubble* pbubble, const CCPoint& posOld, const CCPoint& posNew);

	void onUpdate();
};

#endif //_BZ_BLOCK_H_
