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
	int			_debug_id;

	BZBoard*	_pboard;
	CCArray*	_bubbles;
	string		_bubbleType;

	EBlockState	_state;

	mutable bool _bDirty;
	mutable int _standing;
	mutable int	_stars;
	mutable int _movables;
	mutable bool _hasSOB;

	int _counter;
	//BZBubble*	_pbubbleCenter;

	void _refresh() const;
public:
	BZBlock(BZBoard* pboard);
	virtual ~BZBlock();

	//when bubbles'state changed, they will call this function
	void setDirty(bool b) { _bDirty = b; }
	EBlockState getState() const { return _state; };

#if 0
	void loadData(CADataBuf& data);
	void saveData(CADataBuf& data);
#endif

	int debug_id() const { return _debug_id; }
	void verify();

	void clear();
	void reset();
	void append(BZBlock* pblock);

	void attachBubble(BZBubble* pbubble);
	void detachBubble(BZBubble* pbubble);

	const string& getBubbleType() const { return _bubbleType; }
	int getStars() const;
	//int getProps() const;

	void resetBubblesDoodad(const char* doodad, const char* pszPose);

	int getStandingCount() const;
	bool isAllStanding() const;
	//bool isOpen() const;
	void getStatus(int& bubbles, int& movables, int& stars, bool& hasSOB) const;

	CCArray* getBubbles() { return _bubbles; }

	BZBubble* booom();
	//BZBubble* getBoomCenterBubble() { return _pbubbleCenter; }

	virtual void onUpdate();

	float getMagneticForce(const string& bubbleType) const;
	//void onBubblePositionChanged(BZBubble* pbubble, const CCPoint& posOld, const CCPoint& posNew);
};

#endif //_BZ_BLOCK_H_
