package com.crazyamber.buzzle;

import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;

public class DebugActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_debug);
		
		
		this.findViewById(R.id.dummy_button).setOnClickListener(_onDebug);
	}
	
	protected View.OnClickListener _onDebug = new View.OnClickListener() 
	{
		@Override
		public void onClick(View v) 
		{
			Intent i = new Intent(DebugActivity.this, MainActivity.class);
			startActivity(i);
		}
	};


	static
	{
		System.loadLibrary("game_frame");
	}
}
