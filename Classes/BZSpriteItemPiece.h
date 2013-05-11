#if !defined(_TSSPRITE_ITEM_P_H_)
#define _TSSPRITE_ITEM_P_H_
#include "BZSpriteCommon.h"

class BZSpriteItemPiece : public BZSpriteCommon
{
protected:
public:
	BZSpriteItemPiece(CAStageLayer* palyer = null, const char* name = null);
	virtual ~BZSpriteItemPiece(void);

	void onStateChanged(const string& olds, const string& news);
	virtual void onUpdate();
};

#endif //_TSSPRITE_ITEM_P_H_
