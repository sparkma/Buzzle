LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := game_frame_shared

LOCAL_MODULE_FILENAME := libgame_frame

CXXFLAGS="-Wno-psabi $CXXFLAGS"

LOCAL_SRC_FILES := ../game_frame.cpp \
                  ../../../libcacore/Classes/AAudio.cpp \
                  ../../../libcacore/Classes/ACache.cpp \
				   				../../../libcacore/Classes/ADebug.cpp \
				   				../../../libcacore/Classes/ADataBuf.cpp \
                  ../../../libcacore/Classes/AEnvironment.cpp \
                  ../../../libcacore/Classes/AEvent.cpp \
                  ../../../libcacore/Classes/AFixArray.cpp \
                  ../../../libcacore/Classes/AHashSHA.cpp \
                  ../../../libcacore/Classes/ASpritesPattern.cpp \
                  ../../../libcacore/Classes/AMachine.cpp \
				   				../../../libcacore/Classes/AMath2d.cpp \
				   				../../../libcacore/Classes/AMath3d.cpp \
                  ../../../libcacore/Classes/AObject.cpp \
                  ../../../libcacore/Classes/AAnimateObject.cpp \
                  ../../../libcacore/Classes/ASettings.cpp \
                  ../../../libcacore/Classes/ASprite.cpp \
                  ../../../libcacore/Classes/ASpriteContainer.cpp \
                  ../../../libcacore/Classes/ASpriteModel.cpp \
                  ../../../libcacore/Classes/AStage.cpp \
                  ../../../libcacore/Classes/AStageLayer.cpp \
                  ../../../libcacore/Classes/AState.cpp \
                  ../../../libcacore/Classes/AString.cpp \
                  ../../../libcacore/Classes/ATransition.cpp \
                  ../../../libcacore/Classes/AUtils.cpp \
                  ../../../libcacore/Classes/AValue.cpp \
				   				../../../libcacore/Classes/AUserData.cpp \
									../../../libcacore/Classes/AWorld.cpp \
									../../../libcacore/Classes/AXML.cpp \
									../../Classes/BZBlock.cpp \
									../../Classes/BZBubble.cpp \
									../../Classes/BZBoard.cpp \
									../../Classes/BZGame.cpp \
									../../Classes/BZGameClassic.cpp \
									../../Classes/BZGameMenu.cpp \
									../../Classes/BZGameMenuBar.cpp \
									../../Classes/BZGameTapBoom.cpp \
									../../Classes/BZGroupNumber.cpp \
									../../Classes/BZSpriteButton.cpp \
									../../Classes/BZSpriteCommon.cpp \
									../../Classes/BZStageCommon.cpp \
									../../Classes/BZStageLayerCommon.cpp \
									../../Classes/BZStageLogoLayerMain.cpp \
									../../Classes/BZStagePlayLayerDialog.cpp \
									../../Classes/BZStagePlayLayerDialogCollectionFailed.cpp \
									../../Classes/BZStagePlayLayerDialogCollectionPause.cpp \
									../../Classes/BZStagePlayLayerDialogCollectionSucc.cpp \
									../../Classes/BZStagePlayLayerDialogEndlessGameover.cpp \
									../../Classes/BZStagePlayLayerDialogEndlessPause.cpp \
									../../Classes/BZStagePlayLayerDialogHelp.cpp \
									../../Classes/BZStagePlayLayerHome.cpp \
									../../Classes/BZStagePlayLayerMenuCollection.cpp \
									../../Classes/BZStagePlayLayerMenuEndless.cpp \
									../../Classes/BZStagePlayLayerPlayCollection.cpp \
									../../Classes/BZStagePlayLayerPlayCollectionLogic.cpp \
									../../Classes/BZStagePlayLayerPlayEndless.cpp \
									../../Classes/BZStagePlayLayerPlayEndlessLogic.cpp \
									../../Classes/BZWorld.cpp \
									../../Classes/AppDelegate.cpp \


LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../libcacore/Classes                   

LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static cocosdenshion_static
            
include $(BUILD_SHARED_LIBRARY)

$(call import-module,CocosDenshion/android) $(call import-module,cocos2dx)
