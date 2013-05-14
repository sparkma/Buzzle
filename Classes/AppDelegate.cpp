#include "AppDelegate.h"

#include "cocos2d.h"
//#include "CCEGLView.h"
#include "SimpleAudioEngine.h"
using namespace CocosDenshion;

#include "AWorld.h"
#include "ACache.h"
#include "AStage.h"

USING_NS_CC;

AppDelegate::AppDelegate() 
{
	_InfoWhere();
}

AppDelegate::~AppDelegate() 
{
	CACache::sharedCache().release();
}

#if defined(_DEBUG) && 0
void _extractImage()
{
	const char* names[64] = { "main_bg", "mainmenu", "stage", "stagebg" };
	for (int i = 0; i < 4; i++)
	{
		string plist = string("temp/") + names[i] + ".plist";
		const char* pszPlist = plist.c_str();
		string png = string("temp/") + names[i] + ".png";
		string pathKey = png;

		const char *pszPath = CCFileUtils::sharedFileUtils()->fullPathFromRelativePath(pszPlist);
		CCDictionary *dictionary = CCDictionary::createWithContentsOfFileThreadSafe(pszPath);
		pathKey = CCFileUtils::sharedFileUtils()->fullPathFromRelativePath(pathKey.c_str());

		CCImage image;                
        CCImage::EImageFormat eImageFormat = CCImage::kFmtUnKnown;
        eImageFormat = CCImage::kFmtPng;
		unsigned long nSize = 0;
		unsigned char* pBuffer = CCFileUtils::sharedFileUtils()->getFileData(pathKey.c_str(), "rb", &nSize);
		CC_BREAK_IF(! image.initWithImageData((void*)pBuffer, nSize, eImageFormat));
		//::delete [] pBuffer;
		//CC_SAFE_DELETE_ARRAY(pBuffer);

			
		/*
		Supported Zwoptex Formats:

		ZWTCoordinatesFormatOptionXMLLegacy = 0, // Flash Version
		ZWTCoordinatesFormatOptionXML1_0 = 1, // Desktop Version 0.0 - 0.4b
		ZWTCoordinatesFormatOptionXML1_1 = 2, // Desktop Version 1.0.0 - 1.0.1
		ZWTCoordinatesFormatOptionXML1_2 = 3, // Desktop Version 1.0.2+
		*/

		CCDictionary *metadataDict = (CCDictionary*)dictionary->objectForKey("metadata");
		CCDictionary *framesDict = (CCDictionary*)dictionary->objectForKey("frames");
		int format = 0;

		// get the format
		if(metadataDict != NULL) 
		{
			format = metadataDict->valueForKey("format")->intValue();
		}

		// check the format
		CCAssert(format >=0 && format <= 3, "format is not supported for CCSpriteFrameCache addSpriteFramesWithDictionary:textureFilename:");

		CCDictElement* pElement = NULL;
		CCDICT_FOREACH(framesDict, pElement)
		{
			CCDictionary* frameDict = (CCDictionary*)pElement->getObject();
			std::string spriteFrameName = pElement->getStrKey();
	        
			CCRect rect;
			CCRect orect;
			bool roted = false;

			if(format == 0) 
			{
				float x = frameDict->valueForKey("x")->floatValue();
				float y = frameDict->valueForKey("y")->floatValue();
				float w = frameDict->valueForKey("width")->floatValue();
				float h = frameDict->valueForKey("height")->floatValue();
				float ox = frameDict->valueForKey("offsetX")->floatValue();
				float oy = frameDict->valueForKey("offsetY")->floatValue();
				int ow = frameDict->valueForKey("originalWidth")->intValue();
				int oh = frameDict->valueForKey("originalHeight")->intValue();
				// check ow/oh
				if(!ow || !oh)
				{
					CCLOGWARN("cocos2d: WARNING: originalWidth/Height not found on the CCSpriteFrame. AnchorPoint won't work as expected. Regenrate the .plist");
				}
				// abs ow/oh
				ow = abs(ow);
				oh = abs(oh);
				// create frame
				rect = CCRectMake(x, y, w, h);
				roted = false;
				orect = CCRectMake(ox, oy, (float)ow, (float)oh);
			} 
			else if(format == 1 || format == 2) 
			{
				CCRect frame = CCRectFromString(frameDict->valueForKey("frame")->getCString());
				bool rotated = false;

				// rotation
				if (format == 2)
				{
					rotated = frameDict->valueForKey("rotated")->boolValue();
				}

				CCPoint offset = CCPointFromString(frameDict->valueForKey("offset")->getCString());
				CCSize sourceSize = CCSizeFromString(frameDict->valueForKey("sourceSize")->getCString());

				// create frame
				rect = frame;
				roted = rotated;
				orect = CCRectMake(offset.x, offset.y, sourceSize.width, sourceSize.height);
			} 
			else if (format == 3)
			{
				// get values
				CCSize spriteSize = CCSizeFromString(frameDict->valueForKey("spriteSize")->getCString());
				CCPoint spriteOffset = CCPointFromString(frameDict->valueForKey("spriteOffset")->getCString());
				CCSize spriteSourceSize = CCSizeFromString(frameDict->valueForKey("spriteSourceSize")->getCString());
				CCRect textureRect = CCRectFromString(frameDict->valueForKey("textureRect")->getCString());
				bool textureRotated = frameDict->valueForKey("textureRotated")->boolValue();

				// get aliases
				CCArray* aliases = (CCArray*) (frameDict->objectForKey("aliases"));
				CCString * frameKey = new CCString(spriteFrameName);

				CCObject* pObj = NULL;
				CCARRAY_FOREACH(aliases, pObj)
				{
					std::string oneAlias = ((CCString*)pObj)->getCString();
					//CCLOGWARN("cocos2d: WARNING: an alias with name %s already exists", oneAlias.c_str());
				}
				frameKey->release();
				// create frame
				rect = CCRectMake(textureRect.origin.x, textureRect.origin.y, spriteSize.width, spriteSize.height);
				roted = textureRotated;
				orect = CCRectMake(spriteOffset.x, spriteOffset.y, spriteSourceSize.width, spriteSourceSize.height);
			}

			//rect, roted, orect are ready
			CCImage imageSub;
			CCImage::EImageFormat eImageFormatSub = CCImage::kFmtUnKnown;
			eImageFormatSub = CCImage::kFmtRawData;
			
			int bpp = image.getBitsPerComponent() / 8;
			if (image.hasAlpha()) bpp *= 4;
			else bpp *= 3;

			int w = (int)rect.size.width;
			int h = (int)rect.size.height;
			int iw = image.getWidth();
			int ih = image.getHeight();
			unsigned char* pbData = image.getData();
			unsigned char* pTempData = new unsigned char [w * h * 4];
            for (int y = 0; y < h; y++)
            {
                for (int x = 0; x < w; x++)
                {
					for (int bb = 0; bb < bpp; bb++)
					{
						pTempData[(y * w + x) * 4 + bb] = pbData[((y + (int)rect.origin.y) * iw + x + (int)rect.origin.x) * bpp + bb];
						if (bpp < 4) pTempData[(y * w + x) * 4 + 3] = 0xff;
					}
                }
            }
			imageSub.initWithImageData(pTempData, w * h, eImageFormatSub, w, h, 8);
			delete [] pTempData;

			string name = string("temp/") + names[i];
			CreateDirectoryA(name.c_str(), null);
			name += "/";
			name += spriteFrameName;
			name = CCFileUtils::sharedFileUtils()->fullPathFromRelativePath(name.c_str());
			imageSub.saveToFile(name.c_str(), false);
	    }
	}
}
#endif

