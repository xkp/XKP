package xkp.android.libs.Graphics;
import android.content.Context;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Path.Direction;
import android.util.AttributeSet;
import xkp.android.jBox2d_2.R;
public class XKPImage extends XKPGraphics {
	private Context 	mContext;
	private int 		mBitmapResourceId;
	public XKPImage(Context context) {
		this(context, null);
	}
	public XKPImage(Context context, AttributeSet attrs) {
		super(context, attrs);
		mContext = context;
		TypedArray ta = context.obtainStyledAttributes(
				attrs, R.styleable.XKPImage);
		mBitmapResourceId = ta.getResourceId(R.styleable.XKPImage_src, 0);
		ta.recycle();
		resolveBitmapFromResource(mBitmapResourceId);
		calculateSize();
	}
	public void setImageBitmap(Bitmap bmp) {
		mBitmap = bmp;
		calculateSize();
		mBitmap.prepareToDraw();
		invalidate();
	}
	public Bitmap getImageBitmap() {
		return mBitmap;
	}
	public void setAlpha() {
		//TODO: 
	}
	public void setImageResource(int resId) {
		if(resId == 0) return;
		Resources res = mContext.getResources();
		setImageBitmap(BitmapFactory.decodeResource(res, resId));
	}
	private void resolveBitmapFromResource(int resId) {
		Resources res = mContext.getResources();
		mBitmap = BitmapFactory.decodeResource(res, resId);
	}
	@Override
	protected void updateShapePosition() {
		mPathShape.reset();
		mPathShape.addRect(mLeftTop.x, mLeftTop.y, mBottomRight.x, mBottomRight.y, Direction.CCW);
		mPathShape.transform(mMtxRotation);
	}
	private void calculateSize() {
		if(mBitmap == null) return;
		setFigureSize(mBitmap.getWidth(), mBitmap.getHeight());
	}
}
