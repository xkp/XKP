package xkp.android.Figures2D;
import java.util.ArrayList;
import java.util.Arrays;
import android.view.View;
import android.view.ViewTreeObserver;
import android.view.ViewTreeObserver.OnGlobalLayoutListener;
			import android.app.Activity;
			import android.os.Bundle;
import xkp.android.libs.Layout.XKPLayout;
						import android.view.View.OnKeyListener;
						import android.view.KeyEvent;
						import android.view.View.OnClickListener;
						import android.view.View.OnLongClickListener;
						import android.view.MotionEvent;
						import android.view.View.OnTouchListener;
						import android.view.View.OnFocusChangeListener;
						import xkp.android.libs.Graphics.XKPGraphics.OnClickInsideFigureListener;
						import xkp.android.libs.Graphics.XKPCircle;
						import xkp.android.libs.Graphics.XKPRectangle;
						import xkp.android.libs.Graphics.XKPPolygon;
		public class ActFigures2D 
						extends Activity
{
				private XKPLayout div1;
				private XKPCircle c1;
				private XKPRectangle r1;
				private XKPPolygon p1;
		private XKPLayout layoutapplication;
	private boolean mLayoutStarted = false;
			private ActFigures2D application;
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
				div1 = (XKPLayout) findViewById(R.id.div1);
				c1 = (XKPCircle) findViewById(R.id.c1);
				c1.setOnClickInsideFigureListener(new OnClickInsideFigureListener() {
					@Override
					public void onClickInsideFigure(View v) {
						onClickInsideFigurec1();
					}
				});
				r1 = (XKPRectangle) findViewById(R.id.r1);
				r1.setOnClickInsideFigureListener(new OnClickInsideFigureListener() {
					@Override
					public void onClickInsideFigure(View v) {
						onClickInsideFigurer1();
					}
				});
				p1 = (XKPPolygon) findViewById(R.id.p1);
				p1.setOnClickInsideFigureListener(new OnClickInsideFigureListener() {
					@Override
					public void onClickInsideFigure(View v) {
						onClickInsideFigurep1();
					}
				});
			p1.addPoint(100, 100);
			p1.addPoint(100, 50);
			p1.addPoint(190, 10);
			p1.addPoint(150, 200);
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
			private void onClickInsideFigurec1() {
				c1.setLineWidth(c1.getLineWidth() + 1);
			}
			private void onClickInsideFigurer1() {
				r1.setStrokeColor("brown");
			}
			private void onClickInsideFigurep1() {
				for(Integer i = 0; i < p1.getPointsCount(); i++)
{
	Integer new_x;
	Integer new_y;
	if (i % 2 == 0)
	{
		new_x = p1.getPointX(i) + 5;
		new_y = p1.getPointY(i) + 5;
	}
	else
	{
		new_x = p1.getPointX(i) - 5;
		new_y = p1.getPointY(i) - 5;
	}
	p1.setPoint(i, new_x, new_y);
}
			}
}
