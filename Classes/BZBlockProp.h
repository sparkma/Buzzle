#if !defined(_BZBLOCK_PROP_H_)
#define _BZBLOCK_PROP_H_

#include "AObject.h"
#include "BZSpriteCommon.h"

class BZBoard;
class BZBlock;
class BZBlockProp : public CAObject
{
protected:
	//this is star, bomb, ice-sprite, and so-on
	string		_type;
	BZBoard*	_pboard;
	BZBlock*	_pblock;
	CASprite*	_psprProp;
public: 
	BZBlockProp(BZBoard* pblock);
	virtual ~BZBlockProp();

	//init sprite with name
	void initialize(const char* prop);
	CASprite* getSpriteProp() { return _psprProp; }
	const string& type() const { return _type; }

	virtual void onEnter();
	virtual void onUpdate();
	virtual void onExit();
};

#endif //_BZBLOCK_PROP_H_
