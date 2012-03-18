package xkp.android.TouchMe;
import java.util.ArrayList;
import java.util.Arrays;
import android.view.View;
			import android.app.Activity;
			import android.os.Bundle;
import xkp.android.libs.Layout.XKPLayout;
						import android.widget.Button;
						import android.text.method.KeyListener;
						import android.view.View.OnClickListener;
						import android.view.View.OnLongClickListener;
						import android.view.MotionEvent;
						import android.view.View.OnTouchListener;
						import android.view.View.OnKeyListener;
						import android.view.View.OnFocusChangeListener;
		public class ActTouchMe 
						extends Activity
							implements OnTouchListener
{
				private Button btnTouchme;
	ActTouchMe application;
			@Override
			public void onCreate(Bundle savedInstanceState) {
				super.onCreate(savedInstanceState);
                initCallers();
				initInstances();
				setContentView(R.layout.main);
				bindViews();
			}
			private void bindViews() {
				btnTouchme = (Button) findViewById(R.id.btnTouchme);
					btnTouchme.setOnTouchListener(this);
	}
void initCallers() {
}
			void initInstances() {
				application = this;
			}
						public boolean onTouch(View v, MotionEvent ev) {
							switch (v.getId()) {
									case R.id.btnTouchme:
										onTouchbtnTouchme(ev);
										break;
						} //switch
							return true;
						}
			private Double prop_btnTouchme_MAX_DELTHA = 80.000000f;
			private Double prop_btnTouchme_FIRST_QUATER = Math.PI / 4;
			private Double prop_btnTouchme_SECOND_QUATER = 3 * Math.PI / 4;
			private Double prop_btnTouchme_THIRD_QUATER = 5 * Math.PI / 4;
			private Double prop_btnTouchme_FOURTH_QUATER = 7 * Math.PI / 4;
			private Float prop_btnTouchme_mStartX;
			private Float prop_btnTouchme_mStartY;
			private void onTouchbtnTouchme(MotionEvent ev) {
				int action = ev.getAction();
int pl = XKPLayout.PL_CENTER;
Display display = getWindowManager().getDefaultDisplay();
Integer dispWidth = display.getWidth();
Integer dispHeight = display.getHeight();
Float currentX = ev.getX();
Float currentY = dispHeight - ev.getY();
if (action == MotionEvent.ACTION_DOWN)
{
	prop_btnTouchme_mStartX = currentX;
	prop_btnTouchme_mStartY = currentY;
}
else
{
	if (action == MotionEvent.ACTION_UP)
	{
		Float delthaX = currentX - prop_btnTouchme_mStartX;
		Float delthaY = currentY - prop_btnTouchme_mStartY;
		Double angle = Math.atan2(delthaY, delthaX);
		if (angle < 0)
		{
			angle += 2 * Math.PI;
		}
		Float abs_delthaX = Math.abs(delthaX);
		Float abs_delthaY = Math.abs(delthaY);
		Float max_deltha = Math.max(abs_delthaX, abs_delthaY);
		if (max_deltha < prop_btnTouchme_MAX_DELTHA / 10)
		{
			((XKPLayout.LayoutParams) btnTouchme.getLayoutParams()).setPlacement(pl);
			return;
		}
		else
		{
			if (max_deltha < prop_btnTouchme_MAX_DELTHA)
			{
				return;
			}
		}
		if (angle >= prop_btnTouchme_FIRST_QUATER && angle <= prop_btnTouchme_SECOND_QUATER)
		{
			pl = XKPLayout.PL_TOP;
		}
		else
		{
			if (angle >= prop_btnTouchme_SECOND_QUATER && angle <= prop_btnTouchme_THIRD_QUATER)
			{
				pl = XKPLayout.PL_LEFT;
			}
			else
			{
				if (angle >= prop_btnTouchme_THIRD_QUATER && angle <= prop_btnTouchme_FOURTH_QUATER)
				{
					pl = XKPLayout.PL_BOTTOM;
				}
				else
				{
					pl = XKPLayout.PL_RIGHT;
				}
			}
		}
		((XKPLayout.LayoutParams) btnTouchme.getLayoutParams()).setPlacement(pl);
	}
}
			}
}
