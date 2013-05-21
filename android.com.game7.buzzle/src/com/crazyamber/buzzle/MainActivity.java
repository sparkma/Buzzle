/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
 ****************************************************************************/
package com.crazyamber.buzzle;

import java.lang.ref.WeakReference;

import org.cocos2dx.lib.Cocos2dxActivity;

import com.crazyamber.core.GameEnvHandler;
import com.wyh.framework.Constant;
import com.wyh.framework.SDK;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

public class MainActivity extends Cocos2dxActivity
{
	private static final int EVENT_Loaded = 0x0001;
	private static final int EVENT_Prepare = 0x0002;
	private static final int EVENT_Play = 0x0003;
	private static final int EVENT_Exit = 0x0004;
	private static final int EVENT_PageAD = 0x0005;
	private static final int EVENT_LevelUp = 0x0006;
	
	private static class _ADManagerHandler extends Handler
	{
		WeakReference<MainActivity> mActivity;  

		_ADManagerHandler(MainActivity activity) 
		{  
			mActivity = new WeakReference<MainActivity>(activity);  
		}  
		@Override
	    public void handleMessage(Message msg) 
		{
           MainActivity theActivity = mActivity.get();
           theActivity.onMessage(msg.what);
	    }
	};
	
	private boolean _ad_initialized = false;
	private int _ad_showd = 0;
	private int _levels_up = 0;
	
	//private int _state = EVENT_Prepare;
	private void onMessage(int what)
	{
		//_state = what;
		switch (what)
		{
		case EVENT_Loaded:
			break;
		case EVENT_Prepare:
			if (!_ad_initialized)
			{
				//this is tested, it ok! 
				try
				{
					SDK.init(this);
					//Log.d("SDK", "SDK.init");
					//SDK.showFullScreen(this);
					//Log.d("SDK", "SDK.showFullScreen");
					_showAD();
					//Log.d("SDK", "SDK.showad");
					//this.adRequest(CoreService.LEFT_BOTTOM, _google_adid);
					//this.adRequest(_adContainer, _google_adid);
					_ad_initialized = true;
				}
				catch (Throwable t)
				{
					_ad_initialized = false;
				}
			}
			break;
		case EVENT_Play:
			break;
		case EVENT_Exit:
			_onExit();
			break;
		case EVENT_PageAD:
			switch(_ad_showd)
			{
			case 0:
				_showPauseAD();
				break;
			case 1:
				_showFullscreenAD();
				break;
			}
			_ad_showd++;
			break;
		case EVENT_LevelUp:
			_levels_up++;
			if (_levels_up == 10)
			{
				_showPauseAD();
			}
			break;
		}
	}
	
	private Handler _adManageHandler = new _ADManagerHandler(this);
	
	private GameEnvHandler.GameEventListener _eventListener = new GameEnvHandler.GameEventListener()
	{
		@Override
		public void onEvent(String evt, String val)
		{
			if (evt.equals("play_state"))
			{
				//MobclickAgent.onEvent(MainActivity.this, val);
				if (val.equals("prepare"))
				{
					Message msg = new Message();
					msg.what = EVENT_Prepare;
					_adManageHandler.sendMessage(msg);
				}
				else if (val.equals("play"))
				{
					Message msg = new Message();
					msg.what = EVENT_Play;
					_adManageHandler.sendMessage(msg);
				}
				else if (val.equals("exit"))
				{
					Message msg = new Message();
					msg.what = EVENT_Exit;
					_adManageHandler.sendMessage(msg);
				}
				else if (val.equals("page_ad"))
				{
					Message msg = new Message();
					msg.what = EVENT_PageAD;
					_adManageHandler.sendMessage(msg);
				}
				else if (val.equals("levelup"))
				{
					Message msg = new Message();
					msg.what = EVENT_LevelUp;
					_adManageHandler.sendMessage(msg);
				}
			}
		}
	};
	
	protected void onCreate(Bundle bundle)
	{
		super.onCreate(bundle);
		
		GameEnvHandler.intialize(this, _eventListener);

		SDK.onCreate(this);
		//do not load ad here, we are loading data now
		//SDK.adRequest(this, Constant.CENTER_BOTTOM);
		//SDK.showFullScreen(this);
	}
	
	protected void onResume()
	{
		super.onResume();
		SDK.onResume(this);
	}

	protected void onPause()
	{
		SDK.onPause(this);
		super.onPause();
	}

	protected void onDestroy()
	{
		SDK.onDestroy(this);
		super.onDestroy();
	}

	protected void onStart()
	{
		super.onStart();
		SDK.onStart(this);
	}

	protected void onStop(Activity context)
	{
		SDK.onStop(this);
		super.onStop();
	}

	protected void _onExit()
	{
	    SDK.exit(this);
	}

	protected void _showAD()
	{
		SDK.adRequest(this, Constant.CENTER_BOTTOM);
	}
	
	protected void _showPauseAD()
	{
		SDK.popInterstitial(this); 
	}
	
	protected void _showFullscreenAD()
	{
		SDK.showFullScreen(this);
	}
	
	static
	{
		System.loadLibrary("game_frame");
	}
}
