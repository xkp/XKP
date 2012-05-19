package xkp.android.Gallery;
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
						import android.widget.Gallery;
						import xkp.android.libs.Widget.XKPUtils;
						import android.view.ViewGroup;
						import android.widget.ImageView;
						import android.widget.BaseAdapter;
						import android.content.Context;
						import xkp.android.libs.Widget.XKPPackage;
						import xkp.android.libs.Widget.XKPPackage.OnResourcePackageListener;
		public class ActGallery 
						extends Activity
{
				private Gallery gallery;
			public static XKPUtils util;
		private XKPLayout layoutapplication;
				private XKPPackage ;
			private String [] mResources__XKPName = {
				"img01",
				"img02",
				"img03",
				"img04",
				"img05",
				"img06",
				"img07",
				"img08"
			};
			private String [] mResources__DroidName = {
				"gallery_photo_1.jpg",
				"gallery_photo_2.jpg",
				"gallery_photo_3.jpg",
				"gallery_photo_4.jpg",
				"gallery_photo_5.jpg",
				"gallery_photo_6.jpg",
				"gallery_photo_7.jpg",
				"gallery_photo_8.jpg"
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
				R.drawable.gallery_photo_1,
				R.drawable.gallery_photo_2,
				R.drawable.gallery_photo_3,
				R.drawable.gallery_photo_4,
				R.drawable.gallery_photo_5,
				R.drawable.gallery_photo_6,
				R.drawable.gallery_photo_7,
				R.drawable.gallery_photo_8
			};
	private boolean mLayoutStarted = false;
			private ActGallery application;
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
				gallery = (Gallery) findViewById(R.id.gallery);
					gallery.setAdapter(new ImageAdapter(this));
	}
private void initCallers() {
			util = new XKPUtils();
			ActGallery.util.addView(this);
			util = ActGallery.util;
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
}
