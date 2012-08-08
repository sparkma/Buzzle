#if !defined(_BZDOODADS_H_)
#define _BZDOODADS_H_

#include "AObject.h"
#include "BZSpriteCommon.h"

class BZBlock;
class BZBoard;

class BZDoodad : public CAObject
{
protected:
	CASprite* _psprDoodad;
public: 
	BZDoodad();
	virtual ~BZDoodad();

	//init sprite with name
	void initialize(const char* doodad);
};

class BZBlockDoodad : public BZDoodad
{
public:
	BZBlockDoodad(BZBlock* pblock) {};
	virtual ~BZBlockDoodad() {};
};

class BZBoardDoodad : public BZDoodad
{
public:
	BZBoardDoodad(BZBoard* pboard) {};
	virtual ~BZBoardDoodad() {}
};

#endif //_BZDOODADS_H_
