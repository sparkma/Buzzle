
#include "BZStagePlayLayerDialog.h"
#include "AStage.h"
#include "AString.h"

BZStagePlayLayerDialog::BZStagePlayLayerDialog(CAStage* pstage, CAStageLayer* playerParent) : BZStageLayerCommon(pstage, playerParent)
{
	_Trace("%s allocated", __FUNCTION__);
}

BZStagePlayLayerDialog::~BZStagePlayLayerDialog(void)
{
	_Trace("%s destroyed", __FUNCTION__);
}

void BZStagePlayLayerDialog::doBack(const char* backTo)
{
	_backTo = backTo;
	this->hide();
}

void BZStagePlayLayerDialog::_onClean()
{
	//where should I go? _from
	_Assert(_backTo.length() > 0);
	BZStageLayerCommon* pl = (BZStageLayerCommon*)_pstage->getSubLayer(_backTo);
	pl->resume();
}

#include "AWorld.h"
REG_CLASS(BZStagePlayLayerDialog);
