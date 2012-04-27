package xkp.android.jBox2d_3;

import java.util.ArrayList;
import java.util.Arrays;
import android.view.View;
import android.view.ViewTreeObserver;
import android.view.ViewTreeObserver.OnGlobalLayoutListener;
import android.content.Context;
import android.content.res.TypedArray;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import xkp.android.libs.Layout.XKPLayout;
import xkp.android.libs.Widget.XKPUtils;
import xkp.android.libs.Graphics.XKPGraphics.OnClickInsideFigureListener;
import xkp.android.libs.Graphics.XKPImage;
import xkp.android.libs.Widget.XKPUtils;

import org.jbox2d.collision.shapes.ShapeType;
import org.jbox2d.common.Vec2;
import org.jbox2d.dynamics.BodyType;
import xkp.android.libs.JBox2d.*;

public class smiley_ball extends XKPImage {
	public static XKPUtils util;
	private XKPImage layoutsmiley_ball;
	private boolean mLayoutStarted = false;

	public smiley_ball(Context context) {
		this(context, null);
	}

	public smiley_ball(Context context, AttributeSet attrs) {
				super(context, attrs);
				initCallers();
				LayoutInflater li = (LayoutInflater) context.
					getSystemService(Context.LAYOUT_INFLATER_SERVICE);
				// TIPS: link xml file with inflater
				XKPImage root = (XKPImage) li.inflate(R.layout.smiley_ball, null);
				// TIPS: and with all exists views
		// TIPS: create an observer for notify layout updated
		layoutsmiley_ball = (XKPImage) root.findViewById(R.id.layoutsmiley_ball);
		ViewTreeObserver mainLayoutViewTreeObserver = layoutsmiley_ball.getViewTreeObserver();
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
			TypedArray ta = context.obtainStyledAttributes(
					attrs, R.styleable.smiley_ball);
			XKPPhysicBody __physics1 = new XKPPhysicBody(ActjBox2d_3.myWorld.getWorld(), 
				ShapeType.CIRCLE);
		__physics1.setSensor(false);
		__physics1.setDensity(1f);
		__physics1.setFriction(0.5f);
		__physics1.setRestitution(0.200000003f);
		__physics1.setBodyType(BodyType.DYNAMIC);
			__physics1.setPosition(, );
		__physics1.createBody(21.500000f);
			ta.recycle();
			}

	private void initCallers() {
		util = new XKPUtils();
		ActjBox2d_3.util.addView(this);
		util = ActjBox2d_3.util;
	}

	private void onLayoutUpdated() {
	}

	private void onLayoutStarted() {
	}

	private void collisionsmiley_ball(Object who) {
	}
}
