package xkp.android.AnimTest;
import java.util.ArrayList;
import java.util.Arrays;
import android.view.View;
			import android.app.Activity;
			import android.os.Bundle;
import xkp.android.libs.Layout.XKPLayout;
						import android.widget.TextView;
						import android.text.method.KeyListener;
						import android.view.View.OnClickListener;
						import android.view.View.OnLongClickListener;
						import android.view.MotionEvent;
						import android.view.View.OnTouchListener;
						import android.view.View.OnKeyListener;
						import android.view.View.OnFocusChangeListener;
						import android.widget.Button;
        import java.util.Timer;
        import java.util.TimerTask;
        import xkp.android.libs.Sequence.*;
		public class ActAnimTest 
						extends Activity
							implements OnClickListener
{
				private TextView lbl1;
				private TextView lbl2;
				private TextView lbl3;
				private Button btn1;
				private Button btn2;
				private Button btn3;
			private Sequence anim1;
private Sequence anim2;
private Sequence anim3;
		public void mthd_anim3_stop_anim3() {
			lbl3.setText("Button 3 Stopped");
anim3.stop();
		}
	ActAnimTest application;
			@Override
			public void onCreate(Bundle savedInstanceState) {
				super.onCreate(savedInstanceState);
                initCallers();
				initInstances();
				setContentView(R.layout.main);
				bindViews();
			}
			private void bindViews() {
				lbl1 = (TextView) findViewById(R.id.lbl1);
				lbl2 = (TextView) findViewById(R.id.lbl2);
				lbl3 = (TextView) findViewById(R.id.lbl3);
				btn1 = (Button) findViewById(R.id.btn1);
					btn1.setOnClickListener(this);
				btn2 = (Button) findViewById(R.id.btn2);
					btn2.setOnClickListener(this);
				btn3 = (Button) findViewById(R.id.btn3);
					btn3.setOnClickListener(this);
	}
void initCallers() {
			Start();
}
			void initInstances() {
				application = this;
anim1 = new Sequence();
anim1.events.addListener("start", new EventHolder.Implementor() 
{
	public void call(Object ev) 
	{
				lbl1.setText("Button 1 Animating...");
				}
});
anim1.events.addListener("stop", new EventHolder.Implementor() 
{
	public void call(Object ev) 
	{
				lbl1.setText("Button 1 Stopped");
anim2.start();
				}
});
            anim1.setParent(null);
					//x
                   final Interpolator ____i1 = new Interpolator();
				   ____i1.setAssigner(new Assign(){
	                    public void put(Object value)
	                    {
						    ((XKPLayout.LayoutParams) btn1.getLayoutParams()).setLeft((Integer)value);
	                    }
				   });
				   ____i1.setInterpolator(Manager.getInstance().interpolator("Integer"));	
						____i1.addKey(0, 40);
						____i1.addKey(1, 200);
				    anim1.addHandler(____i1);
anim2 = new Sequence();
anim2.events.addListener("start", new EventHolder.Implementor() 
{
	public void call(Object ev) 
	{
				lbl2.setText("Button 2 Chasing...");
				}
});
anim2.events.addListener("stop", new EventHolder.Implementor() 
{
	public void call(Object ev) 
	{
				lbl2.setText("Button 2 Stopped");
				}
});
            anim2.setParent(null);
					//x
                   final Interpolator ____i2 = new Interpolator();
				   ____i2.setAssigner(new Assign(){
	                    public void put(Object value)
	                    {
						    ((XKPLayout.LayoutParams) btn2.getLayoutParams()).setLeft((Integer)value);
	                    }
				   });
				   ____i2.setInterpolator(Manager.getInstance().interpolator("Integer"));	
						____i2.addKey(0, null);
						____i2.addKey(2, null);
				    anim2.addHandler(____i2);
                anim2.events.addListener("start",  new EventHolder.Implementor() 
                {
	                public void call(Object ev) 
                    {
                    ____i2.setKeyValue(0, btn2.getLeft());
                    ____i2.setKeyValue(1, btn1.getLeft());
                }
                });
anim3 = new Sequence();anim3.setLoop(true);
                anim3.start();
            anim3.setParent(null);
					//x
                   final Interpolator ____i3 = new Interpolator();
				   ____i3.setAssigner(new Assign(){
	                    public void put(Object value)
	                    {
						    ((XKPLayout.LayoutParams) btn3.getLayoutParams()).setLeft((Integer)value);
	                    }
				   });
				   ____i3.setInterpolator(Manager.getInstance().interpolator("Integer"));	
						____i3.addKey(0, 40);
						____i3.addKey(1.5, 200);
				    anim3.addHandler(____i3);
					//x
                   final Interpolator ____i4 = new Interpolator();
				   ____i4.setAssigner(new Assign(){
	                    public void put(Object value)
	                    {
						    ((XKPLayout.LayoutParams) btn3.getLayoutParams()).setLeft((Integer)value);
	                    }
				   });
				   ____i4.setInterpolator(Manager.getInstance().interpolator("Integer"));	
						____i4.addKey(1.5, 200);
						____i4.addKey(3, 40);
				    anim3.addHandler(____i4);
                anim3.events.addListener("update",  new EventHolder.Implementor() 
                {
	                public void call(Object ev) 
                    {
                    if (prop_application_click_count == 3)
                    {
						mthd_anim3_stop_anim3();
                    }
                }
			    });
			}
			private double update_freq = 1/30.0;
			private void Update() {
				runOnUiThread(new Runnable() {
					public void run() {
						double delta = update_freq;
		Manager.getInstance().update(delta);
					}
				});
			};
			private void Start() {
				Timer timer = new Timer();
				timer.schedule(new TimerTask() {
					@Override
					public void run() {
						Update();
					}
				}, 0, (long)(update_freq * 1000)); 
			}
						public void onClick(View v) {
							switch (v.getId()) {
									case R.id.btn1:
										onClickbtn1();
										break;
									case R.id.btn2:
										onClickbtn2();
										break;
									case R.id.btn3:
										onClickbtn3();
										break;
						} //switch
						}
			private void onClickbtn1() {
				anim1.start();
			}
			private void onClickbtn2() {
				((XKPLayout.LayoutParams) btn2.getLayoutParams()).setLeft((Integer)40);
			}
			private void onClickbtn3() {
				prop_application_click_count = prop_application_click_count + 1;
if (prop_application_click_count == 4)
{
	prop_application_click_count = 0;
	anim3.start();
}
			}
			private Integer prop_application_click_count = 0;
}
