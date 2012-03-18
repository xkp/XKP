package xkp.android.Figures2D;
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
						import xkp.android.libs.Graphic.XKPCircle;
						import xkp.android.libs.Graphic.XKPGraphics.OnClickInsideFigureListener;
						import xkp.android.libs.Graphic.XKPRectangle;
						import xkp.android.libs.Graphic.XKPPolygon;
		public class ActFigures2D 
						extends Activity
							implements OnClickInsideFigureListener
{
				private XKPLayout div1;
				private XKPCircle c1;
				private XKPRectangle r1;
				private XKPPolygon p1;
	ActFigures2D application;
			@Override
			public void onCreate(Bundle savedInstanceState) {
				super.onCreate(savedInstanceState);
                initCallers();
				initInstances();
				setContentView(R.layout.main);
				bindViews();
			}
			private void bindViews() {
				div1 = (XKPLayout) findViewById(R.id.div1);
				c1 = (XKPCircle) findViewById(R.id.c1);
					c1.setOnClickInsideFigureListener(this);
				r1 = (XKPRectangle) findViewById(R.id.r1);
					r1.setOnClickInsideFigureListener(this);
				p1 = (XKPPolygon) findViewById(R.id.p1);
					p1.setOnClickInsideFigureListener(this);
			p1.addPoint(100, 100);
			p1.addPoint(100, 50);
			p1.addPoint(190, 10);
			p1.addPoint(150, 200);
	}
void initCallers() {
}
			void initInstances() {
				application = this;
			}
						public void onClickInsideFigure(View v) {
							switch (v.getId()) {
									case R.id.c1:
										onClickInsideFigurec1();
										break;
									case R.id.r1:
										onClickInsideFigurer1();
										break;
									case R.id.p1:
										onClickInsideFigurep1();
										break;
						} //switch
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
