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
		public class floor 
						extends XKPImage
{
			public static XKPUtils util;
		private XKPImage layoutfloor;
			public XKPPhysicBody __physics4 = new XKPPhysicBody(ActjBox2d_4.myWorld.getWorld(), 
				ShapeType.POLYGON, this);
	private boolean mLayoutStarted = false;
			public floor(Context context) {
				this(context, null);
			}
			public floor(Context context, AttributeSet attrs) {
				super(context, attrs);
				initCallers();
				LayoutInflater li = (LayoutInflater) context.
					getSystemService(Context.LAYOUT_INFLATER_SERVICE);
				// TIPS: link xml file with inflater
				XKPImage root = (XKPImage) li.inflate(R.layout.floor, null);
				// TIPS: and with all exists views
					util.setResourceXKPImage(this, "stone_12.jpg");
		// TIPS: create an observer for notify layout updated
		layoutfloor = (XKPImage) root.findViewById(R.id.layoutfloor);
		ViewTreeObserver mainLayoutViewTreeObserver = layoutfloor.getViewTreeObserver();
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
					attrs, R.styleable.floor);
		__physics4.setSensor(false);
		__physics4.setDensity(1f);
		__physics4.setFriction(0.5f);
		__physics4.setRestitution(0.200000003f);
		__physics4.setBodyType(BodyType.STATIC);
			__physics4.setPosition(this.getX1(), this.getY1());
		__physics4.createBody(this.getDX(), this.getDY());
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
}
