package xkp.android.jBox2d_1;
import java.util.ArrayList;
import java.util.Arrays;
import android.view.View;
import android.view.ViewTreeObserver;
import android.view.ViewTreeObserver.OnGlobalLayoutListener;
			import android.app.Activity;
			import android.os.Bundle;
import xkp.android.libs.Layout.XKPLayout;
						import org.jbox2d.common.Vec2;
						import org.jbox2d.dynamics.BodyType;
						import org.jbox2d.collision.shapes.ShapeType;
						import xkp.android.libs.JBox2d.XKPPhysicBody;
						import xkp.android.libs.JBox2d.XKPJBox2d;
						import xkp.android.libs.JBox2d.XKPDDView;
						import xkp.android.libs.JBox2d.XKPDebugDraw;
		public class ActjBox2d_1 
						extends Activity
{
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
			private XKPDDView 			ddView;
	private boolean mLayoutStarted = false;
			private ActjBox2d_1 application;
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
	}
private void initCallers() {
        myWorld = new XKPJBox2d();
		myWorld.createWorld(new Vec2(0f, 5f), ddView.getDebugDraw());
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
			//TODO: determine real screen dimensions (width & height)
			ddView = new XKPDDView(this, 320, 480);
			layoutapplication.addView(ddView);
			XKPPhysicBody body_1 = new XKPPhysicBody(ActjBox2d_1.myWorld.getWorld(), 
				ShapeType.CIRCLE);
		body_1.setSensor(false);
		body_1.setDensity(1f);
		body_1.setFriction(0.5f);
		body_1.setRestitution(0.200000003f);
		body_1.setBodyType(BodyType.DYNAMIC);
			body_1.setPosition(50, 200);
		body_1.createBody(20);
			XKPPhysicBody body_2 = new XKPPhysicBody(ActjBox2d_1.myWorld.getWorld(), 
				ShapeType.POLYGON);
		body_2.setSensor(false);
		body_2.setDensity(1f);
		body_2.setFriction(0.5f);
		body_2.setRestitution(0.200000003f);
		body_2.setBodyType(BodyType.DYNAMIC);
			body_2.setPosition(30, 24);
		body_2.createBody(20, 30);
			XKPPhysicBody body_3 = new XKPPhysicBody(ActjBox2d_1.myWorld.getWorld(), 
				ShapeType.CIRCLE);
		body_3.setSensor(false);
		body_3.setDensity(1f);
		body_3.setFriction(0.5f);
		body_3.setRestitution(0.200000003f);
		body_3.setBodyType(BodyType.DYNAMIC);
			body_3.setPosition(100, 60);
		body_3.createBody(30);
			XKPPhysicBody body_4 = new XKPPhysicBody(ActjBox2d_1.myWorld.getWorld(), 
				ShapeType.POLYGON);
		body_4.setSensor(false);
		body_4.setDensity(1f);
		body_4.setFriction(0.5f);
		body_4.setRestitution(0.200000003f);
		body_4.setBodyType(BodyType.DYNAMIC);
			body_4.setPosition(150, 180);
		body_4.createBody(50, 20);
			XKPPhysicBody body_5 = new XKPPhysicBody(ActjBox2d_1.myWorld.getWorld(), 
				ShapeType.CIRCLE);
		body_5.setSensor(false);
		body_5.setDensity(1f);
		body_5.setFriction(0.5f);
		body_5.setRestitution(0.200000003f);
		body_5.setBodyType(BodyType.DYNAMIC);
			body_5.setPosition(200, 100);
		body_5.createBody(10);
			XKPPhysicBody floor_1 = new XKPPhysicBody(ActjBox2d_1.myWorld.getWorld(), 
				ShapeType.POLYGON);
		floor_1.setSensor(false);
		floor_1.setDensity(1f);
		floor_1.setFriction(0.5f);
		floor_1.setRestitution(0.200000003f);
		floor_1.setBodyType(BodyType.STATIC);
			floor_1.setPosition(0, 0);
		floor_1.createBody(10, 400);
			XKPPhysicBody floor_2 = new XKPPhysicBody(ActjBox2d_1.myWorld.getWorld(), 
				ShapeType.POLYGON);
		floor_2.setSensor(false);
		floor_2.setDensity(1f);
		floor_2.setFriction(0.5f);
		floor_2.setRestitution(0.200000003f);
		floor_2.setBodyType(BodyType.STATIC);
			floor_2.setPosition(0, 0);
		floor_2.createBody(300, 10);
			XKPPhysicBody floor_3 = new XKPPhysicBody(ActjBox2d_1.myWorld.getWorld(), 
				ShapeType.POLYGON);
		floor_3.setSensor(false);
		floor_3.setDensity(1f);
		floor_3.setFriction(0.5f);
		floor_3.setRestitution(0.200000003f);
		floor_3.setBodyType(BodyType.STATIC);
			floor_3.setPosition(300, 10);
		floor_3.createBody(10, 400);
			XKPPhysicBody floor_4 = new XKPPhysicBody(ActjBox2d_1.myWorld.getWorld(), 
				ShapeType.POLYGON);
		floor_4.setSensor(false);
		floor_4.setDensity(1f);
		floor_4.setFriction(0.5f);
		floor_4.setRestitution(0.200000003f);
		floor_4.setBodyType(BodyType.STATIC);
			floor_4.setPosition(10, 400);
		floor_4.createBody(300, 10);
        mHandler = new android.os.Handler();
        mHandler.post(mRunnableWorld);
}
			private void initInstances() {
				application = this;
			}
}
