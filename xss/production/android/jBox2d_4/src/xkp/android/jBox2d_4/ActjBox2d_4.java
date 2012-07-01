package xkp.android.jBox2d_4;
import java.util.ArrayList;
import java.util.Arrays;
import android.view.View;
import android.view.ViewTreeObserver;
import android.view.ViewTreeObserver.OnGlobalLayoutListener;
			import android.app.Activity;
			import android.os.Bundle;
import xkp.android.libs.Layout.XKPLayout;
			import xkp.android.libs.Widget.XKPUtils;
						import android.view.View.OnKeyListener;
						import android.view.KeyEvent;
						import android.view.View.OnClickListener;
						import android.view.View.OnLongClickListener;
						import android.view.MotionEvent;
						import android.view.View.OnTouchListener;
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
						import xkp.android.libs.JBox2d.XKPSpawner;
        import java.util.Timer;
        import java.util.TimerTask;
        import xkp.android.libs.Sequence.*;
		public class ActjBox2d_4 
						extends Activity
{
				private good_guy hero;
				private floor __floor1;
				private TextView game_over;
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
				"__image4",
				"__image5",
				"__image6",
				"__audio1"
			};
			private String [] mResources___resources1_DroidName = {
				"smile_sad_48.png",
				"smile_grin_48.png",
				"sound_on.png",
				"sound_off.png",
				"stone_12.jpg",
				"bomb.png",
				"boom.wav"
			};
			private Integer [] mResources___resources1_Type = {
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.AUDIO_RESOURCE
			};
			private Integer [] mResources___resources1_Id = {
				R.drawable.smile_sad_48,
				R.drawable.smile_grin_48,
				R.drawable.sound_on,
				R.drawable.sound_off,
				R.drawable.stone_12,
				R.drawable.bomb,
				R.raw.boom
			};
	private boolean mLayoutStarted = false;
			private ActjBox2d_4 application;
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
				hero = (good_guy) findViewById(R.id.hero);
				__floor1 = (floor) findViewById(R.id.__floor1);
				game_over = (TextView) findViewById(R.id.game_over);
	}
private void initCallers() {
			util = new XKPUtils();
			ActjBox2d_4.util.addView(this);
			util = ActjBox2d_4.util;
        myWorld = new XKPJBox2d();
		myWorld.createWorld(new Vec2(0f, 0f));
			Start();
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
			XKPSpawner sp1 = new XKPSpawner( this, layoutapplication, "bad_guy", 300, 100 ); 
				sp1.setPosition(300, sp1.getPosition().y);
				sp1.setPosition(sp1.getPosition().x, 100);
				sp1.setFrequency(3);
			sp1.start();
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
			private double update_freq = 1/30.0;
			private void Update() {
				runOnUiThread(new Runnable() {
					public void run() {
						double delta = update_freq;
		Manager.getInstance().update(delta);
					}
				});
			};
			private void Start() {
				Timer timer = new Timer();
				timer.schedule(new TimerTask() {
					@Override
					public void run() {
						Update();
					}
				}, 0, (long)(update_freq * 1000)); 
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
	@Override	public boolean onKeyDown(int keycode, KeyEvent event)	{				if (keycode == LEFT_ARROW)
{
	hero.move_around.vx = -1;
}
if (keycode == RIGHT_ARROW)
{
	hero.move_around.vx = 1;
}
					return super.onKeyDown(keycode, event);	}
			private void keyupapplication(Object keycode) {
				if (keycode == LEFT_ARROW && hero.move_around.vx < 0)
{
	hero.move_around.vx = 0;
}
if (keycode == RIGHT_ARROW && hero.move_around.vx > 0)
{
	hero.move_around.vx = 0;
}
			}
			private void collisionhero() {
				game_over.setVisibility(true);
hero.move_around.stop();
util.setResourceXKPImage(hero, "smile_sad_48.png");
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
