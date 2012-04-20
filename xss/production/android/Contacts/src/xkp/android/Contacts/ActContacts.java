package xkp.android.Contacts;
import java.util.ArrayList;
import java.util.Arrays;
import android.view.View;
import android.view.ViewTreeObserver;
import android.view.ViewTreeObserver.OnGlobalLayoutListener;
			import android.app.Activity;
			import android.os.Bundle;
import xkp.android.libs.Layout.XKPLayout;
						import android.view.View.OnClickListener;
						import android.view.View.OnLongClickListener;
						import android.view.MotionEvent;
						import android.view.View.OnTouchListener;
						import android.view.View.OnKeyListener;
						import android.view.View.OnFocusChangeListener;
						import android.text.method.KeyListener;
						import android.widget.Button;
						import xkp.android.libs.Widget.XKPSwitcher;
						import android.widget.TextView;
						import android.widget.EditText;
						import android.widget.CheckBox;
						import android.widget.CompoundButton;
						import android.widget.CompoundButton.OnCheckedChangeListener;
						import android.widget.RadioButton;
		public class ActContacts 
						extends Activity
{
				private XKPLayout __div1;
				private Button btnLeft;
				private Button btnRight;
				private XKPSwitcher swtStates;
				private XKPLayout divAdd;
				private TextView lblFullName;
				private TextView lblPhone;
				private EditText edtFullName;
				private EditText edtPhoneNumber;
				private CheckBox chkVerifier;
				private Button btnAddContact;
				private XKPLayout divFind;
				private TextView lblFindType;
				private RadioButton rdbByName;
				private RadioButton rdbByPhone;
				private TextView lblCriterion;
				private EditText edtCriterion;
				private XKPLayout divAbout;
				private TextView lblName;
				private TextView lblContact;
		private XKPLayout layoutapplication;
	private boolean mLayoutStarted = false;
			private ActContacts application;
			@Override
			public void onCreate(Bundle savedInstanceState) {
				super.onCreate(savedInstanceState);
                initCallers();
				initInstances();
				setContentView(R.layout.main);
				bindViews();
			}
			private void bindViews() {
		// TIPS: create an observer for notify layout updated
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
				__div1 = (XKPLayout) findViewById(R.id.__div1);
				btnLeft = (Button) findViewById(R.id.btnLeft);
				btnLeft.setOnClickListener(new OnClickListener() {
					@Override
					public void onClick(View v) {
						onClickbtnLeft();
					}
				});
				btnRight = (Button) findViewById(R.id.btnRight);
				btnRight.setOnClickListener(new OnClickListener() {
					@Override
					public void onClick(View v) {
						onClickbtnRight();
					}
				});
				swtStates = (XKPSwitcher) findViewById(R.id.swtStates);
				divAdd = (XKPLayout) findViewById(R.id.divAdd);
				lblFullName = (TextView) findViewById(R.id.lblFullName);
				lblPhone = (TextView) findViewById(R.id.lblPhone);
				edtFullName = (EditText) findViewById(R.id.edtFullName);
				edtPhoneNumber = (EditText) findViewById(R.id.edtPhoneNumber);
				chkVerifier = (CheckBox) findViewById(R.id.chkVerifier);
				chkVerifier.setOnCheckedChangeListener(new OnCheckedChangeListener() {
					@Override
					public void onCheckedChanged(CompoundButton v, Boolean isChecked) {
						onCheckedChangedchkVerifier();
					}
				});
				btnAddContact = (Button) findViewById(R.id.btnAddContact);
				divFind = (XKPLayout) findViewById(R.id.divFind);
				lblFindType = (TextView) findViewById(R.id.lblFindType);
				rdbByName = (RadioButton) findViewById(R.id.rdbByName);
				rdbByPhone = (RadioButton) findViewById(R.id.rdbByPhone);
				lblCriterion = (TextView) findViewById(R.id.lblCriterion);
				edtCriterion = (EditText) findViewById(R.id.edtCriterion);
				divAbout = (XKPLayout) findViewById(R.id.divAbout);
				lblName = (TextView) findViewById(R.id.lblName);
				lblContact = (TextView) findViewById(R.id.lblContact);
	}
private void initCallers() {
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
			}
			private void onClickbtnLeft() {
				swtStates.setDisplayedChild(swtStates.getDisplayedChild() - 1);
btnLeft.setText(mthd_application_getSwitchText(swtStates.getDisplayedChild() - 1));
int x = ((XKPLayout.LayoutParams) btnLeft.getLayoutParams()).getPlacement();
((XKPLayout.LayoutParams) btnLeft.getLayoutParams()).setPlacement(XKPLayout.PL_LEFT);
			}
			private void onClickbtnRight() {
				swtStates.setDisplayedChild(swtStates.getDisplayedChild() + 1);
btnLeft.setText(mthd_application_getSwitchText(swtStates.getDisplayedChild() + 1));
			}
			private void onCheckedChangedchkVerifier() {
			}
			private ArrayList prop_application_swtTabsTexts = new ArrayList(Arrays.asList( new Object [] {"Add contact", "Find contact", "About"} ));
		public String mthd_application_getSwitchText(Integer idx) {
			Object swtTabsSize = prop_application_swtTabsTexts.size;
if (idx < 0)
{
	idx = swtTabsSize;
}
else
{
	if (idx >= swtTabsSize)
	{
		idx = idx % swtTabsSize;
	}
}
return prop_application_swtTabsTexts.get(idx);
		}
			private void creationapplication() {
			}
}
