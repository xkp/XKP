package xkp.android.Maps;
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
						import android.widget.ImageButton;
						import android.text.method.KeyListener;
						import android.widget.TextView;
						import xkp.android.libs.Widget.XKPSwitcher;
						import xkp.android.libs.Widget.XKPImageMap;
						import xkp.android.libs.Widget.XKPPackage;
						import xkp.android.libs.Widget.XKPPackage.OnResourcePackageListener;
		public class ActMaps 
						extends Activity
{
				private XKPLayout __div1;
				private ImageButton btnGallery;
				private ImageButton btnMaps;
				private TextView lblStatus;
				private TextView lblInformation;
				private XKPSwitcher swtScene;
				private TextView lblSwicherInformation;
				private ImageView imgGallery;
				private XKPImageMap mapController;
			public static XKPUtils util;
		private XKPLayout layoutapplication;
				private XKPPackage ;
				private XKPPackage photos;
				private XKPPackage maps;
			private String [] mResources_photos_XKPName = {
				"worth_1",
				"worth_2",
				"worth_3",
				"worth_4",
				"worth_5",
				"worth_6",
				"worth_7"
			};
			private String [] mResources_photos_DroidName = {
				"worth_1.jpg",
				"worth_2.jpg",
				"worth_3.jpg",
				"worth_4.jpg",
				"worth_5.jpg",
				"worth_6.jpg",
				"worth_7.jpg"
			};
			private Integer [] mResources_photos_Type = {
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE
			};
			private Integer [] mResources_photos_Id = {
				R.drawable.worth_1,
				R.drawable.worth_2,
				R.drawable.worth_3,
				R.drawable.worth_4,
				R.drawable.worth_5,
				R.drawable.worth_6,
				R.drawable.worth_7
			};
			private String [] mResources_maps_XKPName = {
				"holguin"
			};
			private String [] mResources_maps_DroidName = {
				"holguinmap.jpg"
			};
			private Integer [] mResources_maps_Type = {
				XKPPackage.BITMAP_RESOURCE
			};
			private Integer [] mResources_maps_Id = {
				R.drawable.holguinmap
			};
			private String [] mResources__XKPName = {
				"ico_gallery",
				"ico_maps"
			};
			private String [] mResources__DroidName = {
				"launcher_gallery.png",
				"launcher_maps.png"
			};
			private Integer [] mResources__Type = {
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE
			};
			private Integer [] mResources__Id = {
				R.drawable.launcher_gallery,
				R.drawable.launcher_maps
			};
	private boolean mLayoutStarted = false;
			private ActMaps application;
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
				__div1 = (XKPLayout) findViewById(R.id.__div1);
				btnGallery = (ImageButton) findViewById(R.id.btnGallery);
				btnGallery.setOnClickListener(new OnClickListener() {
					@Override
					public void onClick(View v) {
						onClickbtnGallery();
					}
				});
				btnMaps = (ImageButton) findViewById(R.id.btnMaps);
				btnMaps.setOnClickListener(new OnClickListener() {
					@Override
					public void onClick(View v) {
						onClickbtnMaps();
					}
				});
				lblStatus = (TextView) findViewById(R.id.lblStatus);
				lblInformation = (TextView) findViewById(R.id.lblInformation);
				swtScene = (XKPSwitcher) findViewById(R.id.swtScene);
				lblSwicherInformation = (TextView) findViewById(R.id.lblSwicherInformation);
				imgGallery = (ImageView) findViewById(R.id.imgGallery);
				imgGallery.setOnClickListener(new OnClickListener() {
					@Override
					public void onClick(View v) {
						onClickimgGallery();
					}
				});
				mapController = (XKPImageMap) findViewById(R.id.mapController);
	}
private void initCallers() {
			util = new XKPUtils();
			ActMaps.util.addView(this);
			util = ActMaps.util;
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
				photos = new XKPPackage(this, 
					mResources_photos_XKPName, mResources_photos_DroidName, 
					mResources_photos_Type, mResources_photos_Id
					);
				util.addXKPPackage(photos);
				photos.setOnResourcePackageListener(new OnResourcePackageListener() {
					@Override
					public void onResourcePackageLoaded(XKPPackage pack) {
						onResourcePackageLoadedphotos();
					}
				});
				maps = new XKPPackage(this, 
					mResources_maps_XKPName, mResources_maps_DroidName, 
					mResources_maps_Type, mResources_maps_Id
					);
				util.addXKPPackage(maps);
				maps.setOnResourcePackageListener(new OnResourcePackageListener() {
					@Override
					public void onResourcePackageLoaded(XKPPackage pack) {
						onResourcePackageLoadedmaps();
					}
				});
			}
			private void onClickbtnGallery() {
				if (!photos.isResourceLoaded())
{
	lblStatus.setText("Loading resources...");
	photos.load();
}
swtScene.setDisplayedChildByName(util.getIdFromName("imgGallery"));
lblInformation.setText("Click pictures to iterate through theirs.");
			}
			private void onClickbtnMaps() {
				if (!maps.isResourceLoaded())
{
	lblStatus.setText("Loading resources...");
	maps.load();
}
swtScene.setDisplayedChildByName(util.getIdFromName("mapController"));
lblInformation.setText("Drag picture to see whole map.");
			}
			private void onClickimgGallery() {
				currentImage = currentImage + 1;
currentImage = currentImage % resources.size();
String currentResource = resources.get(currentImage);
util.setResourceImageView(imgGallery, currentResource);
			}
			private void onResourcePackageLoadedphotos() {
				lblStatus.setText("Photos was loaded completely...");
imgGallery.performClick();
			}
			private void onResourcePackageLoadedmaps() {
				lblStatus.setText("Maps was loaded completely...");
util.setResourceXKPImageMap(mapController, "holguin");
			}
			private Integer currentImage = -1;
			private ArrayList<String> resources = new ArrayList(Arrays.asList( new Object [] {"worth_1", "worth_2", "worth_3", "worth_4", "worth_5", "worth_6", "worth_7"} ));
}
