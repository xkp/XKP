package xkp.android.jBox2d_4;
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
						import org.jbox2d.common.Vec2;
						import org.jbox2d.dynamics.BodyType;
						import org.jbox2d.collision.shapes.ShapeType;
						import xkp.android.libs.JBox2d.XKPPhysicBody;
						import xkp.android.libs.Graphics.XKPGraphics.OnClickInsideFigureListener;
						import xkp.android.libs.Graphics.XKPImage;
						import xkp.android.libs.Widget.XKPUtils;
		public class good_guy 
						extends XKPImage
{
				private move_action move_around;
			public static XKPUtils util;
		private XKPImage layoutgood_guy;
	private boolean mLayoutStarted = false;
			public good_guy(Context context) {
				this(context, null);
			}
			public good_guy(Context context, AttributeSet attrs) {
				super(context, attrs);
				initCallers();
				LayoutInflater li = (LayoutInflater) context.
					getSystemService(Context.LAYOUT_INFLATER_SERVICE);
				// TIPS: link xml file with inflater
				XKPImage root = (XKPImage) li.inflate(R.layout.good_guy, null);
				// TIPS: and with all exists views
					this.setFigureHeight(30);
					util.setResourceXKPImage(this, "smile_grin_48.png");
					this.setFigureWidth(30);
		// TIPS: create an observer for notify layout updated
		layoutgood_guy = (XKPImage) root.findViewById(R.id.layoutgood_guy);
		ViewTreeObserver mainLayoutViewTreeObserver = layoutgood_guy.getViewTreeObserver();
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
				move_around = (move_action) root.findViewById(R.id.move_around);
			TypedArray ta = context.obtainStyledAttributes(
					attrs, R.styleable.good_guy);
			XKPPhysicBody __physics3 = new XKPPhysicBody(ActjBox2d_4.myWorld.getWorld(), 
				ShapeType.CIRCLE);
		__physics3.setSensor(true);
		__physics3.setDensity(1f);
		__physics3.setFriction(0.5f);
		__physics3.setRestitution(0.200000003f);
		__physics3.setBodyType(BodyType.DYNAMIC);
			__physics3.setPosition(null, null);
		__physics3.createBody(21.500000f);
			ta.recycle();
			}
private void initCallers() {
			util = new XKPUtils();
			ActjBox2d_4.util.addView(this);
			util = ActjBox2d_4.util;
}
private void onLayoutUpdated() {
}
private void onLayoutStarted() {
}
		public void init() {
			target = good_guy;
		}
			private void collisiongood_guy() {
			}
}
