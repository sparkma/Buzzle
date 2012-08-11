#include "AppDelegate.h"

#include "cocos2d.h"
//#include "CCEGLView.h"
#include "SimpleAudioEngine.h"
using namespace CocosDenshion;

#include "AWorld.h"
#include "ACache.h"
#include "AStage.h"

#if defined(_DEBUG)
//#	define _TEST_
#endif

#if defined(_TEST_)
#define START_STAGE "test"
#else
#define START_STAGE "logo/logo"
#endif

USING_NS_CC;

AppDelegate::AppDelegate() 
{

}

AppDelegate::~AppDelegate() 
{
	CACache::sharedCache().release();
}

bool AppDelegate::applicationDidFinishLaunching() {
	GUARD_FUNCTION();

	//const GLubyte* pb = glGetString(GL_VENDOR);

	//int a = CCConfiguration::sharedConfiguration()->getMaxTextureSize();

	// initialize director
	CCDirector *pDirector = CCDirector::sharedDirector();

	pDirector->setOpenGLView(&CCEGLView::sharedOpenGLView());
    pDirector->setProjection(kCCDirectorProjection2D);    // 这里加上

	//for vertexZ
	pDirector->setDepthTest(true);

	// enable High Resource Mode(2x, such as iphone4) and maintains low resource on other devices.
//     pDirector->enableRetinaDisplay(true);

#if defined(_DEBUG)
#if defined(_CC_OLD_)
// turn on display FPS
	pDirector->setDisplayFPS(true);
#endif
#endif
    pDirector->setDisplayStats(true);
	// pDirector->setDeviceOrientation(kCCDeviceOrientationLandscapeLeft);
    //CCDirector::sharedDirector()->setProjection(kCCDirectorProjection3D);

	_world.initialize();
	CAStage* pstage = _world.createStage(START_STAGE);
	while (SLS_Finished != pstage->loadProgressively());
	_world.switchStage(pstage);

	return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
	CCDirector::sharedDirector()->pause();

	_world.dispatchEvent(SE_Pause);
	// if you use SimpleAudioEngine, it must be pause
    //SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
	//SimpleAudioEngine::sharedEngine()->pauseAllEffects();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
	CCDirector::sharedDirector()->resume();

	_world.dispatchEvent(SE_Resume);
	// if you use SimpleAudioEngine, it must resume here
    //SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
	//SimpleAudioEngine::sharedEngine()->resumeAllEffects();
}


