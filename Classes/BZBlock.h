#if !defined(_BZ_BLOCK_H_)
#define _BZ_BLOCK_H_

#include "AObject.h"

class BZBoard;
class BZBlockBubble;
class BZBlockProp;
class BZDoodad;

//blended bubbles in one group!
class BZBlock : public CAObject 
{
protected:
	BZBoard*	_pboard;

	CCArray*	_bubbles;
	CCArray*	_props;
	CCArray*	_doodads;
public:
	BZBlock(BZBoard* pboard);
	virtual ~BZBlock();

	void append(BZBlock* pblock);

	void attachBubble(BZBlockBubble* pbubble);
	void attachProp(BZBlockProp* pprop);
	void attachDoodad(BZDoodad* pdoodad);

	CCArray* getBubbles() { return _bubbles; }
	CCArray* getProps() { return _props; }
	CCArray* getDoodads() { return _doodads; }

	void onUpdate();
};

#endif //_BZ_BLOCK_H_
