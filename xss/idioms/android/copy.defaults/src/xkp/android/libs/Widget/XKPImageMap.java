<xss:parameter id="base_namespace"/>

package <xss:e value="base_namespace"/>.libs.Widget;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.widget.ImageView;

import <xss:e value="base_namespace"/>.<xss:e value="application.appName"/>.R;

public class XKPImageMap extends ImageView implements OnTouchListener {
	
	private Context mContext;
	private Bitmap mBitmap;
	private int    mBitmapResourceId;
	
	private int mCameraLeft = 0;
	private int mCameraTop = 0;
	private int mCameraWidth = -1;
	private int mCameraHeight = -1;
	
	private int mTouchX = 0;
	private int mTouchY = 0;
	
	public XKPImageMap(Context ctx) {
		this(ctx, null);
	}
	
	public XKPImageMap(Context ctx, AttributeSet attrs) {
		super(ctx, attrs);
		
		mContext = ctx;
		
		TypedArray ta = ctx.obtainStyledAttributes(
				attrs, R.styleable.XKPImageMap);
		
		mCameraLeft = ta.getDimensionPixelOffset(R.styleable.XKPImageMap_camera_left, 0);
		mCameraTop = ta.getDimensionPixelOffset(R.styleable.XKPImageMap_camera_top, 0);
		mCameraWidth = ta.getDimensionPixelOffset(R.styleable.XKPImageMap_camera_width, -1);
		mCameraHeight = ta.getDimensionPixelOffset(R.styleable.XKPImageMap_camera_height, -1);
		
		mBitmapResourceId = ta.getResourceId(R.styleable.XKPImageMap_map_src, 0);
		
		ta.recycle();
		
		setImageMap(mBitmapResourceId);
		
		setOnTouchListener(this);
	}
	
	public void setImageMap(Bitmap map) {
		mBitmap = map;
		drawScene();
	}
	
	public void setImageMap(int mapResourceId) {
		mBitmap = BitmapFactory.decodeResource(mContext.getResources(), mapResourceId);
		drawScene();
	}
	
	public Bitmap getImageMap() {
		return mBitmap;
	}
	
	public void setCameraWidth(int width) {
		mCameraWidth = width;
		drawScene();
	}
	
	public int getCameraWidth() {
		return mCameraWidth;
	}
	
	public void setCameraHeight(int height) {
		mCameraHeight = height;
		drawScene();
	}
	
	public int getCameraHeight() {
		return mCameraHeight;
	}
	
	public void setCameraLeft(int left) {
		mCameraLeft = left;
		drawScene();
	}
	
	public int getCameraLeft() {
		return mCameraLeft;
	}
	
	public void setCameraTop(int top) {
		mCameraTop = top;
		drawScene();
	}
	
	public int getCameraTop() {
		return mCameraTop;
	}
	
	protected void drawScene() {
		checkDimensions();
		if(mBitmap != null && mCameraWidth > 0 && mCameraHeight > 0)
			setImageBitmap(Bitmap.createBitmap(
					mBitmap, mCameraLeft, mCameraTop, mCameraWidth, mCameraHeight));
	}
	
	private void checkDimensions() {
		if(mCameraLeft < 0) mCameraLeft = 0;
		if(mCameraTop < 0) mCameraTop = 0;
		
		if(mBitmap != null) {
			if(mCameraLeft > mBitmap.getWidth() - mCameraWidth) 
				mCameraLeft = mBitmap.getWidth() - mCameraWidth;
			if(mCameraTop > mBitmap.getHeight() - mCameraHeight)
				mCameraTop = mBitmap.getHeight() - mCameraHeight;
			
			if(mCameraWidth >= mBitmap.getWidth())
				mCameraWidth = mBitmap.getWidth();
                        else if(mCameraWidth < 0) 
				mCameraWidth = getWidth();
			if(mCameraHeight >= mBitmap.getHeight())
				mCameraHeight = mBitmap.getHeight();
			else if(mCameraHeight < 0)
				mCameraHeight = getHeight();
		}
	}
	
	@Override
	public boolean onTouch(View v, MotionEvent event) {
		
		switch(event.getAction()) {
		case MotionEvent.ACTION_DOWN:
			mTouchX = (int)event.getX();
			mTouchY = (int)event.getY();
			return true;
		case MotionEvent.ACTION_MOVE:
			int dx = (int)event.getX() - mTouchX;
			int dy = (int)event.getY() - mTouchY;
			
			mCameraLeft -= dx;
			mCameraTop -= dy;

			checkDimensions();
			drawScene();
			
			mTouchX = (int)event.getX();
			mTouchY = (int)event.getY();
			break;
		}
		
		return false;
	}
}
