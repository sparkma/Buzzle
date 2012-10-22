#include "main.h"
#include "AppDelegate.h"
#include "CCEGLView.h"

USING_NS_CC;

//#define _HRES_
#if defined(_HRES_)
#define DEVICE_WIDTH	960
#define DEVICE_HEIGHT	640
#else
#define DEVICE_WIDTH	(480 * 7 / 8)
#define DEVICE_HEIGHT	(800 * 7 / 8)
#endif


string getConfig()
{
	return "mg=bool:false";
}

void postGameEvent(const char* key, const char* value)
{
	_Info("Game Event:%s=%s", key, value);
}

string getLanguage()
{
	return "cn";
}


int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // create the application instance
    AppDelegate app;
    CCEGLView& eglView = CCEGLView::sharedOpenGLView();
    eglView.setViewName("CA.WIN32.View");
    eglView.setFrameSize(DEVICE_WIDTH, DEVICE_HEIGHT);
    // set the design resolution screen size, if you want to use Design Resoulution scaled to current screen, please uncomment next line.
    // eglView.setDesignResolutionSize(480, 320);

    return CCApplication::sharedApplication().run();
}
