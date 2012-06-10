package xkp.android.libs.Graphics;
import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.PointF;
import android.graphics.RectF;
import android.graphics.Region;
import android.graphics.drawable.ShapeDrawable;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;
import xkp.android.jBox2d_1.R;
import xkp.android.libs.Layout.XKPLayout;
public class XKPGraphics extends View {
	protected int 			mLineWidth 			= 0;
	protected int 			mStrokeColor 		= 0xffffffff;
	protected int 			mFillColor 			= 0xffffffff;
	protected int   		mBackgroundColor	= 0xffffffff;
	protected Paint.Cap 	mCap 				= Paint.Cap.BUTT;
	protected Paint.Style 	mStyle 				= Paint.Style.FILL_AND_STROKE;
	protected float   		mX1					= -1;
	protected float   		mY1					= -1;
	protected float   		mX2					= -1;
	protected float   		mY2					= -1;
	protected float 		mDX					= 0;
	protected float			mDY					= 0;
	protected float			mCenterX			= 0;
	protected float			mCenterY			= 0;
	protected float 		mRadius 			= 1;
	protected RectF			mBounds				= new RectF();
	protected float			mAngle				= 0;
	protected Matrix		mMtxAngle			= new Matrix();
	protected PointF		mLeftTop			= new PointF();
	protected PointF		mBottomRight		= new PointF();
	protected RectF			mRectF				= new RectF();
	protected Path			mPathShape			= new Path();
	protected Path			mPathClipRef;
	protected ShapeDrawable	mDrawable;
	protected Bitmap		mBitmap;
	protected Paint mPaintFill 		= new Paint();
	protected Paint mPaintStroke 	= new Paint();
	protected OnClickInsideFigureListener mOnClickInsideFigureListener;
	public XKPGraphics(Context context) {
		this(context, null);
	}
	public XKPGraphics(Context context, AttributeSet attrs) {
		super(context, attrs);
		TypedArray ta = context.obtainStyledAttributes(
				attrs, R.styleable.XKPGraphics);
		mLineWidth = ta.getDimensionPixelOffset(R.styleable.XKPGraphics_line_width, 1);
		mStrokeColor = ta.getColor(R.styleable.XKPGraphics_stroke_color, 0xffffffff);
		mFillColor = ta.getColor(R.styleable.XKPGraphics_fill_color, 0xffffffff);
		String strCap = ta.getString(R.styleable.XKPGraphics_paint_cap);
		mX1 = ta.getDimensionPixelOffset(R.styleable.XKPGraphics_pos_x1, -1);
		mY1 = ta.getDimensionPixelOffset(R.styleable.XKPGraphics_pos_y1, -1);
		mX2 = ta.getDimensionPixelOffset(R.styleable.XKPGraphics_pos_x2, -1);
		mY2 = ta.getDimensionPixelOffset(R.styleable.XKPGraphics_pos_y2, -1);
		mDX = ta.getDimensionPixelOffset(R.styleable.XKPGraphics_pos_width, -1);
		mDY = ta.getDimensionPixelOffset(R.styleable.XKPGraphics_pos_height, -1);
		float angle = ta.getFloat(R.styleable.XKPGraphics_angle, 0);
		if(mDX != -1 && mDY != -1) {
			mX2 = mX1 + mDX;
			mY2 = mY1 + mDY;
		}
		// no implement functionality for now
		//mBackgroundColor = ta.getColor(R.styleable.XKPGraphics_background_color, 0xffffffff);
		//String strStyle = ta.getString(R.styleable.XKPGraphics_paint_style);
		//setStringStyle(strStyle);
		ta.recycle();
		mPaintStroke.setStyle(Paint.Style.STROKE);
		mPaintStroke.setColor(mStrokeColor);
		mPaintStroke.setAntiAlias(true);
		mPaintStroke.setStrokeWidth(mLineWidth);
		setStringCap(strCap);
		mPaintFill.setStyle(Paint.Style.FILL);
		mPaintFill.setColor(mFillColor);
		mPaintFill.setAntiAlias(true);
		mPaintFill.setStrokeWidth(mLineWidth);
		setAngle(angle);
		setPosition(mX1, mY1, mX2, mY2);
		setOnTouchListener(new OnTouchListener() {
			public boolean onTouch(final View v, final MotionEvent event) {
				if(!isEnabled())
					return false;
				//if(event.getAction() == MotionEvent.ACTION_UP) {
				if(event.getAction() == MotionEvent.ACTION_DOWN) {
					// Ohh... yea... yes, this is very difficult method to know 
					// if a specific point is inside a figure,... 
					// Momentarily I implement this code described here: 
					// http://stackoverflow.com/questions/2597590/how-can-i-tell-if-a-closed-path-contains-a-given-point
					RectF rectF = new RectF();
					mPathShape.computeBounds(rectF, true);
					Region figRegion = new Region();
					figRegion.setPath(mPathShape, new Region((int)rectF.left, (int)rectF.top, (int)rectF.right, (int)rectF.bottom));
					mPathClipRef.computeBounds(rectF, true);
					Region clipRegion = new Region();
					clipRegion.setPath(mPathClipRef, new Region((int)rectF.left, (int)rectF.top, (int)rectF.right, (int)rectF.bottom));
					if(!clipRegion.isEmpty())
						figRegion.op(clipRegion, Region.Op.INTERSECT);
					if(figRegion.contains((int)event.getX(), (int)event.getY())) {
						return performClickInsideFigure();
					}
					//else
					//	return false;
				}
				//return true;
				return false;
			}
		});
	}
	@Override
	protected void onDraw(Canvas canvas) {
		if(!getIsDrawable() || getVisibility() != VISIBLE)
			return;
		//canvas.save();
		View parent = (View)getParent();
		if(parent instanceof XKPLayout) {
			mPathClipRef = ((XKPLayout)parent).getPathClip();
			if(mPathClipRef != null && !mPathClipRef.isEmpty())
				canvas.clipPath(mPathClipRef, Region.Op.REPLACE);
		}
		if(mBitmap != null) {
			if(mX2 <= 0 || mY2 <= 0) return;
			Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);
			paint.setAntiAlias(true);
			// http://stackoverflow.com/questions/5287483/image-changes-size-as-its-rotated-how-do-i-stop-this
	        // precompute some trig functions
	        float radians = (float) Math.toRadians(mAngle);
	        float sin = (float) Math.abs(Math.sin(radians));
	        float cos = (float) Math.abs(Math.cos(radians));
	        // figure out total width and height of new bitmap
	        float newWidth = mDX * cos + mDY * sin;
	        float newHeight = mDX * sin + mDY * cos;
	        float middleWidth = newWidth / 2;
	        float middleHeight = newHeight / 2;
	        RectF newRectF = new RectF(mCenterX - middleWidth, mCenterY - middleHeight, 
	        					mCenterX + middleWidth, mCenterY + middleHeight);
			Bitmap renderBmp = Bitmap.createBitmap(mBitmap, 0, 0, mBitmap.getWidth(), mBitmap.getHeight(), mMtxAngle, false);
			canvas.drawBitmap(renderBmp, null, newRectF, paint);
		}
		if(mDrawable != null) {
			mDrawable.getPaint().set(mPaintFill);
			mDrawable.draw(canvas);
			mDrawable.getPaint().set(mPaintStroke);
			mDrawable.draw(canvas);
		}
		//canvas.restore();
	}
