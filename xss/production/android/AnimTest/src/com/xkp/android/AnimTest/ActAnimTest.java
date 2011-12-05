package com.xkp.android.AnimTest;
import java.util.ArrayList;
import java.util.Arrays;
import android.view.View;
			import android.app.Activity;
			import android.os.Bundle;
						import android.widget.Button;
						import android.text.method.KeyListener;
						import android.view.View.OnClickListener;
						import android.view.View.OnLongClickListener;
						import android.view.MotionEvent;
						import android.view.View.OnTouchListener;
						import android.view.View.OnKeyListener;
						import android.view.View.OnFocusChangeListener;
		public class ActAnimTest 
						extends Activity
{
				private Button btn1;
			@Override
			public void onCreate(Bundle savedInstanceState) {
				super.onCreate(savedInstanceState);
				setContentView(R.layout.main);
				bindViews();
				initInstances();
			}
			private void bindViews() {
				btn1 = (Button) findViewById(R.id.btn1);
	}
			void initInstances() {
			}
}
