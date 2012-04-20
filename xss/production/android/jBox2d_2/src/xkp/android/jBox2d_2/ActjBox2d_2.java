package xkp.android.jBox2d_2;
import java.util.ArrayList;
import java.util.Arrays;
import android.view.View;
import android.view.ViewTreeObserver;
import android.view.ViewTreeObserver.OnGlobalLayoutListener;
			import android.app.Activity;
			import android.os.Bundle;
import xkp.android.libs.Layout.XKPLayout;
			import xkp.android.libs.Widget.XKPUtils;
						import xkp.android.libs.Graphics.XKPGraphics.OnClickInsideFigureListener;
						import xkp.android.libs.Graphics.XKPImage;
						import xkp.android.libs.Widget.XKPUtils;
						import xkp.android.libs.Widget.XKPPackage;
						import xkp.android.libs.Widget.XKPPackage.OnResourcePackageListener;
						import org.jbox2d.common.Vec2;
						import org.jbox2d.dynamics.BodyType;
						import xkp.android.libs.JBox2d.*;
						import org.jbox2d.collision.shapes.ShapeType;
		public class ActjBox2d_2 
						extends Activity
{
				private XKPImage btn1;
			public static XKPUtils util;
		private XKPLayout layoutapplication;
		private XKPJBox2d 			myWorld;
		private android.os.Handler 	mHandler;
		private final double jBox2dFreq = 1 / 60.0f;
		private final Runnable mRunnableWorld = new Runnable() {
			public void run() {
				myWorld.update();
				mHandler.postDelayed(mRunnableWorld, (long) (jBox2dFreq * 1000));
			}
		};
			private XKPDDView 			ddView;
				private XKPPackage __resources1;
			private String [] mResources___resources1_XKPName = {
				"__image1",
				"__image2"
			};
			private String [] mResources___resources1_DroidName = {
				"smile_grin_48.png",
				"smile_sad_48.png"
			};
			private Integer [] mResources___resources1_Type = {
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE
			};
			private Integer [] mResources___resources1_Id = {
				R.drawable.smile_grin_48,
				R.drawable.smile_sad_48
			};
	private boolean mLayoutStarted = false;
			private ActjBox2d_2 application;
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
				btn1 = (XKPImage) findViewById(R.id.btn1);
	}
private void initCallers() {
			util = new XKPUtils();
			ActjBox2d_2.util.addView(this);
			util = ActjBox2d_2.util;
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
		myWorld.createWorld(new Vec2(0f, 10f), ddView.getDebugDraw());
		XKPPhysicBody __physics1 = new XKPPhysicBody(myWorld.getWorld(), 
			ShapeType.CIRCLE, btn1);
		__physics1.setSensor(false);
		__physics1.setDensity(1f);
		__physics1.setFriction(0.5f);
		__physics1.setRestitution(0.200000003f);
		__physics1.setBodyType(BodyType.DYNAMIC);
			__physics1.setPosition(btn1.getX1(), btn1.getY1());
		__physics1.createBody(btn1.getRadius());
		XKPPhysicBody floor = new XKPPhysicBody(myWorld.getWorld(), 
			ShapeType.POLYGON);
		floor.setSensor(false);
		floor.setDensity(1f);
		floor.setFriction(0.5f);
		floor.setRestitution(0.200000003f);
		floor.setBodyType(BodyType.STATIC);
			floor.setPosition(0, 380);
		floor.createBody(400, 2);
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
			private Object prop_application_i = 0;
}
