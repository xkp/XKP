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
						import org.jbox2d.collision.shapes.ShapeType;
						import xkp.android.libs.JBox2d.XKPPhysicBody;
						import xkp.android.libs.JBox2d.XKPJBox2d;
						import xkp.android.libs.JBox2d.XKPDDView;
						import xkp.android.libs.JBox2d.XKPDebugDraw;
		public class ActjBox2d_2 
						extends Activity
{
				private XKPImage btn1;
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
			private XKPDDView 			ddView;
<<<<<<< HEAD
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
=======
				private XKPPackage ;
			private String [] mResources__XKPName = {
				"",
				""
			};
			private String [] mResources__DroidName = {
				"smile_grin_48.png",
				"smile_sad_48.png"
			};
			private Integer [] mResources__Type = {
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE
			};
			private Integer [] mResources__Id = {
				R.drawable.smile_grin_48,
				R.drawable.smile_sad_48
			};
>>>>>>> 48b4121a3614a3e7cdb142311c9a6460262befd0
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
        myWorld = new XKPJBox2d();
		myWorld.createWorld(new Vec2(0f, 10f), ddView.getDebugDraw());
			XKPPhysicBody __physics1 = new XKPPhysicBody(ActjBox2d_2.myWorld.getWorld(), 
				ShapeType.CIRCLE, btn1);
		__physics1.setSensor(false);
		__physics1.setDensity(1f);
		__physics1.setFriction(0.5f);
		__physics1.setRestitution(0.200000003f);
		__physics1.setBodyType(BodyType.DYNAMIC);
			__physics1.setPosition(btn1.getX1(), btn1.getY1());
		__physics1.createBody(btn1.getRadius());
			XKPPhysicBody floor = new XKPPhysicBody(ActjBox2d_2.myWorld.getWorld(), 
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
<<<<<<< HEAD
				__resources1 = new XKPPackage(this, 
					mResources___resources1_XKPName, mResources___resources1_DroidName, 
					mResources___resources1_Type, mResources___resources1_Id
					, true);
				util.addXKPPackage(__resources1);
=======
				 = new XKPPackage(this, 
					mResources__XKPName, mResources__DroidName, 
					mResources__Type, mResources__Id
					, true);
				util.addXKPPackage();
>>>>>>> 48b4121a3614a3e7cdb142311c9a6460262befd0
			}
			private Object i = 0;
}
