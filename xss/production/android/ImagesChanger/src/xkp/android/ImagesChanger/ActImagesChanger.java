package xkp.android.ImagesChanger;
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
						import android.widget.ImageView;
						import xkp.android.libs.Widget.XKPUtils;
						import android.text.method.KeyListener;
						import android.widget.Button;
						import android.widget.TextView;
						import xkp.android.libs.Widget.XKPPackage;
						import xkp.android.libs.Widget.XKPPackage.OnResourcePackageListener;
		public class ActImagesChanger 
						extends Activity
{
				private ImageView album;
				private Button changer;
				private TextView identifier;
			public static XKPUtils util;
		private XKPLayout layoutapplication;
				private XKPPackage ;
			private String [] mResources__XKPName = {
				"img1",
				"img2",
				"img3",
				"img4",
				"img5",
				"img6",
				"img7",
				"img8"
			};
			private String [] mResources__DroidName = {
				"photo_1.jpg",
				"photo_2.jpg",
				"photo_3.jpg",
				"photo_4.jpg",
				"photo_5.jpg",
				"photo_6.jpg",
				"photo_7.jpg",
				"photo_8.jpg"
			};
			private Integer [] mResources__Type = {
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE
			};
			private Integer [] mResources__Id = {
				R.drawable.photo_1,
				R.drawable.photo_2,
				R.drawable.photo_3,
				R.drawable.photo_4,
				R.drawable.photo_5,
				R.drawable.photo_6,
				R.drawable.photo_7,
				R.drawable.photo_8
			};
	private boolean mLayoutStarted = false;
			private ActImagesChanger application;
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
				album = (ImageView) findViewById(R.id.album);
				changer = (Button) findViewById(R.id.changer);
				changer.setOnClickListener(new OnClickListener() {
					@Override
					public void onClick(View v) {
						onClickchanger();
					}
				});
				identifier = (TextView) findViewById(R.id.identifier);
	}
private void initCallers() {
			util = new XKPUtils();
			ActImagesChanger.util.addView(this);
			util = ActImagesChanger.util;
}
			@Override
			public void onPause() {
				super.onPause();
			}
			@Override
			public void onStop() {
				super.onStop();
			}
			@Override
			public void onResume() {
				super.onResume();
			}
private void onLayoutUpdated() {
}
private void onLayoutStarted() {
}
			private void initInstances() {
				application = this;
				 = new XKPPackage(this, 
					mResources__XKPName, mResources__DroidName, 
					mResources__Type, mResources__Id
					, true);
				util.addXKPPackage();
			}
			private void onClickchanger() {
				currentImage = currentImage + 1;
currentImage = currentImage % resources.size();
String currentResource = resources.get(currentImage);
util.setResourceImageView(album, currentResource);
identifier.setText(currentResource);
			}
			private Integer currentImage = 0;
			private ArrayList<String> resources = new ArrayList(Arrays.asList( new Object [] {"img1", "img2", "img3", "img4", "img5", "img6", "img7", "img8"} ));
}
