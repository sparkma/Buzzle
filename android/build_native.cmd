rem set params
set NDK_ROOT=\_projects\android-ndk-r8
set COCOS2DX_ROOT=\_projects\cocos2d.home\

set GAME_ROOT=%~dp0
set GAME_ANDROID_ROOT=%GAME_ROOT%
set RESOURCE_ROOT=%GAME_ROOT%\..\Resources

rem make sure assets is exist
if exist %GAME_ANDROID_ROOT%\assets. (
    rm -rf %GAME_ANDROID_ROOT%\assets
)

mkdir %GAME_ANDROID_ROOT%\assets

xcopy /E %RESOURCE_ROOT%\* %GAME_ANDROID_ROOT%\assets\

echo "Building external dependencies from source"
%NDK_ROOT%\ndk-build.cmd -C %GAME_ANDROID_ROOT% NDK_MODULE_PATH=%COCOS2DX_ROOT%;%COCOS2DX_ROOT%\cocos2dx\platform\third_party\android\prebuilt

