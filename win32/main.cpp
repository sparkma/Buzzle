#include "main.h"
#include "AppDelegate.h"
#include "CCEGLView.h"
#include "AXML.h"

USING_NS_CC;

//#define _HRES_
#if defined(_HRES_)
#define DEVICE_WIDTH	960
#define DEVICE_HEIGHT	640
#else
#define DEVICE_WIDTH	(480 * 4.17f / 8)
#define DEVICE_HEIGHT	(800 * 4.17f / 8)
#endif


string getConfig()
{
	return "mg=bool:false";
}

void postGameEvent(const char* key, const char* value)
{
	//_Info("Game Event:%s=%s", key, value);
}

string getLanguage()
{
	return "cn";
}

CASettings _settings;
void _loadSettings(CAXMLNode* pnode)
{
	_Assert(pnode);
	_settings.initialize(pnode);
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

	CAXMLNode* pxml = CAXML::parse("game.world", false);
	_Assert(pxml);
	CAXMLNode* pnode;
	pnode = pxml->firstnode("world");
	_Assert(pnode);
	_loadSettings(pnode->firstnode("settings"));

    //CCEGLView& eglView = CCEGLView::sharedOpenGLView();
	CCEGLView& eglView = *CCEGLView::sharedOpenGLView();
    eglView.setViewName("CA.WIN32.View");
	float w = DEVICE_WIDTH;
	float h = DEVICE_HEIGHT;
	CCSize size = _settings.getSize("window");
	float scale = _settings.getFloat("window_scale", 1.0f);
	if (size.width > 0) w = size.width;
	if (size.height > 0) h = size.height;
	w *= scale; 
	h *= scale;
    eglView.setFrameSize(w, h);
    // set the design resolution screen size, if you want to use Design Resoulution scaled to current screen, please uncomment next line.
    // eglView.setDesignResolutionSize(480, 320);

    return CCApplication::sharedApplication()->run();
}
