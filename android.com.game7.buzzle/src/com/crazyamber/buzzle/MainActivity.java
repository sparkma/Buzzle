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
import com.crazyamber.core.Utils;
import com.umeng.analytics.MobclickAgent;
import com.wyh.framework.Constant;
import com.wyh.framework.SDK;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

public class MainActivity extends Cocos2dxActivity
{
	private static final int GAME_EVENT = 0x0001;
	
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
           theActivity.onMessage(msg);
	    }
	};
	
	private boolean _ad_initialized = false;
	private int _ad_showd = 0;
	private int _levels_up = 0;
	
//	postGameEvent("ui", "page_ad0");
//	postGameEvent("ui", "page_ad1");
//	postGameEvent("ui", "home");
//	postGameEvent("ui", "game_endless");
//	postGameEvent("ui", "game_endless_select_level");
//	postGameEvent("ui", "exit");
//	postGameEvent("gameover", szLevel);
//	postGameEvent("levelup", szLevel);
//	postGameEvent("level_stars", szStars);
//	postGameEvent("prop_piece", difficulty + "-" + "prop_boom|samecolor|changecolor";
//	postGameEvent("prop_use", difficulty + "-" + "prop_boom|samecolor|changecolor";

	private void _onLevelResult(String difficulty, int level, int stars)
	{
		String ss;
		if (stars < 0) ss = "sf";
		else if (stars == 0) ss = "s0";
		else if (stars == 1) ss = "s1";
		else if (stars == 2) ss = "s2";
		else ss = "s3";
		String mval0 = String.format("%s-%d-%s", difficulty, level, ss);
		MobclickAgent.onEvent(MainActivity.this, mval0);

		level /= 10;
		level *= 10;
		if (stars < 0) ss = "sf";
		else if (stars >= 0 && stars <= 2) ss = "s012";
		else ss = "s3";
		String mval = String.format("%s-%d-%s", difficulty, level, ss);
		MobclickAgent.onEvent(MainActivity.this, mval);
		Log.d("MA", mval);
	}
	
	private void onMessage(Message msg)
	{
		if (msg.what != GAME_EVENT)
			return;
		
		String str = (String)msg.obj;
		Log.d("MA", str);
		
		String[] kvs = str.split(":");
		if (2 != kvs.length)
			return;
		String evt = kvs[0];
		String val = kvs[1];
		if ("ui".equals(evt))
		{
			if (val.equals("page_ad0") || val.equals("page_ad1"))
			{
				int n = _ad_showd % 5;
				switch(n)
				{
				case 0:
					_showPauseAD();
					break;
				case 1:
					_showFullscreenAD();
					break;
				}
				_ad_showd++;
			}
			else if (val.equals("home"))
			{
				//_ad_showd = 0;
				if (!_ad_initialized)
				{
					//this is tested, it ok! 
					try
					{
						SDK.init(this);
						//Log.d("SDK", "SDK.init");
						//SDK.showFullScreen(this);
						//Log.d("SDK", "SDK.showFullScreen");
						//	_showAD();
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
				_showAD();
			}
			else if (val.equals("game_endless_select_level"))
			{
			}
			else if (val.equals("game_endless"))
			{
			}
			else if (val.equals("exit"))
			{
				_onExit();
			}
		}
		else if ("levelup".equals(evt))
		{
			_levels_up++;
			if (_levels_up == 10)
			{
				_levels_up = 0;
				_showPauseAD();
			}
		}
		else if ("level_stars".equals(evt))
		{
			String[] items = val.split("-");
			if (items.length == 3)
			{
				_onLevelResult(items[0], Utils.parseInt(items[1]), Utils.parseInt(items[2]));
			}
		}
		else if ("prop_use".equals(evt))
		{
			String[] items = val.split("-");
			if (items.length == 2)
			{
				String mval = String.format("%s_prop_use-%s", items[0], items[1]);
				MobclickAgent.onEvent(MainActivity.this, mval);
				Log.d("MA", mval);
			}
		}
		else if ("prop_piece".equals(evt))
		{
			String[] items = val.split("-");
			if (items.length == 2)
			{
				String mval = String.format("%s_prop_piece-%s", items[0], items[1]);
				Log.d("MA", mval);
				MobclickAgent.onEvent(MainActivity.this, mval);
			}
		}
	}
	
	private Handler _adManageHandler = new _ADManagerHandler(this);

	private GameEnvHandler.GameEventListener _eventListener = new GameEnvHandler.GameEventListener()
	{
		@Override
		public void onEvent(String evt, String val)
		{
			Message msg = new Message();
			msg.what = GAME_EVENT;
			msg.obj = evt + ":" + val;
			_adManageHandler.sendMessage(msg);
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
}

