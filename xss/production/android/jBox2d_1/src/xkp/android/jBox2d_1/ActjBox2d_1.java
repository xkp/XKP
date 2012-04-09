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
						import xkp.android.libs.JBox2d.*;
		public class ActjBox2d_1 
						extends Activity
{
		private XKPLayout layoutapplication;
		private XKPJBox2d 			myWorld;
		private XKPDDView 			ddView;
		private android.os.Handler 	mHandler;
		private final double jBox2dFreq = 1 / 60.0f;
		private final Runnable mRunnableWorld = new Runnable() {
			public void run() {
				myWorld.update();
				mHandler.postDelayed(mRunnableWorld, (long) (jBox2dFreq * 1000));
			}
		};
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
}
			@Override
			public void onPause() {
				super.onPause();
		mHandler.removeCallbacks(mRunnableWorld);
			}
			@Override
			public void onStop() {
				super.onStop();
		mHandler.removeCallbacks(mRunnableWorld);
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
        myWorld = new XKPJBox2d();
		myWorld.createWorld(new Vec2(0f, 5f), ddView.getDebugDraw());
				XKPPhysicBody body_1 = new XKPPhysicBody(myWorld.getWorld());
				body_1.createCircleBody(90f, 160f, 20f, 
					0f, BodyType.DYNAMIC, 
					1f, 0.5f, 0.200000003f);
				XKPPhysicBody body_2 = new XKPPhysicBody(myWorld.getWorld());
				body_2.createBoxBody(30f, 24f, 20f, 30f, 
					45f, BodyType.DYNAMIC, 
					1f, 0.5f, 0.200000003f);
				XKPPhysicBody body_3 = new XKPPhysicBody(myWorld.getWorld());
				body_3.createCircleBody(130f, 80f, 30f, 
					0f, BodyType.DYNAMIC, 
					1f, 0.5f, 0.200000003f);
				XKPPhysicBody body_4 = new XKPPhysicBody(myWorld.getWorld());
				body_4.createBoxBody(150f, 180f, 50f, 20f, 
					0f, BodyType.DYNAMIC, 
					1f, 0.5f, 0.200000003f);
				XKPPhysicBody body_5 = new XKPPhysicBody(myWorld.getWorld());
				body_5.createCircleBody(200f, 100f, 10f, 
					0f, BodyType.DYNAMIC, 
					1f, 0.5f, 0.200000003f);
				XKPPhysicBody floor_1 = new XKPPhysicBody(myWorld.getWorld());
				floor_1.createBoxBody(0f, 0f, 10f, 400f, 
					0f, BodyType.STATIC, 
					1f, 0.5f, 0.200000003f);
				XKPPhysicBody floor_2 = new XKPPhysicBody(myWorld.getWorld());
				floor_2.createBoxBody(0f, 0f, 300f, 10f, 
					0f, BodyType.STATIC, 
					1f, 0.5f, 0.200000003f);
				XKPPhysicBody floor_3 = new XKPPhysicBody(myWorld.getWorld());
				floor_3.createBoxBody(300f, 10f, 10f, 400f, 
					0f, BodyType.STATIC, 
					1f, 0.5f, 0.200000003f);
				XKPPhysicBody floor_4 = new XKPPhysicBody(myWorld.getWorld());
				floor_4.createBoxBody(10f, 400f, 300f, 10f, 
					0f, BodyType.STATIC, 
					1f, 0.5f, 0.200000003f);
        mHandler = new android.os.Handler();
        mHandler.post(mRunnableWorld);
}
			private void initInstances() {
				application = this;
			}
}
