package xkp.android.ImagesChanger;
import java.util.ArrayList;
import java.util.Arrays;
import android.view.View;
			import android.app.Activity;
			import android.os.Bundle;
import xkp.android.libs.Layout.XKPLayout;
			import xkp.android.libs.Widget.XKPUtils;
						import android.widget.ImageView;
						import xkp.android.libs.Widget.XKPUtils;
						import android.view.View.OnClickListener;
						import android.view.View.OnLongClickListener;
						import android.view.MotionEvent;
						import android.view.View.OnTouchListener;
						import android.view.View.OnKeyListener;
						import android.view.View.OnFocusChangeListener;
						import android.widget.Button;
						import android.text.method.KeyListener;
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
				private XKPPackage __resources1;
			private String [] mResources___resources1_XKPName = {
				"img1",
				"img2",
				"img3",
				"img4",
				"img5",
				"img6",
				"img7",
				"img8"
			};
			private String [] mResources___resources1_DroidName = {
				"photo_1.jpg",
				"photo_2.jpg",
				"photo_3.jpg",
				"photo_4.jpg",
				"photo_5.jpg",
				"photo_6.jpg",
				"photo_7.jpg",
				"photo_8.jpg"
			};
			private Integer [] mResources___resources1_Type = {
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE,
				XKPPackage.BITMAP_RESOURCE
			};
			private Integer [] mResources___resources1_Id = {
				R.drawable.photo_1,
				R.drawable.photo_2,
				R.drawable.photo_3,
				R.drawable.photo_4,
				R.drawable.photo_5,
				R.drawable.photo_6,
				R.drawable.photo_7,
				R.drawable.photo_8
			};
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
			private void initInstances() {
				application = this;
				__resources1 = new XKPPackage(this, 
					mResources___resources1_XKPName, mResources___resources1_DroidName, 
					mResources___resources1_Type, mResources___resources1_Id
					, true);
				util.addXKPPackage(__resources1);
			}
			private void onClickchanger() {
				prop_application_currentImage = prop_application_currentImage + 1;
prop_application_currentImage = prop_application_currentImage % prop_application_resources.size();
String currentResource = prop_application_resources.get(prop_application_currentImage);
util.setResourceImageView(album, currentResource);
identifier.setText(currentResource);
			}
			private Integer prop_application_currentImage = 0;
			private ArrayList<String> prop_application_resources = new ArrayList(Arrays.asList( new Object [] {"img1", "img2", "img3", "img4", "img5", "img6", "img7", "img8"} ));
}
