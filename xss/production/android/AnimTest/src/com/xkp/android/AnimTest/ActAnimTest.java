package com.xkp.android.AnimTest;
import java.util.ArrayList;
import java.util.Arrays;
import android.view.View;
import xkp.android.lib.*;
						import android.widget.Button;
						import android.text.method.KeyListener;
						import android.view.View.OnClickListener;
						import android.view.View.OnLongClickListener;
						import android.view.MotionEvent;
						import android.view.View.OnTouchListener;
						import android.view.View.OnKeyListener;
						import android.view.View.OnFocusChangeListener;
			import android.app.Activity;
			import android.os.Bundle;
		public class ActAnimTest 
						extends Activity
							implements OnClickListener
{
				private Button btn1;
			private Sequence anim1;
			@Override
			public void onCreate(Bundle savedInstanceState) {
				super.onCreate(savedInstanceState);
				setContentView(R.layout.main);
				bindViews();
				initModules();
				initInstances();
			}
			private void bindViews() {
				btn1 = (Button) findViewById(R.id.btn1);
					btn1.setOnClickListener(this);
	}
			void initModules() {
        Manager.getInstance().start(1/30.0);
			}
			void initInstances() {
anim1 = new Sequence();
            anim1.parent_sequence = null;
					//x
                   Interpolator ____i1 = new Interpolator();
				   ____i1.setTarget(btn1);//sin la prop
				   ____i1.setAssign(new Assign(){
								public void execute(Object t,Object value){
								Button btn1 = (Button)t;
								((XKPLayout.LayoutParams) btn1.getLayoutParams()).setLeft((Integer)value);
								}
								});
				   ____i1.defaultInterpolator(Manager.getInstance().interpolator("Integer"));	
						____i1.addKey(0, 0);
						____i1.addKey(3, 200);
				anim1.addHandler(____i1);
			}
						@Override
						public void onClick(View v) {
							switch (v.getId()) {
									case R.id.btn1:
										onClickbtn1();
										break;
						} //switch
						}
			private void onClickbtn1() {
				anim1.start();
			}
}
