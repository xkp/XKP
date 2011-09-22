package com.xkp.android.SimpleTest;
import java.util.ArrayList;
import java.util.Arrays;
import android.app.Activity;
import android.os.Bundle;
import android.view.View;
						import android.widget.TextView;
						import android.view.View.OnClickListener;
						import android.widget.EditText;
						import android.widget.Button;
public class ActSimpleTest 
                extends Activity
							implements OnClickListener
{
                private TextView lblFirstName;
                private EditText edtFirstName;
                private TextView lblLastName;
                private EditText edtLastName;
                private Button btnFullName;
                private TextView lblFullName;
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        bindViews();
    }
    private void bindViews() {
                lblFirstName = (TextView) findViewById(R.id.lblFirstName);
                edtFirstName = (EditText) findViewById(R.id.edtFirstName);
                lblLastName = (TextView) findViewById(R.id.lblLastName);
                edtLastName = (EditText) findViewById(R.id.edtLastName);
                btnFullName = (Button) findViewById(R.id.btnFullName);
                    btnFullName.setOnClickListener(this);
                lblFullName = (TextView) findViewById(R.id.lblFullName);
			    }
						@Override
						public void onClick(View v) {
							switch (v.getId()) {
									case R.id.btnFullName:
										onClickbtnFullName();
										break;
							} //switch
						}
			private void onClickbtnFullName() {
				String fullname = edtFirstName.getText().toString() + " " + edtLastName.getText().toString();
if (fullname != " ")
{
	lblFullName.setText("Your fullname is " + fullname);
}
else
{
	lblFullName.setText("Your fullname is EMPTY!");
}
			}
}
