
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

void BZStagePlayLayerDialog::doBack(const char* backTo, const char* result)
{
	_backTo = backTo;
	_result = result;
	this->hide();
}

void BZStagePlayLayerDialog::_onClean()
{
	//where should I go? _from
	_Assert(_backTo.length() > 0);
	BZStageLayerCommon* pl = (BZStageLayerCommon*)_pstage->getSubLayer(_backTo);
	pl->resume();
	pl->setDialogResult(_result);
}

#include "AWorld.h"
REG_CLASS(BZStagePlayLayerDialog);
