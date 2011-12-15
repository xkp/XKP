package com.xkp.android.KitchenSink;
import java.util.ArrayList;
import java.util.Arrays;
import android.view.View;
						import android.view.ViewGroup;
						import android.widget.TabHost;
						import android.widget.TabHost.TabSpec;
						import android.widget.TabHost.TabContentFactory;
						import android.widget.TabHost.OnTabChangeListener;
						import android.view.View.OnClickListener;
						import android.view.View.OnLongClickListener;
						import android.view.MotionEvent;
						import android.view.View.OnTouchListener;
						import android.view.View.OnKeyListener;
						import android.view.View.OnFocusChangeListener;
						import android.widget.Button;
						import android.text.method.KeyListener;
						import android.widget.ToggleButton;
						import android.widget.ImageButton;
						import android.widget.ImageView;
						import android.widget.TextView;
						import android.widget.EditText;
						import android.widget.CheckBox;
						import android.widget.CompoundButton;
						import android.widget.CompoundButton.OnCheckedChangeListener;
						import android.widget.ScrollView;
						import android.widget.RadioGroup;
						import android.widget.RadioGroup.OnCheckedChangeListener;
						import android.widget.RadioButton;
						import android.widget.ProgressBar;
						import android.widget.SeekBar;
						import android.widget.SeekBar.OnSeekBarChangeListener;
			import android.app.Activity;
			import android.os.Bundle;
		public class ActKitchenSink 
						extends Activity
{
				private TabHost kitchen;
					XKPUtils util = new XKPUtils();
				private XKPLayout __div1;
				private Button btn1;
				private ToggleButton toggle1;
				private ImageButton imgbtn1;
				private TextView lbl1;
				private EditText edit1;
				private CheckBox chkb1;
				private ScrollView __scroller1;
				private RadioGroup rdgTab3;
				private RadioButton radio1;
				private RadioButton radio2;
				private RadioButton radio3;
				private RadioButton radio4;
				private RadioButton radio5;
				private RadioButton radio6;
				private RadioButton radio7;
				private RadioButton radio8;
				private RadioButton radio9;
				private ScrollView __scroller2;
				private ImageView img1;
				private XKPLayout __div2;
				private ProgressBar prgbar1;
				private ProgressBar prgbar2;
				private ProgressBar prgbar3;
				private ProgressBar prgbar4;
				private SeekBar seekb1;
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
				kitchen = (TabHost) findViewById(R.id.kitchen);
					ArrayList tabs = new ArrayList(Arrays.asList( new String [] {				"Buttons?"							, "Radios"							, "Image"							, "ProgressBars"						} ));        util.setupTabHost(R.id.kitchen, tabs);
				__div1 = (XKPLayout) findViewById(R.id.__div1);
				btn1 = (Button) findViewById(R.id.btn1);
				toggle1 = (ToggleButton) findViewById(R.id.toggle1);
				imgbtn1 = (ImageButton) findViewById(R.id.imgbtn1);
				lbl1 = (TextView) findViewById(R.id.lbl1);
				edit1 = (EditText) findViewById(R.id.edit1);
				chkb1 = (CheckBox) findViewById(R.id.chkb1);
				__scroller1 = (ScrollView) findViewById(R.id.__scroller1);
				rdgTab3 = (RadioGroup) findViewById(R.id.rdgTab3);
				radio1 = (RadioButton) findViewById(R.id.radio1);
				radio2 = (RadioButton) findViewById(R.id.radio2);
				radio3 = (RadioButton) findViewById(R.id.radio3);
				radio4 = (RadioButton) findViewById(R.id.radio4);
				radio5 = (RadioButton) findViewById(R.id.radio5);
				radio6 = (RadioButton) findViewById(R.id.radio6);
				radio7 = (RadioButton) findViewById(R.id.radio7);
				radio8 = (RadioButton) findViewById(R.id.radio8);
				radio9 = (RadioButton) findViewById(R.id.radio9);
				__scroller2 = (ScrollView) findViewById(R.id.__scroller2);
				img1 = (ImageView) findViewById(R.id.img1);
				__div2 = (XKPLayout) findViewById(R.id.__div2);
				prgbar1 = (ProgressBar) findViewById(R.id.prgbar1);
				prgbar2 = (ProgressBar) findViewById(R.id.prgbar2);
				prgbar3 = (ProgressBar) findViewById(R.id.prgbar3);
				prgbar4 = (ProgressBar) findViewById(R.id.prgbar4);
				seekb1 = (SeekBar) findViewById(R.id.seekb1);
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
					public class XKPUtils {
						void setupTabHost(int id, ArrayList<String> tabs) {
			TabHost tabh = (TabHost) findViewById(id);
			tabh.setup();
			TabHost.TabSpec spec;
			ViewGroup linear = (ViewGroup) tabh.getChildAt(0);
			ViewGroup frame = (ViewGroup) linear.getChildAt(1);
			int szTabs = tabs.size();
			int childs = frame.getChildCount();
			int szMin = Math.min(szTabs, childs);
			for(int i = 0; i < szMin; i++) {
				String tab = tabs.get(i);
				int viewId = frame.getChildAt(i).getId();
				spec = tabh.newTabSpec(tab);
				spec.setContent(viewId);
				spec.setIndicator(tab);
				tabh.addTab(spec);
			}
		}
		void setResourceImageView(int id, String resource) {
			int rpos = resource.lastIndexOf("/");
			String n_res = resource.substring(rpos + 1);
			rpos = n_res.lastIndexOf(".");
			String res = n_res.substring(0, rpos);
			int resId = getResources().getIdentifier(
							res, "drawable", 
							getContext().getPackageName());
			ImageView img = (ImageView) findViewById(id);
			img.setImageResource(resId);
		}
			}
}
