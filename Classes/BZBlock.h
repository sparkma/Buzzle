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
public:
	BZBlock(BZBoard* pboard);
	virtual ~BZBlock();

	void append(BZBlock* pblock);
	void attachBubble(BZBlockBubble* pbubble);
	int getStars() const { return _stars; }

	CCArray* getBubbles() { return _bubbles; }

	void onBubblePositionChanged(BZBlockBubble* pbubble, const CCPoint& posOld, const CCPoint& posNew);

	void onUpdate();
};

#endif //_BZ_BLOCK_H_
