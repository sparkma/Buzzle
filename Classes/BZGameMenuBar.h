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

class BZGameMenu;
//menu item with board
class BZGameMenuBar : public BZGame
{
protected:
	BZGameMenu*		_parent;
	string			_id;
	string			_bubbletype;
	CASprite*		_psprLabel;
	EMenuBarState	_state;
	float			_zorderLabel;

	virtual void _doBornStrategy();
public:
	BZGameMenuBar(const char* id, CAStageLayer* player, BZGameMenu* pmenu);
	virtual ~BZGameMenuBar();

	const string& id() const { return _id; }
	void initialize(const char* label, float labscale, const char* btype);
	virtual void createBoard(const CCPoint& ptLeftBottom, 
		int rows, int cols, float bubblesize, float zorder);
	void createNow();
	
	virtual void onBubbleClicked(BZBubble* pbubble);
	virtual void onUpdate();
};

#endif //_BZGAME_MENUBAR_H_
