package xkp.android.Contacts;
import java.util.ArrayList;
import java.util.Arrays;
import android.view.View;
			import android.app.Activity;
			import android.os.Bundle;
import xkp.android.libs.Layout.XKPLayout;
						import android.view.View.OnClickListener;
						import android.view.View.OnLongClickListener;
						import android.view.MotionEvent;
						import android.view.View.OnTouchListener;
						import android.view.View.OnKeyListener;
						import android.view.View.OnFocusChangeListener;
						import android.widget.Button;
						import android.text.method.KeyListener;
						import android.widget.TextView;
						import android.widget.EditText;
						import android.widget.CheckBox;
						import android.widget.CompoundButton;
						import android.widget.CompoundButton.OnCheckedChangeListener;
						import android.widget.RadioButton;
		public class ActContacts 
						extends Activity
							implements OnClickListener
							implements OnCheckedChangeListener
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
	ActContacts application;
			@Override
			public void onCreate(Bundle savedInstanceState) {
				super.onCreate(savedInstanceState);
				setContentView(R.layout.main);
				bindViews();
				initInstances();
                initCallers();
			}
			private void bindViews() {
				__div1 = (XKPLayout) findViewById(R.id.__div1);
				btnLeft = (Button) findViewById(R.id.btnLeft);
					btnLeft.setOnClickListener(this);
				btnRight = (Button) findViewById(R.id.btnRight);
					btnRight.setOnClickListener(this);
				swtStates = (XKPSwitcher) findViewById(R.id.swtStates);
				divAdd = (XKPLayout) findViewById(R.id.divAdd);
				lblFullName = (TextView) findViewById(R.id.lblFullName);
				lblPhone = (TextView) findViewById(R.id.lblPhone);
				edtFullName = (EditText) findViewById(R.id.edtFullName);
				edtPhoneNumber = (EditText) findViewById(R.id.edtPhoneNumber);
				chkVerifier = (CheckBox) findViewById(R.id.chkVerifier);
					chkVerifier.setOnCheckedChangeListener(this);
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
void initCallers() {
}
			void initInstances() {
				application = this;
			}
						public void onClick(View v) {
							switch (v.getId()) {
									case R.id.btnLeft:
										onClickbtnLeft();
										break;
									case R.id.btnRight:
										onClickbtnRight();
										break;
						} //switch
						}
						public void onCheckedChanged(CompoundButton v, Boolean isChecked) {
							switch (v.getId()) {
									case R.id.chkVerifier:
										onCheckedChangedchkVerifier();
										break;
						} //switch
						}
			private void onClickbtnLeft() {
				swtStates.setDisplayedChild(swtStates.getDisplayedChild() - 1);
btnLeft.setText(application.mthd_application_getSwitchText(swtStates.getDisplayedChild() - 1));
int x = ((XKPLayout.LayoutParams) btnLeft.getLayoutParams()).getPlacement();
((XKPLayout.LayoutParams) btnLeft.getLayoutParams()).setPlacement(XKPLayout.PL_LEFT);
			}
			private void onClickbtnRight() {
				swtStates.setDisplayedChild(swtStates.getDisplayedChild() + 1);
btnLeft.setText(application.mthd_application_getSwitchText(swtStates.getDisplayedChild() + 1));
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
