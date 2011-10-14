package com.xkp.android.TouchMe;
import java.util.ArrayList;
import java.util.Arrays;
import android.app.Activity;
import android.os.Bundle;
import android.view.View;
						import android.widget.Button;
						import android.view.View.OnClickListener;
public class ActTouchMe 
                extends Activity
{
                private Button btnTouchme;
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        bindViews();
    }
    private void bindViews() {
                btnTouchme = (Button) findViewById(R.id.btnTouchme);
			    }
			private Double prop_btnTouchme_MAX_DELTHA = 80.000000;
			private Double prop_btnTouchme_FIRST_QUATER = Math.PI / 4;
			private Double prop_btnTouchme_SECOND_QUATER = 3 * Math.PI / 4;
			private Double prop_btnTouchme_THIRD_QUATER = 5 * Math.PI / 4;
			private Double prop_btnTouchme_FOURTH_QUATER = 7 * Math.PI / 4;
			private Double prop_btnTouchme_mStartX;
			private Double prop_btnTouchme_mStartY;
			private void touchbtnTouchme(MotionEvent ev) {
				int action = ev.getAction();
int pl = XKPLayout.PL_CENTER;
Double currentX = ev.getX();
Double currentY = ev.getY();
if (action == MotionEvent.ACTION_DOWN)
{
	this.prop_btnTouchme_mStartX = currentX;
	this.prop_btnTouchme_mStartY = currentY;
}
else
{
	if (action == MotionEvent.ACTION_UP)
	{
		Double delthaX = currentX - this.prop_btnTouchme_mStartX;
		Double delthaY = currentY - this.prop_btnTouchme_mStartY;
		Double angle = Math.atan2(delthaY, delthaX);
		if (angle < 0)
		{
			angle += 2 * Math.PI;
		}
		Double abs_delthaX = Math.abs(delthaX);
		Double abs_delthaY = Math.abs(delthaY);
		Double max_deltha = Math.max(abs_delthaX, abs_delthaY);
		if (max_deltha < this.prop_btnTouchme_MAX_DELTHA / 10)
		{
			((XKPLayout.LayoutParams) btnTouchme.getLayoutParams()).setPlacement(pl);
			return;
		}
		else
		{
			if (max_deltha < this.prop_btnTouchme_MAX_DELTHA)
			{
				return;
			}
		}
		if (angle >= this.prop_btnTouchme_FIRST_QUATER && angle <= this.prop_btnTouchme_SECOND_QUATER)
		{
			pl = XKPLayout.PL_TOP;
		}
		else
		{
			if (angle >= this.prop_btnTouchme_SECOND_QUATER && angle <= this.prop_btnTouchme_THIRD_QUATER)
			{
				pl = XKPLayout.PL_LEFT;
			}
			else
			{
				if (angle >= this.prop_btnTouchme_THIRD_QUATER && angle <= this.prop_btnTouchme_FOURTH_QUATER)
				{
					pl = XKPLayout.PL_BOTTOM;
				}
				else
				{
					pl = XKPLayout.PL_RIGHT;
				}
			}
		}
		((XKPLayout.LayoutParams) this.getLayoutParams()).setPlacement(pl);
	}
}
			}
}
