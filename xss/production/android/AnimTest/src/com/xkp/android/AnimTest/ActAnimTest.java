package com.xkp.android.AnimTest;
import xkp.android.lib.Assign;
import xkp.android.lib.Interpolator;
import xkp.android.lib.Manager;
import xkp.android.lib.Sequence;
import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
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
	    ____i1.setAssign(new Assign() {
				@Override
				public void execute(Object t, Object value){
					Button btn1 = (Button)t;
					//Log.d("execute", btn1.toString());
					//Log.d("execute", value.toString());
					((XKPLayout.LayoutParams) btn1.getLayoutParams()).setLeft(Integer.valueOf(value.toString()));
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
		//((XKPLayout.LayoutParams) btn1.getLayoutParams()).setLeft(btn1.getLeft() - 5);
	}
}
