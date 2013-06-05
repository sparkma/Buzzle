package com.crazyamber.core;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.Calendar;
import java.util.Date;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.telephony.TelephonyManager;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.LinearLayout.LayoutParams;

import com.appgame7.lovepuddings.R;

public class UpdateManager
{
	private static final int MSG_Downloading = 1;
	private static final int MSG_Downloaded = 2;
	private static final int MSG_DownloadCanceld = 3;

	private static File _apkFile;
	private static String _downloadedFile;
	private static String _date;
	private static int _days;
	private static int _mask;
	private static String _newVersionFileUrl;

	private static boolean _updateProgressIsCanceled = false;

	private static Activity _activity;
	private static ProgressBar _progressBar;
	private static Dialog _downloadDialog;

	private static Handler mHandler = new Handler()
	{
		public void handleMessage(Message msg)
		{
			switch (msg.what)
			{
			case MSG_Downloading:
				_progressBar.setProgress(msg.arg1);
				break;
			case MSG_Downloaded:
				Utils.installApk(_activity, _downloadedFile);
				_activity.finish();
				break;
			case MSG_DownloadCanceld:
				if (null != _apkFile) _apkFile.delete();
				_activity.finish();
				break;
			default:
				break;
			}
		};
	};

	//crazy
	//private static String _url_config = "f3d894b47ddb3a98c86f217b2f159382b96e3fa61c2a3765c7d872f6d170ea0a2acd8966354d9b5bf7a9e33fb97d235a510a246a99a71936482673daa7df414f";
	//lige
	private static String _url_config = "3956f1143ee4a1dc45cdb3f52643cacf929b51e1ec14b68eb033a4d2d23e204b3d201c672dc466eada2105dd732ec114af1f05dead32728fac27305a63d6610e";
	
	private static SimpleCipher _c = new SimpleCipher(
			Utils.hexToBytes("37ca9a53c87eaa66f8fe88444f02a876"), 
			Utils.hexToBytes("a3bb37149550b256009d23fc34b85836"));
	
	private static File _createOutputFile(File folder, String fname)
	{
		if (!folder.exists())
		{
			folder.mkdirs();
			if (!folder.exists())
			{
				return null;
			}
		}
		File file = new File(folder, fname);
		if (file.exists())
		{
			file.delete();
		}
		try
		{
			file.createNewFile();
		}
		catch (IOException e)
		{
			file = null;
		}
		
		return file;
	}
	
	public static Boolean  checkUpdate(Activity c)
	{
		_activity = c;
		int versionCode = Utils.getPackageVersionCode(_activity);

		/*
		JSONObject jo = new JSONObject();
		try
		{
			jo.put("title", "Love Puddings");
			jo.put("msg", "new version");
			jo.put("verCode", "953");
			jo.put("url", "http://app.lige.cc/upgrade/android/com.game7.buzzle_googleplay_0.9.53.apk");
			String js = jo.toString();
			js += "end";
		}
		catch (JSONException e1)
		{
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		
		String ue;
		ue = _c.encryptString("http://www.crazyamber.com/upgrade/android/buzzle.status");
		ue = _c.encryptString("http://app.lige.cc/upgrade/android/buzzle.status");
		ue = _c.encryptString("http://update.crazyamber.com/android/buzzle.status");
		*/
		
		String ru = _c.decryptString(_url_config);
		
		String serverText = Utils.dowloadTextFile(ru);
		//JSONArray array;
		int newVerCode = -1;
		String title = "";
		String msg = "";
		if (serverText.length() > 0)
		{
			try
			{
				//array = new JSONArray(serverText);
		        //JSONObject jsonObj = array.getJSONObject(0);
				JSONObject jsonObj = new JSONObject(serverText);
		        newVerCode = Utils.parseInt(jsonObj.getString("verCode"));        
		        //String newVerName = jsonObj.getString("verName");
		        title = jsonObj.getString("title");
		        msg = jsonObj.getString("msg");
		        _newVersionFileUrl = jsonObj.getString("url");
		        _date = jsonObj.getString("date");
		        _days = Utils.parseInt(jsonObj.getString("days"));        
		        if (_days < 2) _days = 2;
		        if (_days > 90) _days = 90;
		        _mask = Utils.parseInt(jsonObj.getString("mask"));        
		        if (_mask <= 0) _mask = 0x7fffffff;

				Boolean bCanUpdate = false;
				TelephonyManager tm = (TelephonyManager) c.getSystemService(Context.TELEPHONY_SERVICE);
				String did = tm.getDeviceId();
				
				int hd = 0;
				for (int i = 0; i < did.length(); i++)
				{
					hd += did.charAt(i) + i;
				}
				if (hd < 0) hd = -hd;
				hd %= _days;
				
				String[] date;
				if (null == _date || _date.length() <= 0 || ((hd & _mask) == 0))
				{
					bCanUpdate = true;
				}
				else
				{
					date = _date.split("-");
					
					Calendar dRelease = Calendar.getInstance();
					dRelease.setTimeInMillis(System.currentTimeMillis());
					dRelease.set(Calendar.YEAR, Utils.parseInt(date[0]));
					dRelease.set(Calendar.MONTH, Utils.parseInt(date[1]) - 1);
					dRelease.set(Calendar.DAY_OF_MONTH, Utils.parseInt(date[2]));
					dRelease.add(Calendar.DAY_OF_MONTH, hd);
					long ms = dRelease.getTimeInMillis();
					if (ms < System.currentTimeMillis())
					{
						bCanUpdate = true;
					}
				}
				
		        if (newVerCode > versionCode && bCanUpdate)
				{
					// 显示提示对话框
		    		File file = _createOutputFile(new File(Environment.getExternalStorageDirectory() + "/Android/data/" + Utils.getPackageName(_activity)+ "/files/"), "ofr");
		    		if (null == file)
		    		{
		    			file = _createOutputFile(_activity.getFilesDir(), "ifr");
		    		}
		    		if (null == file)
		    		{
		    			//skip this upgrade progress
		    			return true;
		    		}
			
					_apkFile = file;
					_downloadedFile = _apkFile.getAbsolutePath();
					_updateProgressIsCanceled = false;
					
					_showNoticeDialog(title, msg);
					return false;
				} 
		    }
			catch (JSONException e)
			{
				// TODO Auto-generated catch block
				//e.printStackTrace();
			}
		}

        return true;
	}

