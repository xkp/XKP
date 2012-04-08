package xkp.android.KitchenSink;
import java.util.ArrayList;
import java.util.Arrays;
import android.view.View;
import android.view.ViewTreeObserver;
import android.view.ViewTreeObserver.OnGlobalLayoutListener;
			import android.app.Activity;
			import android.os.Bundle;
import xkp.android.libs.Layout.XKPLayout;
			import xkp.android.libs.Widget.XKPUtils;
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
						import xkp.android.libs.Widget.XKPUtils;
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
						import xkp.android.libs.Widget.XKPPackage;
						import xkp.android.libs.Widget.XKPPackage.OnResourcePackageListener;
		public class ActKitchenSink 
						extends Activity
{
				private TabHost kitchen;
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
			public static XKPUtils util;
		private XKPLayout layoutapplication;
				private XKPPackage __resources1;
			private String [] mResources___resources1_XKPName = {
				"dialog",
				"emulator"
			};
			private String [] mResources___resources1_DroidName = {
				"dialog.png",
				"emulator.png"
			};
			private Integer [] mResources___resources1_Type = {
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE
			};
			private Integer [] mResources___resources1_Id = {
				R.drawable.dialog,
				R.drawable.emulator
			};
	private boolean mLayoutStarted = false;
			private ActKitchenSink application;
			@Override
			public void onCreate(Bundle savedInstanceState) {
				super.onCreate(savedInstanceState);
                initCallers();
				initInstances();
				setContentView(R.layout.main);
				bindViews();
			}
			private void bindViews() {
		layoutapplication = (XKPLayout) findViewById(R.id.layoutapplication);
		ViewTreeObserver mainLayoutViewTreeObserver = layoutapplication.getViewTreeObserver();
		if(mainLayoutViewTreeObserver.isAlive()) {
			mainLayoutViewTreeObserver.addOnGlobalLayoutListener(new OnGlobalLayoutListener() {
				@Override
				public void onGlobalLayout() {
					if(!mLayoutStarted) {
						mLayoutStarted = true;
						onLayoutStarted();
					}
					onLayoutUpdated();
				}
			});
		}
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
private void initCallers() {
			util = new XKPUtils();
			ActKitchenSink.util.addView(this);
			util = ActKitchenSink.util;
}
			@Override
			public void onPause() {
				super.onPause();
			}
			@Override
			public void onStop() {
				super.onStop();
			}
			@Override
			public void onResume() {
				super.onResume();
			}
private void onLayoutUpdated() {
}
private void onLayoutStarted() {
}
			private void initInstances() {
				application = this;
				__resources1 = new XKPPackage(this, 
					mResources___resources1_XKPName, mResources___resources1_DroidName, 
					mResources___resources1_Type, mResources___resources1_Id
					, true);
				util.addXKPPackage(__resources1);
			}
}
