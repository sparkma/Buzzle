package com.crazyamber.core;

import android.content.Context;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;

public class Settings
{
	public static void set(Context c, String key, String val)
	{
		SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(c);
		sp.edit().putString(key, val).commit();
	}

	public static String get(Context c, String key)
	{
		SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(c);
		String val = sp.getString(key, "");
		return val;
	}
}
