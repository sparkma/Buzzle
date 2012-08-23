#if !defined(_BZGAME_MENUBAR_H_)
#define _BZGAME_MENUBAR_H_

#include "BZGame.h"

typedef enum enumMenuBarState
{
	MIS_NA,
	MIS_Run,
	MIS_Running,
	MIS_Standing,
	MIS_Booming,
	MIS_Gone,
}
EMenuBarState;

//menu item with board
class BZGameMenuBar : public BZGame
{
protected:
	string			_bubbletype;
	CASprite*		_psprLabel;
	EMenuBarState	_state;

	virtual void _doBornStrategy();
public:
	BZGameMenuBar(CAStageLayer* player);
	virtual ~BZGameMenuBar();

	void initialize(const char* btype, const char* labelSprite);
	void createNow();
	
	virtual void onBubbleClicked(BZBubble* pbubble);
	virtual void onUpdate();
};

#endif //_BZGAME_MENUBAR_H_
