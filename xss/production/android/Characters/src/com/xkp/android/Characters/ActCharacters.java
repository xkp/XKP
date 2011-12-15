package com.xkp.android.Characters;
import java.util.ArrayList;
import java.util.Arrays;
import android.view.View;
			import android.app.Activity;
			import android.os.Bundle;
		public class ActCharacters 
						extends Activity
{
				private character aladdin;
				private character mickey;
				private character pluto;
			@Override
			public void onCreate(Bundle savedInstanceState) {
				super.onCreate(savedInstanceState);
				setContentView(R.layout.main);
				bindViews();
				initModules();
				initInstances();
                Start();
			}
			private void bindViews() {
				aladdin = (character) findViewById(R.id.aladdin);
				mickey = (character) findViewById(R.id.mickey);
				pluto = (character) findViewById(R.id.pluto);
	}
			void initModules() {
    }
			void initInstances() {
			}
private double update_freq = 1/30.0;
private void Update()
{
    runOnUiThread(new Runnable() 
    {
		public void run() 
        {
            double delta = update_freq;
                    }
	});
};
private void Start()
{
    Timer timer = new Timer();
    timer.schedule(new TimerTask() 
    {
		@Override
		public void run() 
        {
            Update();
		} 
    }, 0, (long)(update_freq * 1000)); 
}
}
