package com.xkp.android.Gallery;
import java.util.ArrayList;
import java.util.Arrays;
import android.app.Activity;
import android.os.Bundle;
import android.view.View;
						import android.widget.Gallery;
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
public class ActGallery 
				extends Activity
{
				private Gallery gallery;
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        bindViews();
    }
    private void bindViews() {
				gallery = (Gallery) findViewById(R.id.gallery);
					gallery.setAdapter(new ImageAdapter(this));
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
