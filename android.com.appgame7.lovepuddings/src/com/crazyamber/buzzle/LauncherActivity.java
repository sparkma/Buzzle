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

import java.util.List;

import com.appgame7.lovepuddings.R;
import com.crazyamber.core.UpdateManager;

import android.app.Activity;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.content.Intent.ShortcutIconResource;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.ProviderInfo;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;

public class LauncherActivity extends Activity
{
	private boolean _hasShortcutI(String url) 
	{ 
        String title = null;
        try 
        {
            final PackageManager pm = this.getPackageManager();
            title = pm.getApplicationLabel(
                    pm.getApplicationInfo(this.getPackageName(),
                            PackageManager.GET_META_DATA)).toString();
        } 
        catch (Exception e) 
        {
        }
        ContentResolver resolver = this.getContentResolver(); 
        Cursor cursor = resolver.query(Uri.parse(url), null, "title=?", 
                        new String[] {title }, null); 
 
        boolean has = false;
        if (cursor != null) 
        { 
        	has = cursor.moveToFirst();
        	cursor.close(); 
        } 
 
        return has; 
    }
	
	private String _getAuthorityFromPermission(Context context, String permission)
	{
	    if (permission == null) return null;
	    List<PackageInfo> packs = context.getPackageManager().getInstalledPackages(PackageManager.GET_PROVIDERS);
	    if (packs != null) 
	    {
	        for (PackageInfo pack : packs) 
	        { 
	            ProviderInfo[] providers = pack.providers; 
	            if (providers != null) 
	            { 
	                for (ProviderInfo provider : providers) 
	                { 
	                    if (permission.equals(provider.readPermission)) return provider.authority;
	                    if (permission.equals(provider.writePermission)) return provider.authority;
	                } 
	            }
	        }
	    }
	    return null;
	}
	
	private boolean _hasShortcut()
	{
		String url;
        String auth;
        
        auth = _getAuthorityFromPermission(this, "com.android.launcher.permission.READ_SETTINGS");
        url = "content://" + auth + "/favorites?notify=true";
        if (_hasShortcutI(url))
        	return true;

        //url = "content://com.android.launcher.settings/favorites?notify=true";
        //if (_hasShortcutI(url))
        //	return true;
        //url = "content://com.android.launcher2.settings/favorites?notify=true"; 
        //if (_hasShortcutI(url))
        //	return true;
        
        return false;
	}

	private void _addShortcut()
	{  
		//if (_hasShortcut())
		//	return;
		
        Intent shortcut = new Intent("com.android.launcher.action.INSTALL_SHORTCUT");  
              
        //快捷方式的名称  
        shortcut.putExtra(Intent.EXTRA_SHORTCUT_NAME, getString(R.string.app_name));  
        shortcut.putExtra("duplicate", false); //不允许重复创建  
              
        //指定当前的Activity为快捷方式启动的对象: 如 com.everest.video.VideoPlayer  
        //注意: ComponentName的第二个参数必须加上点号(.)，否则快捷方式无法启动相应程序  
        //ComponentName comp = new ComponentName(this.getPackageName(), "."+this.getLocalClassName()); 
        //shortcut.putExtra(Intent.EXTRA_SHORTCUT_INTENT, new Intent(Intent.ACTION_MAIN).setComponent(comp));
        
        shortcut.putExtra(Intent.EXTRA_SHORTCUT_INTENT, new Intent(this, LauncherActivity.class));  
      
        //快捷方式的图标  
        ShortcutIconResource iconRes = Intent.ShortcutIconResource.fromContext(this, R.drawable.icon);  
        shortcut.putExtra(Intent.EXTRA_SHORTCUT_ICON_RESOURCE, iconRes);  
              
        sendBroadcast(shortcut);  
    }  
	
	private boolean _firstRun()
	{
		SharedPreferences sharedPreferences = this.getSharedPreferences("runtime", MODE_PRIVATE);  
        
		boolean isFirstRun = sharedPreferences.getBoolean("isFirstRun", true);  
		Editor editor = sharedPreferences.edit();  
		  
		if (isFirstRun)  
		{  
		    //Log.d("debug", "第一次运行");  
		    editor.putBoolean("isFirstRun", false);  
		    editor.commit();  
		} else  
		{  
		    //Log.d("debug", "不是第一次运行");  
		}
		return isFirstRun;
	}
	
	protected void onCreate(Bundle bundle)
	{
		super.onCreate(bundle);
		if (UpdateManager.checkUpdate(this))
		{
			if (_firstRun()) 
				_addShortcut();
            Intent intent = new Intent(this, MainActivity.class);  
            startActivity(intent);   
            finish();
        }
	}
	
	protected void onResume()
	{
		super.onResume();
	}

	protected void onPause()
	{
		super.onPause();
	}

	protected void onDestroy()
	{
		super.onDestroy();
	}
	
	static
	{
		System.loadLibrary("game_frame");
	}
}
