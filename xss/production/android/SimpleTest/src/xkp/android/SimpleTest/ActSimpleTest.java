package xkp.android.SimpleTest;
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
						import android.view.KeyEvent;
						import android.view.View.OnFocusChangeListener;
						import android.text.method.KeyListener;
						import android.widget.TextView;
						import android.widget.EditText;
						import android.widget.Button;
		public class ActSimpleTest 
						extends Activity
{
				private TextView lblFirstName;
				private EditText edtFirstName;
				private TextView lblLastName;
				private EditText edtLastName;
				private Button btnFullName;
				private TextView lblFullName;
				private EditText edtContainer;
		private XKPLayout layoutapplication;
	private boolean mLayoutStarted = false;
			private ActSimpleTest application;
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
				lblFirstName = (TextView) findViewById(R.id.lblFirstName);
				edtFirstName = (EditText) findViewById(R.id.edtFirstName);
				lblLastName = (TextView) findViewById(R.id.lblLastName);
				edtLastName = (EditText) findViewById(R.id.edtLastName);
				btnFullName = (Button) findViewById(R.id.btnFullName);
				btnFullName.setOnClickListener(new OnClickListener() {
					@Override
					public void onClick(View v) {
						onClickbtnFullName();
					}
				});
				lblFullName = (TextView) findViewById(R.id.lblFullName);
				edtContainer = (EditText) findViewById(R.id.edtContainer);
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
