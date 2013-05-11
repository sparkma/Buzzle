
#include "BZSpriteItemPiece.h"
#include "AStageLayer.h"

BZSpriteItemPiece::BZSpriteItemPiece(CAStageLayer* player, const char* name) : BZSpriteCommon(player, name)
{
}

BZSpriteItemPiece::~BZSpriteItemPiece(void)
{
}

void BZSpriteItemPiece::onStateChanged(const string& olds, const string& news)
{
	BZSpriteCommon::onStateChanged(olds, news);
}

void BZSpriteItemPiece::onUpdate()
{
	if (this->getState() == "fly")
	{
		BZSpriteCommon::onUpdate();
		return;	
	}
	BZSpriteCommon::onUpdate();
}

#include "AWorld.h"
REG_CLASS(BZSpriteItemPiece);
