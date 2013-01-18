#include "AppDelegate.h"
#include "cocos2d.h"
#include "platform/android/jni/JniHelper.h"
#include "CCEventType.h"
#include <jni.h>
#include <android/log.h>

#define  LOG_TAG    "main"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

using namespace cocos2d;
//using namespace cocos2d::extension;

string getConfig()
{
	string ret = "";
	JniMethodInfo t;
	if (JniHelper::getStaticMethodInfo(t
		, "com/crazyamber/core/GameEnvHandler"
		, "getConfig"
		, "()Ljava/lang/String;"))
	{

		jstring result = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID);
		ret = JniHelper::jstring2string(result);
		//LOGD("Jave 1::getConfig returns:%s", ret.c_str());
		t.env->DeleteLocalRef(t.classID);
		//LOGD("Jave 2::getConfig returns:%s", ret.c_str());
	}
	else
	{
		LOGD("can not get static function:getConfig");
	}
	return ret;
}

string getLanguage()
{
	string ret = "";
	JniMethodInfo t;
	if (JniHelper::getStaticMethodInfo(t
		, "com/crazyamber/core/GameEnvHandler"
		, "getLanguage"
		, "()Ljava/lang/String;"))
	{

		jstring result = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID);
		ret = JniHelper::jstring2string(result);
		LOGD("Jave 1::getLanugage returns:%s", ret.c_str());
		t.env->DeleteLocalRef(t.classID);
		LOGD("Jave 2::getLanugage returns:%s", ret.c_str());
	}
	else
	{
		LOGD("can not get static function:getLanugage");
	}
	return ret;
}

void postGameEvent(const char* key, const char* value)
{
	if (null == key || null == value)
	{
		return;
	}

	LOGD("postGameEvent %s=%s", key, value);

	JniMethodInfo t;
	if (JniHelper::getStaticMethodInfo(t
		, "com/crazyamber/core/GameEnvHandler"
		, "onJNIEvent"
		, "(Ljava/lang/String;Ljava/lang/String;)V"))
	{
		jstring stringArg1 = t.env->NewStringUTF(key);
		jstring stringArg2 = t.env->NewStringUTF(value);
		t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg1, stringArg2);
		t.env->DeleteLocalRef(stringArg1);
		t.env->DeleteLocalRef(stringArg2);
		t.env->DeleteLocalRef(t.classID);
	}
	else
	{
		LOGD("can not get static function");
	}
}


extern "C"
{

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    JniHelper::setJavaVM(vm);

    return JNI_VERSION_1_4;
}

void Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeInit(JNIEnv*  env, jobject thiz, jint w, jint h)
{
	_InfoWhere();

	if (!CCDirector::sharedDirector()->getOpenGLView())
    {
        CCEGLView *view = CCEGLView::sharedOpenGLView();
        view->setFrameSize(w, h);
        // set the design resolution screen size, if you want to use Design Resoulution scaled to current screen, please uncomment next line.
        // view->setDesignResolutionSize(480, 320);

        AppDelegate *pAppDelegate = new AppDelegate();
        CCApplication::sharedApplication()->run();
    }
    else
    {
        ccDrawInit();
        ccGLInvalidateStateCache();
        
        CCShaderCache::sharedShaderCache()->reloadDefaultShaders();
        CCTextureCache::reloadAllTextures();
        CCNotificationCenter::sharedNotificationCenter()->postNotification(EVNET_COME_TO_FOREGROUND, NULL);
        CCDirector::sharedDirector()->setGLDefaultValues(); 
    }
}

}
