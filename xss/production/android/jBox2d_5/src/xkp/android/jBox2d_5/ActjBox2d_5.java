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
						import android.view.KeyEvent;
						import android.view.View.OnFocusChangeListener;
						import android.text.method.KeyListener;
						import android.widget.TextView;
						import xkp.android.libs.Widget.XKPPackage;
						import xkp.android.libs.Widget.XKPPackage.OnResourcePackageListener;
import org.jbox2d.callbacks.QueryCallback;
import org.jbox2d.collision.AABB;
import org.jbox2d.dynamics.Body;
import org.jbox2d.dynamics.BodyDef;
import org.jbox2d.dynamics.Fixture;
import org.jbox2d.dynamics.joints.MouseJoint;
import org.jbox2d.dynamics.joints.MouseJointDef;
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
		private Body groundBody;
		private MouseJoint mouseJoint = null;
		private final AABB queryAABB = new AABB();
		private final XKPQueryCallback callback = new XKPQueryCallback();
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
        myWorld = new XKPJBox2d();
		myWorld.createWorld(new Vec2(0f, 3f));
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
		// create body ground
		BodyDef bodyDef = new BodyDef();
		groundBody = myWorld.getWorld().createBody(bodyDef);
			XKPDistanceJoint __distance_joint1 = new XKPDistanceJoint( ActjBox2d_5.myWorld.getWorld() ); 
				__distance_joint1.setBodyA(btn1);
				__distance_joint1.setBodyB(btn2);
				__distance_joint1.setLength(5);
		__distance_joint1.createJoint();
			XKPWeldJoint __weld_joint1 = new XKPWeldJoint( ActjBox2d_5.myWorld.getWorld() ); 
				__weld_joint1.setBodyA(btn3);
				__weld_joint1.setBodyB(btn4);
		__weld_joint1.createJoint();
			XKPPulleyJoint __pulley_joint1 = new XKPPulleyJoint( ActjBox2d_5.myWorld.getWorld() ); 
				__pulley_joint1.setBodyA(btn5);
				__pulley_joint1.setBodyB(btn6);
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
		@Override
		public boolean onTouchEvent(MotionEvent event) {
			Vec2 p = new Vec2(event.getX(), event.getY());
			switch(event.getAction()) {
			case MotionEvent.ACTION_DOWN:
				break;
			case MotionEvent.ACTION_UP:
				if (mouseJoint != null) {
					myWorld.getWorld().destroyJoint(mouseJoint);
					mouseJoint = null;
				}
				break;
			case MotionEvent.ACTION_MOVE:
				if (mouseJoint != null) {
					mouseJoint.setTarget(p);
					break;
				}
				queryAABB.lowerBound.set(p.x - .001f, p.y - .001f);
				queryAABB.upperBound.set(p.x + .001f, p.y + .001f);
				callback.point.set(p);
				callback.fixture = null;
				myWorld.getWorld().queryAABB(callback, queryAABB);
				if (callback.fixture != null) {
					Body body = callback.fixture.getBody();
					XKPPhysicBody physicBody = (XKPPhysicBody)body.getUserData();
					if(!physicBody.getMouseJoint()) break;
					MouseJointDef def = new MouseJointDef();
					def.bodyA = groundBody;
					def.bodyB = body;
					def.target.set(p);
					def.maxForce = 3000f * body.getMass();
					def.dampingRatio = 0;
					def.frequencyHz = 1000;
					mouseJoint = (MouseJoint) myWorld.getWorld().createJoint(def);
					body.setAwake(true);
				}
				break;
			}
			return super.onTouchEvent(event);
		}
			private void keydownapplication(Object keycode) {
				if (keycode == KeyEvent.KEYCODE_DPAD_LEFT)
{
	btn1.setLeft(btn1.getX1() - 5);
}
if (keycode == KeyEvent.KEYCODE_DPAD_RIGHT)
{
	btn1.setLeft(btn1.getX1() + 5);
}
if (keycode == KeyEvent.KEYCODE_DPAD_UP)
{
	btn1.setTop(btn1.getY1() - 5);
}
if (keycode == KeyEvent.KEYCODE_DPAD_DOWN)
{
	btn1.setTop(btn1.getY1() + 5);
}
			}
		class XKPQueryCallback implements QueryCallback {
			public final Vec2 point;
			public Fixture fixture;
			public XKPQueryCallback() {
				point = new Vec2();
				fixture = null;
			}
			public boolean reportFixture(Fixture fixture) {
				Body body = fixture.getBody();
				if (body.getType() == BodyType.DYNAMIC) {
					boolean inside = fixture.testPoint(point);
					if (inside) {
						this.fixture = fixture;
						return false;
					}
				}
				return true;
			}
		}
}
