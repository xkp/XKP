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
			Sequence anim1;
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
anim1 = new Sequence();
            anim1.parent_sequence = null;
					//x
                   Interpolator ____i1 = new Interpolator();
				   ____i1.setTarget(btn1);//sin la prop
				   ____i1.setAssign(new Assign(){
								public void execute(Object t,double value){
								Button btn1 = (Button)t;
								((XKPLayout.LayoutParams) btn1.getLayoutParams()).setLeft((int)value);
								}
								});
				   ____i1.defaultInterpolator(default_interpolate);	
						____i1.addKey(0, 0);
						____i1.addKey(3, 200);
				anim1.addHandler(____i1);
	}
}
