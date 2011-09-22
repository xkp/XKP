package com.xkp.android.Contacts;
import java.util.ArrayList;
import java.util.Arrays;
import android.app.Activity;
import android.os.Bundle;
import android.view.View;
						import android.widget.Button;
						import android.view.View.OnClickListener;
						import android.widget.TextView;
						import android.widget.EditText;
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
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        bindViews();
    }
    private void bindViews() {
                __div1 = (XKPLayout) findViewById(R.id.);
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
						@Override
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
						@Override
						public void onCheckedChanged(CompoundButton v, Boolean isChecked) {
							switch (v.getId()) {
									case R.id.chkVerifier:
										onCheckedChangedchkVerifier();
										break;
							} //switch
						}
			private void onClickbtnLeft() {
				swtStates.setDisplayedChild(swtStates.getDisplayedChild() - 1);
btnLeft.setText(application.getSwitchText(swtStates.getDisplayedChild() - 1));
Integer x = ((XKPLayout.LayoutParams) btnLeft.getLayoutParams()).getPlacement();
((XKPLayout.LayoutParams) btnLeft.getLayoutParams()).setPlacement(XKPLayout.PL_LEFT);
			}
			private void onClickbtnRight() {
				swtStates.setDisplayedChild(swtStates.getDisplayedChild() + 1);
btnLeft.setText(application.getSwitchText(swtStates.getDisplayedChild() + 1));
			}
			private void onCheckedChangedchkVerifier() {
			}
				private ArrayList prop_application_swtTabsTexts
			 = new ArrayList(Arrays.asList( new Object [] {"Add contact", "Find contact", "About"} )) 	; 
			private void creationapplication() {
			}
		public String getSwitchText(Integer idx) {
			if (idx < 0)
{
	idx = prop_application_swtTabsTexts.size;
}
else
{
	if (idx >= prop_application_swtTabsTexts.size)
	{
		idx = idx % prop_application_swtTabsTexts.size;
	}
}
return prop_application_swtTabsTexts.get(idx);
		}
}