bool AppDelegate::applicationDidFinishLaunching() {
	GUARD_FUNCTION();

	_InfoWhere();

	//const GLubyte* pb = glGetString(GL_VENDOR);

	//int a = CCConfiguration::sharedConfiguration()->getMaxTextureSize();

	_Info("get director");
	// initialize director
	CCDirector *pDirector = CCDirector::sharedDirector();

	_Info("set director ogl view");
	pDirector->setOpenGLView(CCEGLView::sharedOpenGLView());
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
    
	//DO NOT use cc stats
	//pDirector->setDisplayStats(true);

	// pDirector->setDeviceOrientation(kCCDeviceOrientationLandscapeLeft);
    //CCDirector::sharedDirector()->setProjection(kCCDirectorProjection3D);

#if defined(_DEBUG) && 0
	_extractImage();
#endif

	_Info("initialize world");
	_world.initialize();
	_Info("read world setting:entry");
	string start = _world.settings().getString("entry");
	_Info("create stage:%s", start.c_str());
	CAStage* pstage = _world.createStage(start.c_str());
	_Info("load stage:%s", start.c_str());
	while (SLS_Finished != pstage->loadProgressively());
	_Info("switch stage:%s", start.c_str());
	_world.switchStage(pstage);

	return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
	CCDirector::sharedDirector()->pause();

	_world.dispatchEvent(SE_Pause);
	// if you use SimpleAudioEngine, it must be pause
    //_engine->pauseBackgroundMusic();
	//_engine->pauseAllEffects();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
	CCDirector::sharedDirector()->resume();

	_world.dispatchEvent(SE_Resume);
	// if you use SimpleAudioEngine, it must resume here
    //_engine->resumeBackgroundMusic();
	//_engine->resumeAllEffects();
}


