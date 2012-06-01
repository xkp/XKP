package xkp.android.jBox2d_3;
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
						import org.jbox2d.collision.shapes.ShapeType;
						import xkp.android.libs.JBox2d.XKPPhysicBody;
						import xkp.android.libs.JBox2d.XKPJBox2d;
						import xkp.android.libs.JBox2d.XKPDDView;
						import xkp.android.libs.JBox2d.XKPDebugDraw;
						import xkp.android.libs.JBox2d.XKPSpawner;
		public class ActjBox2d_3 
						extends Activity
{
				private XKPImage __img1;
				private XKPImage __img2;
				private XKPImage __img3;
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
				"__image2"
			};
			private String [] mResources___resources1_DroidName = {
				"smile_grin_48.png",
				"stone_12.jpg"
			};
			private Integer [] mResources___resources1_Type = {
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE
			};
			private Integer [] mResources___resources1_Id = {
				R.drawable.smile_grin_48,
				R.drawable.stone_12
			};
	private boolean mLayoutStarted = false;
			private ActjBox2d_3 application;
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
				__img1 = (XKPImage) findViewById(R.id.__img1);
				__img2 = (XKPImage) findViewById(R.id.__img2);
				__img3 = (XKPImage) findViewById(R.id.__img3);
	}
private void initCallers() {
			util = new XKPUtils();
			ActjBox2d_3.util.addView(this);
			util = ActjBox2d_3.util;
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
		myWorld.createWorld(new Vec2(0f, 10f));
			XKPSpawner sp1 = new XKPSpawner( this, layoutapplication, "smiley_ball", 100, 100, 95 ); 
				sp1.setPosition(100, sp1.getPosition().y);
				sp1.setPosition(sp1.getPosition().x, 100);
				sp1.setRotation(95);
				sp1.setFrequency(10);
				sp1.setLinearVelocity(5);
			sp1.start();
			XKPPhysicBody __physics2 = new XKPPhysicBody(ActjBox2d_3.myWorld.getWorld(), 
				ShapeType.POLYGON, __img1);
		__physics2.setSensor(false);
		__physics2.setDensity(1f);
		__physics2.setFriction(0.5f);
		__physics2.setRestitution(0.200000003f);
		__physics2.setBodyType(BodyType.STATIC);
			__physics2.setPosition(__img1.getX1(), __img1.getY1());
		__physics2.createBody(__img1.getDX(), __img1.getDY());
			XKPPhysicBody __physics3 = new XKPPhysicBody(ActjBox2d_3.myWorld.getWorld(), 
				ShapeType.POLYGON, __img2);
		__physics3.setSensor(false);
		__physics3.setDensity(1f);
		__physics3.setFriction(0.5f);
		__physics3.setRestitution(0.200000003f);
		__physics3.setBodyType(BodyType.STATIC);
			__physics3.setPosition(__img2.getX1(), __img2.getY1());
		__physics3.createBody(__img2.getDX(), __img2.getDY());
			XKPPhysicBody __physics4 = new XKPPhysicBody(ActjBox2d_3.myWorld.getWorld(), 
				ShapeType.POLYGON, __img3);
		__physics4.setSensor(false);
		__physics4.setDensity(1f);
		__physics4.setFriction(0.5f);
		__physics4.setRestitution(0.200000003f);
		__physics4.setBodyType(BodyType.STATIC);
			__physics4.setPosition(__img3.getX1(), __img3.getY1());
		__physics4.createBody(__img3.getDX(), __img3.getDY());
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
}