//	private void invalidateRect() {
//		//TODO: calculate the max coordinate when figure is rotated 45 degree
//		//		then, replace all invalidate() methods for invalidateRect()
//		invalidate(mX1, mY1, mX2, mY2);
//	}
	protected Boolean getIsDrawable() {
		return true;
	}
	protected void preCalcAngle(float angle) {
		mAngle = angle % 360;
		mMtxAngle.reset();
		mMtxAngle.setRotate(mAngle, mCenterX, mCenterY);
	}
	public void setAngle(float angle) {
		preCalcAngle(angle);
		updateShapePosition();
		invalidate();
	}
	public float getAngle() {
		return mAngle;
	}
	public void setLineWidth(int width) {
		mLineWidth = width;
		mPaintStroke.setStrokeWidth(mLineWidth);
		mPaintFill.setStrokeWidth(mLineWidth);
		invalidate();
	}
	public int getLineWidth() {
		return mLineWidth;
	}
	protected int parseStringColor(String color) {
		int resId = getResources().getIdentifier(color, "color", getContext().getPackageName());
		int iColor = getResources().getInteger(resId);
		return iColor;
	}
	public void setStrokeColor(int color) {
		mStrokeColor = color;
		mPaintStroke.setColor(mStrokeColor);
		invalidate();
	}
	public void setStrokeColor(String color) {
		int iColor = parseStringColor(color);
		setStrokeColor(iColor);
	}
	public int getStrokeColor() {
		return mStrokeColor;
	}
	public void setFillColor(int color) {
		mFillColor = color;
		mPaintFill.setColor(mFillColor);
		invalidate();
	}
	public int getFillColor() {
		return this.mFillColor;
	}
	protected Paint.Cap parseStringCap(String strCap) {
		Paint.Cap cap = Paint.Cap.BUTT;
		if(strCap == null || strCap.length() == 0)
			return cap;
		if(strCap.compareToIgnoreCase("butt") == 0)
			cap = Paint.Cap.BUTT;
		else
		if(strCap.compareToIgnoreCase("round") == 0)
			cap = Paint.Cap.ROUND;
		else
		if(strCap.compareToIgnoreCase("square") == 0)
			cap = Paint.Cap.SQUARE;
		return cap;
	}
	public void setStringCap(String strCap) {
		mCap = parseStringCap(strCap);
		mPaintStroke.setStrokeCap(mCap);
		mPaintFill.setStrokeCap(mCap);
		invalidate();
	}
	public String getStringCap() {
		return "TODO";
	}
	protected Paint.Style parseStringStyle(String strStyle) {
		Paint.Style style = Paint.Style.FILL_AND_STROKE;
		if(strStyle == null || strStyle.length() == 0)
			return style;
		if(strStyle.compareToIgnoreCase("fill") == 0)
			style = Paint.Style.FILL;
		else
		if(strStyle.compareToIgnoreCase("fill_and_stroke") == 0)
			style = Paint.Style.FILL_AND_STROKE;
		else
		if(strStyle.compareToIgnoreCase("stroke") == 0)
			style = Paint.Style.STROKE;
		return style;
	}
	public void setStringStyle(String strStyle) {
		mStyle = parseStringStyle(strStyle);
		mPaintFill.setStyle(mStyle);
		invalidate();
	}
	public String getStringStyle() {
		return "TODO";
	}
	protected void updateShapePosition() {
	}
	public void setRadius(float radius) {
		mRadius = radius;
		mX2 = mRadius * 2;
		mY2 = mRadius * 2;
		invalidate();
	}
	public float getRadius() {
		return mRadius;
	}
	protected void preCalcPosition(float x1, float y1, float x2, float y2) {
		mX1 = x1;
		mY1 = y1;
		mX2 = x2;
		mY2 = y2;
		mLeftTop.set(Math.min(mX1, mX2), Math.min(mY1, mY2));
		mBottomRight.set(Math.max(mX1, mX2), Math.max(mY1, mY2));
		mRectF.set(mX1, mY1, mX2, mY2);
		mDX = Math.abs(mX2 - mX1);
		mDY = Math.abs(mY2 - mY1);
        mCenterX = mLeftTop.x + mDX / 2;
		mCenterY = mLeftTop.y + mDY / 2;
		// TODO: compare mDelthas X and Y with an epsilon value and not with zero 
		if(mDX == 0) mDX = 1;
		if(mDY == 0) mDY = 1;
		mRadius = Math.max(mDX, mDY) / 2;
	}
	public void setPosition(float x1, float y1, float x2, float y2) {
		preCalcPosition(x1, y1, x2, y2);
		preCalcAngle(mAngle);
		updateShapePosition();
		invalidate();
	}
	public void setPosition(float x1, float y1, boolean anchorRightBottom) {
		float x2 = mX2;
		float y2 = mY2;
		if(anchorRightBottom == false && x2 != -1 && y2 != -1) {
			x2 = x1 + mDX;
			y2 = y1 + mDY;
		}
		setPosition(x1, y1, x2, y2);
	}
	public void setPosition(float x1, float y1) {
		setPosition(x1, y1, false);
	}
	public void setFigureSize(float width, float height) {
		setPosition(mX1, mY1, mX1 + width, mY1 + height);
	}
	public void setFigureWidth(float width) {
		setPosition(mX1, mY1, mX1 + width, mY2);
	}
	public void setFigureHeight(float height) {
		setPosition(mX1, mY1, mX2, mY1 + height);
	}
	public void setLeft(float x) {
		float delthaX = mX2 - mX1;
		setPosition(x, mY1, x + delthaX, mY2);
	}
	public void setTop(float y) {
		float delthaY = mY2 - mY1;
		setPosition(mX1, y, mX2, y + delthaY);
	}
	public void setWidth(float width) {
		setPosition(mX1, mY1, mX1 + width, mY2);
	}
	public void setHeight(float height) {
		setPosition(mX1, mY1, mX2, mY1 + height);
	}
	public float getDX() {
		return mDX;
	}
	public float getDY() {
		return mDY;
	}
	public void setX1(float x1) {
		setPosition(x1, mY1, mX2, mY2);
	}
	public void setX2(float x2) {
		setPosition(mX1, mY1, x2, mY2);
	}
	public void setY1(float y1) {
		setPosition(mX1, y1, mX2, mY2);
	}
	public void setY2(float y2) {
		setPosition(mX1, mY1, mX2, y2);
	}
	public float getX1() {
		return mX1;
	}
	public float getX2() {
		return mX2;
	}
	public float getY1() {
		return mY1;
	}
	public float getY2() {
		return mY2;
	}
	public float getFigureWidth() {
		return mDX;
	}
	public float getFigureHeight() {
		return mDY;
	}
	public void setOnClickInsideFigureListener(OnClickInsideFigureListener l) {
		mOnClickInsideFigureListener = l;
	}
	public boolean performClickInsideFigure() {
		if (mOnClickInsideFigureListener != null) {
			mOnClickInsideFigureListener.onClickInsideFigure(this);
			return true;
		}
		return false;
	}
	public interface OnClickInsideFigureListener {
		void onClickInsideFigure(View v);
	}
}
