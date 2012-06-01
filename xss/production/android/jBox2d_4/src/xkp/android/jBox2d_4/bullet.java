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
		public class bullet 
						extends XKPImage
{
			public static XKPUtils util;
		private XKPImage layoutbullet;
			public XKPPhysicBody __physics1 = new XKPPhysicBody(ActjBox2d_4.myWorld.getWorld(), 
				ShapeType.CIRCLE, this);
	private boolean mLayoutStarted = false;
			public bullet(Context context) {
				this(context, null);
			}
			public bullet(Context context, AttributeSet attrs) {
				super(context, attrs);
				initCallers();
				LayoutInflater li = (LayoutInflater) context.
					getSystemService(Context.LAYOUT_INFLATER_SERVICE);
				// TIPS: link xml file with inflater
				XKPImage root = (XKPImage) li.inflate(R.layout.bullet, null);
				// TIPS: and with all exists views
					this.setFigureHeight(20);
					util.setResourceXKPImage(this, "bomb.png");
					this.setFigureWidth(20);
		// TIPS: create an observer for notify layout updated
		layoutbullet = (XKPImage) root.findViewById(R.id.layoutbullet);
		ViewTreeObserver mainLayoutViewTreeObserver = layoutbullet.getViewTreeObserver();
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
					attrs, R.styleable.bullet);
		__physics1.setSensor(false);
		__physics1.setDensity(1f);
		__physics1.setFriction(0.5f);
		__physics1.setRestitution(0.200000003f);
		__physics1.setBodyType(BodyType.DYNAMIC);
			__physics1.setPosition(this.getX1(), this.getY1());
		__physics1.createBody(this.getRadius());
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
			private void collisionbullet(Object who) {
				if (who instanceof floor)
{
	this.RemoveFromWorld();
	if (sound.active)
	{
		SoundUtils.play("boom.wav");
	}
}
			}
}