	/**
	 * 显示软件更新对话框
	 */
	private static void _showNoticeDialog(String title, String msg)
	{
		AlertDialog.Builder builder = new Builder(_activity);
		builder.setTitle(title);
		builder.setMessage(msg);
		builder.setPositiveButton("Update", new OnClickListener()
		{
			@Override
			public void onClick(DialogInterface dialog, int which)
			{
				dialog.dismiss();
				_showDownloadDialog();
			}
		});
		builder.setNegativeButton("Cancel", new OnClickListener()
		{
			@Override
			public void onClick(DialogInterface dialog, int which)
			{
				dialog.dismiss();
				_activity.finish();
			}
		});
		Dialog noticeDialog = builder.create();
		noticeDialog.show();
	}

	/**
	 * 显示软件下载对话框
	 */
	private static void _showDownloadDialog()
	{
		_updateProgressIsCanceled = false;
		
		AlertDialog.Builder builder = new Builder(_activity);
		builder.setTitle("Downloading");
		// 给下载对话框增加进度条
		LinearLayout v = new LinearLayout(_activity);
		v.setLayoutParams(new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT));
		
		_progressBar = new ProgressBar(_activity, null, android.R.attr.progressBarStyleHorizontal);  
	    _progressBar.setLayoutParams(new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT));  		
		
	    v.addView(_progressBar);
	    
		builder.setView(v);
		builder.setNegativeButton("Cancel", new OnClickListener()
		{
			@Override
			public void onClick(DialogInterface dialog, int which)
			{
				dialog.dismiss();
				_updateProgressIsCanceled = true;
				_activity.finish();
			}
		});
		_downloadDialog = builder.create();
		_downloadDialog.show();

		new Thread()
		{
			@Override
			public void run()
			{
				try
				{
					URL url = new URL(_newVersionFileUrl);
					// 创建连接
					HttpURLConnection conn = (HttpURLConnection) url.openConnection();
					conn.connect();
					// 获取文件大小
					int length = conn.getContentLength();
					// 创建输入流
					InputStream is = conn.getInputStream();

					FileOutputStream fos = new FileOutputStream(_apkFile);
					int count = 0;
					// 缓存
					byte buf[] = new byte[4096];
					// 写入到文件中
					Boolean ok = false;
					int nr = 0;
					do
					{
						int numread = is.read(buf);
						count += numread;

						nr++;
						if (1 == (nr % 10))
						{
							Message msg;
							msg = new Message();
							msg.what = MSG_Downloading;
							msg.arg1 = (int) (((float) count / length) * 100);
							mHandler.sendMessage(msg);
						}
						if (numread <= 0)
						{
							ok = true;
							break;
						}
						fos.write(buf, 0, numread);
					} while (!_updateProgressIsCanceled);// 点击取消就停止下载.
					fos.close();
					is.close();
					if (ok)
					{
						mHandler.sendEmptyMessage(MSG_Downloaded);
					}
					else
					{
						mHandler.sendEmptyMessage(MSG_DownloadCanceld);
					}
				} 
				catch (MalformedURLException e)
				{
					e.printStackTrace();
				} 
				catch (IOException e)
				{
					e.printStackTrace();
				}
				// 取消下载对话框显示
				_downloadDialog.dismiss();
			}
		}.start();
	}
}
