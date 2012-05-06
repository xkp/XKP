package xkp.android.jBox2d_5;
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
		public class body_2 
						extends XKPImage
{
			public static XKPUtils util;
		private XKPImage layoutbody_2;
			public XKPPhysicBody __physics2 = new XKPPhysicBody(ActjBox2d_5.myWorld.getWorld(), 
				ShapeType.CIRCLE, this);
	private boolean mLayoutStarted = false;
			public body_2(Context context) {
				this(context, null);
			}
			public body_2(Context context, AttributeSet attrs) {
				super(context, attrs);
				initCallers();
				LayoutInflater li = (LayoutInflater) context.
					getSystemService(Context.LAYOUT_INFLATER_SERVICE);
				// TIPS: link xml file with inflater
				XKPImage root = (XKPImage) li.inflate(R.layout.body_2, null);
				// TIPS: and with all exists views
					this.setFigureHeight(50);
					util.setResourceXKPImage(this, "smile_sad_48.png");
					this.setFigureWidth(50);
		// TIPS: create an observer for notify layout updated
		layoutbody_2 = (XKPImage) root.findViewById(R.id.layoutbody_2);
		ViewTreeObserver mainLayoutViewTreeObserver = layoutbody_2.getViewTreeObserver();
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
					attrs, R.styleable.body_2);
		__physics2.setSensor(false);
		__physics2.setDensity(1f);
		__physics2.setFriction(0.5f);
		__physics2.setRestitution(0.200000003f);
		__physics2.setBodyType(BodyType.DYNAMIC);
			__physics2.setPosition(this.getX1(), this.getY1());
		__physics2.createBody(this.getRadius());
			ta.recycle();
			}
private void initCallers() {
			util = new XKPUtils();
			ActjBox2d_5.util.addView(this);
			util = ActjBox2d_5.util;
}
private void onLayoutUpdated() {
}
private void onLayoutStarted() {
}
}
