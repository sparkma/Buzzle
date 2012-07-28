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

#if defined(_CC_OLD_)
bool AppDelegate::initInstance() {
	GUARD_FUNCTION();

	bool bRet = false;
	do {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

		// Initialize OpenGLView instance, that release by CCDirector when application terminate.
		// The CrazyAmberGame is designed as HVGA.
		CCEGLView * pMainWnd = new CCEGLView();
		CC_BREAK_IF(! pMainWnd
				|| ! pMainWnd->Create(TEXT("Crazy Amber Game Engine Framework"), DEVICE_WIDTH, DEVICE_HEIGHT));

#endif  // CC_PLATFORM_WIN32
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

		// OpenGLView initialized in testsAppDelegate.mm on ios platform, nothing need to do here.

#endif  // CC_PLATFORM_IOS
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

		// OpenGLView initialized in CrazyAmberGame/android/jni/helloworld/main.cpp
		// the default setting is to create a fullscreen view
		// if you want to use auto-scale, please enable view->create(DEVICE_WIDTH,DEVICE_HEIGHT) in main.cpp
		// if the resources under '/sdcard" or other writeable path, set it.
		// warning: the audio source should in assets/
		// cocos2d::CCFileUtils::setResourcePath("/sdcard");

#endif  // CC_PLATFORM_ANDROID
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WOPHONE)

		// Initialize OpenGLView instance, that release by CCDirector when application terminate.
		// The CrazyAmberGame is designed as HVGA.
		CCEGLView* pMainWnd = new CCEGLView(this);
		CC_BREAK_IF(! pMainWnd || ! pMainWnd->Create(DEVICE_WIDTH,DEVICE_HEIGHT, WM_WINDOW_ROTATE_MODE_CW));

#ifndef _TRANZDA_VM_  
		// on wophone emulator, we copy resources files to Work7/NEWPLUS/TDA_DATA/Data/ folder instead of zip file
		cocos2d::CCFileUtils::setResource("CrazyAmberGame.zip");
#endif

#endif  // CC_PLATFORM_WOPHONE
#if (CC_TARGET_PLATFORM == CC_PLATFORM_MARMALADE)
		// MaxAksenov said it's NOT a very elegant solution. I agree, haha
		CCDirector::sharedDirector()->setDeviceOrientation(kCCDeviceOrientationLandscapeLeft);
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)

		// Initialize OpenGLView instance, that release by CCDirector when application terminate.
		// The CrazyAmberGame is designed as HVGA.
		CCEGLView * pMainWnd = new CCEGLView();
		CC_BREAK_IF(! pMainWnd
				|| ! pMainWnd->Create("cocos2d: Hello World", DEVICE_HEIGHT, DEVICE_WIDTH ,DEVICE_HEIGHT, DEVICE_WIDTH));

		CCFileUtils::setResourcePath("../Resources/");

#endif  // CC_PLATFORM_LINUX

#if (CC_TARGET_PLATFORM == CC_PLATFORM_BADA)

		CCEGLView * pMainWnd = new CCEGLView();
		CC_BREAK_IF(! pMainWnd|| ! pMainWnd->Create(this, DEVICE_HEIGHT, DEVICE_WIDTH));
		pMainWnd->setDeviceOrientation(Osp::Ui::ORIENTATION_LANDSCAPE);
		CCFileUtils::setResourcePath("/Res/");

#endif  // CC_PLATFORM_BADA

#if (CC_TARGET_PLATFORM == CC_PLATFORM_QNX)
		CCEGLView * pMainWnd = new CCEGLView();
		CC_BREAK_IF(! pMainWnd|| ! pMainWnd->Create(1024, 600));
		CCFileUtils::setResourcePath("app/native/Resources");
#endif // CC_PLATFORM_QNX
		bRet = true;
	} while (0);
	return bRet;
}
#endif //defined(_CC_OLD_)


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


