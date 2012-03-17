package xkp.android.Gallery;
import java.util.ArrayList;
import java.util.Arrays;
import android.view.View;
			import android.app.Activity;
			import android.os.Bundle;
import xkp.android.libs.Layout.XKPLayout;
						import android.widget.Gallery;
						import android.graphics.Bitmap;
						import xkp.android.libs.Widget.XKPPackage;
						import android.view.View.OnClickListener;
						import android.view.View.OnLongClickListener;
						import android.view.MotionEvent;
						import android.view.View.OnTouchListener;
						import android.view.View.OnKeyListener;
						import android.view.View.OnFocusChangeListener;
						import android.view.ViewGroup;
						import android.widget.ImageView;
						import android.widget.BaseAdapter;
						import android.content.Context;
						import xkp.android.libs.Widget.XKPPackage;
		public class ActGallery 
						extends Activity
{
				private Gallery gallery;
					XKPUtils util = new XKPUtils(this);
				private XKPPackage __resources1;
			private String [] mResources___resources1_XKPName = {
				"img01",
				"img02",
				"img03",
				"img04",
				"img05",
				"img06",
				"img07",
				"img08"
			};
			private String [] mResources___resources1_DroidName = {
				"gallery_photo_1.jpg",
				"gallery_photo_2.jpg",
				"gallery_photo_3.jpg",
				"gallery_photo_4.jpg",
				"gallery_photo_5.jpg",
				"gallery_photo_6.jpg",
				"gallery_photo_7.jpg",
				"gallery_photo_8.jpg"
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
				R.drawable.gallery_photo_1,
				R.drawable.gallery_photo_2,
				R.drawable.gallery_photo_3,
				R.drawable.gallery_photo_4,
				R.drawable.gallery_photo_5,
				R.drawable.gallery_photo_6,
				R.drawable.gallery_photo_7,
				R.drawable.gallery_photo_8
			};
	ActGallery application;
			@Override
			public void onCreate(Bundle savedInstanceState) {
				super.onCreate(savedInstanceState);
				setContentView(R.layout.main);
				bindViews();
				initInstances();
                initCallers();
			}
			private void bindViews() {
				gallery = (Gallery) findViewById(R.id.gallery);
					gallery.setAdapter(new ImageAdapter(this));
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
	public class ImageAdapter extends BaseAdapter {
		private static final int ITEM_WIDTH = 236;
		private static final int ITEM_HEIGHT = 188;
		private final Context mContext;
		private final float mDensity;
		private final Integer[] mImageIds = {
				R.drawable.gallery_photo_1,
				R.drawable.gallery_photo_2,
				R.drawable.gallery_photo_3,
				R.drawable.gallery_photo_4,
				R.drawable.gallery_photo_5,
				R.drawable.gallery_photo_6,
				R.drawable.gallery_photo_7,
				R.drawable.gallery_photo_8
					};
		public ImageAdapter(Context ctx) {
			mContext = ctx;
			mDensity = ctx.getResources().getDisplayMetrics().density;
		}
		@Override
		public int getCount() {
			return mImageIds.length;
		}
		@Override
		public Object getItem(int position) {
			return position;
		}
		@Override
		public long getItemId(int position) {
			return position;
		}
		@Override
		public View getView(int position, View convertView, ViewGroup parent) {
			ImageView imageView;
			if (convertView == null) {
				convertView = new ImageView(mContext);
				imageView = (ImageView) convertView;
				imageView.setScaleType(ImageView.ScaleType.FIT_XY);
				imageView.setLayoutParams(new Gallery.LayoutParams(
						(int) (ITEM_WIDTH * mDensity + 0.5f),
						(int) (ITEM_HEIGHT * mDensity + 0.5f)));
			} else {
				imageView = (ImageView) convertView;
			}
			imageView.setImageResource(mImageIds[position]);
			return imageView;
		}
	}
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
