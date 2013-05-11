#if !defined(_TSSPRITE_BUTTON_ITEM_H_)
#define _TSSPRITE_BUTTON_ITEM_H_
#include "BZSpriteButton.h"
#include "BZSpriteCommon.h"

class BZSpriteButtonItem : public BZSpriteButton
{
protected:
	CCArray* _pieces;
	string _difficulty;
	int _pcount;

	void _onPieceCountChanged();
	virtual void _onClick();
public:
	BZSpriteButtonItem(CAStageLayer* palyer = null, const char* name = null);
	virtual ~BZSpriteButtonItem(void);

	void setDifficulty(const string& diff);
	void addPiece(const string& name, const CCPoint& pos);

	virtual void onUpdate();
};

#endif //_TSSPRITE_BUTTON_ITEM_H_
