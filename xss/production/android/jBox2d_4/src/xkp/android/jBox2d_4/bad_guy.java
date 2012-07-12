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
						import xkp.android.libs.JBox2d.XKPSpawner;
						import xkp.android.libs.Graphics.XKPGraphics.OnClickInsideFigureListener;
						import xkp.android.libs.Graphics.XKPImage;
						import xkp.android.libs.Widget.XKPUtils;
		public class bad_guy 
						extends XKPImage
{
				private move_action move_around;
				private Sequence fade_out;
			public static XKPUtils util;
		private XKPImage layoutbad_guy;
	private boolean mLayoutStarted = false;
			public bad_guy(Context context) {
				this(context, null);
			}
			public bad_guy(Context context, AttributeSet attrs) {
				super(context, attrs);
				initCallers();
				LayoutInflater li = (LayoutInflater) context.
					getSystemService(Context.LAYOUT_INFLATER_SERVICE);
				// TIPS: link xml file with inflater
				XKPImage root = (XKPImage) li.inflate(R.layout.bad_guy, null);
				// TIPS: and with all exists views
					this.setFigureHeight(60);
					util.setResourceXKPImage(this, "smile_sad_48.png");
					this.setFigureWidth(60);
		// TIPS: create an observer for notify layout updated
		layoutbad_guy = (XKPImage) root.findViewById(R.id.layoutbad_guy);
		ViewTreeObserver mainLayoutViewTreeObserver = layoutbad_guy.getViewTreeObserver();
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
				fade_out = (Sequence) root.findViewById(R.id.fade_out);
			TypedArray ta = context.obtainStyledAttributes(
					attrs, R.styleable.bad_guy);
			XKPPhysicBody __physics2 = new XKPPhysicBody(ActjBox2d_4.myWorld.getWorld(), 
				ShapeType.CIRCLE);
		__physics2.setSensor(false);
		__physics2.setDensity(1f);
		__physics2.setFriction(0.5f);
		__physics2.setRestitution(0.200000003f);
		__physics2.setBodyType(BodyType.DYNAMIC);
			__physics2.setPosition(null, null);
		__physics2.createBody(21.500000f);
			XKPSpawner cannon = new XKPSpawner( this, layoutapplication, "bullet", 20, 60, 0 ); 
				cannon.setPosition(20, cannon.getPosition().y);
				cannon.setPosition(cannon.getPosition().x, 60);
				cannon.setRotation(0);
				cannon.setFrequency(1);
				cannon.setLinearVelocity(2);
			cannon.start();
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
			target = bad_guy;
target = bad_guy;
		}
			private void collisionbad_guy(Object who) {
				if (who instanceof bad_guy)
{
	this.RemoveFromWorld(true);
	cannon.stop();
	fade_out.start();
}
			}
}
