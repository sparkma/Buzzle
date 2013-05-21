#if !defined(_TSSPRITE_STAR_HOLDER_H_)
#define _TSSPRITE_STAR_HOLDER_H_

#include "BZSpriteCommon.h"
#include "BZGame.h"

typedef enum enumStarHolderState
{
	ESHS_Blank = 0,
	ESHS_Filling,
	ESHS_Filled,
}
EStarHolderState;

class BZSpriteStarHolder : public BZSpriteCommon
{
protected:
	EStarHolderState _item_state;

	void _setState(EStarHolderState s);
	EStarHolderState _getState() const { return _item_state; };

	CCArray* _pieces;

	float _timeFilling;
public:
	BZSpriteStarHolder(CAStageLayer* palyer = null, const char* name = null);
	virtual ~BZSpriteStarHolder(void);


	void reset();
	bool isBlank() const { return ESHS_Blank == _item_state; }
	void addStar(const string& name, const CCPoint& pos);

	virtual void onEnter();
	virtual void onUpdate();
};

#endif //_TSSPRITE_STAR_HOLDER_H_
