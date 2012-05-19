package xkp.android.jBox2d_5;
import java.util.ArrayList;
import java.util.Arrays;
import android.view.View;
import android.view.ViewTreeObserver;
import android.view.ViewTreeObserver.OnGlobalLayoutListener;
			import android.app.Activity;
			import android.os.Bundle;
import xkp.android.libs.Layout.XKPLayout;
			import xkp.android.libs.Widget.XKPUtils;
						import android.view.View.OnClickListener;
						import android.view.View.OnLongClickListener;
						import android.view.MotionEvent;
						import android.view.View.OnTouchListener;
						import android.view.View.OnKeyListener;
						import android.view.View.OnFocusChangeListener;
						import android.text.method.KeyListener;
						import android.widget.TextView;
						import xkp.android.libs.Widget.XKPPackage;
						import xkp.android.libs.Widget.XKPPackage.OnResourcePackageListener;
						import org.jbox2d.common.Vec2;
						import org.jbox2d.dynamics.BodyType;
						import org.jbox2d.collision.shapes.ShapeType;
						import xkp.android.libs.JBox2d.XKPPhysicBody;
						import xkp.android.libs.JBox2d.XKPJBox2d;
						import xkp.android.libs.JBox2d.XKPDDView;
						import xkp.android.libs.JBox2d.XKPDebugDraw;
						import xkp.android.libs.JBox2d.XKPDistanceJoint;
						import xkp.android.libs.JBox2d.XKPWeldJoint;
						import xkp.android.libs.JBox2d.XKPPulleyJoint;
		public class ActjBox2d_5 
						extends Activity
{
				private TextView __label1;
				private TextView __label2;
				private TextView __label3;
				private TextView __label4;
				private TextView __label5;
				private body_1 btn1;
				private body_1 btn2;
				private body_2 btn3;
				private body_2 btn4;
				private body_3 btn5;
				private body_3 btn6;
				private floor __floor1;
				private floor __floor2;
				private floor __floor3;
				private floor __floor4;
			public static XKPUtils util;
		private XKPLayout layoutapplication;
		public static XKPJBox2d 	myWorld;
		private android.os.Handler 	mHandler;
		private final double jBox2dFreq = 1 / 60.0f;
		private final Runnable mRunnableWorld = new Runnable() {
			public void run() {
				myWorld.update();
				mHandler.postDelayed(mRunnableWorld, (long) (jBox2dFreq * 1000));
			}
		};
				private XKPPackage __resources1;
			private String [] mResources___resources1_XKPName = {
				"__image1",
				"__image2",
				"__image3",
				"__image4"
			};
			private String [] mResources___resources1_DroidName = {
				"smile_sad_48.png",
				"smile_grin_48.png",
				"stone_12.jpg",
				"img1.png"
			};
			private Integer [] mResources___resources1_Type = {
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE
			};
			private Integer [] mResources___resources1_Id = {
				R.drawable.smile_sad_48,
				R.drawable.smile_grin_48,
				R.drawable.stone_12,
				R.drawable.img1
			};
	private boolean mLayoutStarted = false;
			private ActjBox2d_5 application;
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
				__label1 = (TextView) findViewById(R.id.__label1);
				__label2 = (TextView) findViewById(R.id.__label2);
				__label3 = (TextView) findViewById(R.id.__label3);
				__label4 = (TextView) findViewById(R.id.__label4);
				__label5 = (TextView) findViewById(R.id.__label5);
				btn1 = (body_1) findViewById(R.id.btn1);
				btn2 = (body_1) findViewById(R.id.btn2);
				btn3 = (body_2) findViewById(R.id.btn3);
				btn4 = (body_2) findViewById(R.id.btn4);
				btn5 = (body_3) findViewById(R.id.btn5);
				btn6 = (body_3) findViewById(R.id.btn6);
				__floor1 = (floor) findViewById(R.id.__floor1);
				__floor2 = (floor) findViewById(R.id.__floor2);
				__floor3 = (floor) findViewById(R.id.__floor3);
				__floor4 = (floor) findViewById(R.id.__floor4);
	}
private void initCallers() {
			util = new XKPUtils();
			ActjBox2d_5.util.addView(this);
			util = ActjBox2d_5.util;
}
			@Override
			public void onPause() {
				super.onPause();
		if(mHandler != null) mHandler.removeCallbacks(mRunnableWorld);
			}
			@Override
			public void onStop() {
				super.onStop();
		if(mHandler != null) mHandler.removeCallbacks(mRunnableWorld);
			}
			@Override
			public void onResume() {
				super.onResume();
		if(mHandler != null) mHandler.post(mRunnableWorld);
			}
private void onLayoutUpdated() {
}
private void onLayoutStarted() {
        myWorld = new XKPJBox2d();
		myWorld.createWorld(new Vec2(0f, 3f));
			XKPDistanceJoint __distance_joint1 = new XKPDistanceJoint( ActjBox2d_5.myWorld.getWorld() ); 
				__distance_joint1.setBodyA("btn1");
				__distance_joint1.setBodyB("btn2");
				__distance_joint1.setLength(5);
		__distance_joint1.createJoint();
			XKPWeldJoint __weld_joint1 = new XKPWeldJoint( ActjBox2d_5.myWorld.getWorld() ); 
				__weld_joint1.setBodyA("btn3");
				__weld_joint1.setBodyB("btn4");
		__weld_joint1.createJoint();
			XKPPulleyJoint __pulley_joint1 = new XKPPulleyJoint( ActjBox2d_5.myWorld.getWorld() ); 
				__pulley_joint1.setBodyA("btn5");
				__pulley_joint1.setBodyB("btn6");
				__pulley_joint1.setGroundAnchorA(new Vec2(100, __pulley_joint1.getGroundAnchorA().y));
				__pulley_joint1.setGroundAnchorA(new Vec2(__pulley_joint1.getGroundAnchorA().x, 100));
				__pulley_joint1.setGroundAnchorB(new Vec2(500, __pulley_joint1.getGroundAnchorB().y));
				__pulley_joint1.setGroundAnchorB(new Vec2(__pulley_joint1.getGroundAnchorB().x, 500));
				__pulley_joint1.setRatio(1);
		__pulley_joint1.createJoint();
        mHandler = new android.os.Handler();
        mHandler.post(mRunnableWorld);
}
			private void initInstances() {
				application = this;
				__resources1 = new XKPPackage(this, 
					mResources___resources1_XKPName, mResources___resources1_DroidName, 
					mResources___resources1_Type, mResources___resources1_Id
					, true);
				util.addXKPPackage(__resources1);
			}
			private void keydownapplication(Object keycode) {
				if (keycode == LEFT_ARROW)
{
	btn1.setX1(btn1.getX1() - 5);
}
if (keycode == RIGHT_ARROW)
{
	btn1.setX1(btn1.getX1() + 5);
}
if (keycode == UP_ARROW)
{
	btn1.setY1(btn1.getY1() - 5);
}
if (keycode == DOWN_ARROW)
{
	btn1.setY1(btn1.getY1() + 5);
}
			}
}
