package xkp.android.ImagesChanger;
import java.util.ArrayList;
import java.util.Arrays;
import android.view.View;
			import android.app.Activity;
			import android.os.Bundle;
import xkp.android.libs.Layout.XKPLayout;
						import android.graphics.Bitmap;
						import android.widget.ImageView;
						import xkp.android.libs.Widget.XKPPackage;
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
		public class ActImagesChanger 
						extends Activity
							implements OnClickListener
{
				private ImageView album;
					XKPUtils util = new XKPUtils(this);
				private Button changer;
				private TextView identifier;
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
	ActImagesChanger application;
			@Override
			public void onCreate(Bundle savedInstanceState) {
				super.onCreate(savedInstanceState);
				setContentView(R.layout.main);
				bindViews();
				initInstances();
                initCallers();
			}
			private void bindViews() {
				album = (ImageView) findViewById(R.id.album);
				changer = (Button) findViewById(R.id.changer);
					changer.setOnClickListener(this);
				identifier = (TextView) findViewById(R.id.identifier);
	}
void initCallers() {
}
			void initInstances() {
				application = this;
				__resources1 = new XKPPackage(this, 
					mResources___resources1_XKPName, mResources___resources1_DroidName, 
					mResources___resources1_Type, mResources___resources1_Id, true);
				util.addXKPPackage(__resources1);
					__resources1.load();
			}
						public void onClick(View v) {
							switch (v.getId()) {
									case R.id.changer:
										onClickchanger();
										break;
						} //switch
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
					public class XKPUtils {
						private Activity mActivity;
						private ArrayList<XKPPackage> mPackages;
						private XKPUtils(Activity act) {
							mActivity = act;
							mPackages = new ArrayList<XKPPackage>();
						}
						public void setResourceImageView(int id, String resource) {
			ImageView img = (ImageView) mActivity.findViewById(id);
			img.setImageBitmap(getImageFromResource(resource));
		}
		public void addXKPPackage(XKPPackage pack) {			mPackages.add(pack);		}				private int getResourceIdentifierFromPackages(String resource, XKPPackage [] outWrapperPack) {			for(int i = 0; i < mPackages.size(); i++) {				XKPPackage pack = mPackages.get(i);				int resId = pack.getResourceIdentifier(resource);				if(resId != -1) {					outWrapperPack[0] = pack;					return resId;				}			}			outWrapperPack[0] = null;			return -1;		}				public Bitmap getImageFromResource(String resource) {			XKPPackage pack = null;			XKPPackage [] wrapperPack = new XKPPackage [] { pack };  						int resId = getResourceIdentifierFromPackages(resource, wrapperPack);			pack = wrapperPack[0];						if(pack != null) {				return pack.getBitmapResource(resId);			}						return null;		}
			}
}
