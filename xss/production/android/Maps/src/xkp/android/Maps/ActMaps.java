package xkp.android.Maps;
import java.util.ArrayList;
import java.util.Arrays;
import android.view.View;
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
						import android.widget.ImageButton;
						import android.widget.ImageView;
						import xkp.android.libs.Widget.XKPUtils;
						import android.widget.TextView;
						import android.text.method.KeyListener;
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
				private XKPSwitcher swtScene;
				private TextView lblInformation;
				private ImageView imgGallery;
				private XKPImageMap mapController;
			public static XKPUtils util;
				private XKPPackage __resources1;
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
			private String [] mResources___resources1_XKPName = {
				"ico_gallery",
				"ico_maps"
			};
			private String [] mResources___resources1_DroidName = {
				"launcher_gallery.png",
				"launcher_maps.png"
			};
			private Integer [] mResources___resources1_Type = {
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE
			};
			private Integer [] mResources___resources1_Id = {
				R.drawable.launcher_gallery,
				R.drawable.launcher_maps
			};
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
				swtScene = (XKPSwitcher) findViewById(R.id.swtScene);
				lblInformation = (TextView) findViewById(R.id.lblInformation);
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
			private void initInstances() {
				application = this;
				__resources1 = new XKPPackage(this, 
					mResources___resources1_XKPName, mResources___resources1_DroidName, 
					mResources___resources1_Type, mResources___resources1_Id
					, true);
				util.addXKPPackage(__resources1);
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
			}
			private void onClickbtnMaps() {
				if (!maps.isResourceLoaded())
{
	lblStatus.setText("Loading resources...");
	maps.load();
}
swtScene.setDisplayedChildByName(util.getIdFromName("mapController"));
			}
			private void onClickimgGallery() {
				prop_application_currentImage = prop_application_currentImage + 1;
prop_application_currentImage = prop_application_currentImage % prop_application_resources.size();
String currentResource = prop_application_resources.get(prop_application_currentImage);
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
			private Integer prop_application_currentImage = -1;
			private ArrayList<String> prop_application_resources = new ArrayList(Arrays.asList( new Object [] {"worth_1", "worth_2", "worth_3", "worth_4", "worth_5", "worth_6", "worth_7"} ));
}
