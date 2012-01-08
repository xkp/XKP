package xkp.android.SimpleTest;
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
				private EditText edtContainer;
	ActSimpleTest application;
			@Override
			public void onCreate(Bundle savedInstanceState) {
				super.onCreate(savedInstanceState);
				setContentView(R.layout.main);
				bindViews();
				initInstances();
                initCallers();
			}
			private void bindViews() {
				lblFirstName = (TextView) findViewById(R.id.lblFirstName);
				edtFirstName = (EditText) findViewById(R.id.edtFirstName);
				lblLastName = (TextView) findViewById(R.id.lblLastName);
				edtLastName = (EditText) findViewById(R.id.edtLastName);
				btnFullName = (Button) findViewById(R.id.btnFullName);
					btnFullName.setOnClickListener(this);
				lblFullName = (TextView) findViewById(R.id.lblFullName);
				edtContainer = (EditText) findViewById(R.id.edtContainer);
	}
void initCallers() {
}
			void initInstances() {
				application = this;
			}
						public void onClick(View v) {
							switch (v.getId()) {
									case R.id.btnFullName:
										onClickbtnFullName();
										break;
						} //switch
						}
			private void onClickbtnFullName() {
				String fullname = edtFirstName.getText().toString() + " " + edtLastName.getText().toString();
if (!fullname.equals(" "))
{
	lblFullName.setText("Your full name is " + fullname);
}
else
{
	lblFullName.setText("Your full name is EMPTY!");
}
edtContainer.append(lblFullName.getText().toString() + "\n");
			}
}
