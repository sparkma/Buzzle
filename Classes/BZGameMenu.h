#if !defined(_BZGAME_MENU_H_)
#define _BZGAME_MENU_H_

#include "AObject.h"
#include "BZGameMenuBar.h"

class IBZGameMenuListener
{
public:
	virtual void onBarClicked(const string& id) = 0;
};

class CAStageLayer;

class BZGameMenu : public CAObject
{
protected:
	CAStageLayer* _pLayer;
	CAStringMap<BZGameMenuBar>* _bars;
	IBZGameMenuListener* _plistener;
public:
	BZGameMenu(CAStageLayer* player, IBZGameMenuListener* plistener);
	virtual ~BZGameMenu();

	void onBarClicked(BZGameMenuBar* pbar);
	void addBar(const char*id, const char* label, float labscale, const char* bubbletype,
				const CCPoint& pos, int cols, float bubblesize, float zorder);

	virtual void onUpdate();
	virtual void onEvent(CAEvent* pevt);
};

#endif //_BZGAME_MENU_H_
