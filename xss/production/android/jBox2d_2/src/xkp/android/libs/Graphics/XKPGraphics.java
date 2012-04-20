package xkp.android.libs.Graphics;
import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.Point;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.Region;
import android.graphics.drawable.ShapeDrawable;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;
import xkp.android.jBox2d_2.R;
import xkp.android.libs.Layout.XKPLayout;
public class XKPGraphics extends View {
	private final Integer	DEFAULT_SIZE		= 10;
	protected Integer 		mLineWidth 			= 0;
	protected Integer 		mStrokeColor 		= 0xffffffff;
	protected Integer 		mFillColor 			= 0xffffffff;
	protected Integer   	mBackgroundColor	= 0xffffffff;
	protected Paint.Cap 	mCap 				= Paint.Cap.BUTT;
	protected Paint.Style 	mStyle 				= Paint.Style.FILL_AND_STROKE;
	protected Integer   	mX1					= -1;
	protected Integer   	mY1					= -1;
	protected Integer   	mX2					= -1;
	protected Integer   	mY2					= -1;
	protected Integer 		mDX					= 0;
	protected Integer		mDY					= 0;
	protected Integer 		mRadius 			= 1;
	protected RectF			mBounds				= new RectF();
	protected double		mRotation			= 0;
	protected Matrix		mMtxRotation		= new Matrix();
	protected Point			mLeftTop			= new Point();
	protected Point			mBottomRight		= new Point();
	protected Rect			mRect				= new Rect();
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
		double rotation = ta.getFloat(R.styleable.XKPGraphics_rotation, 0);
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
		setRotation(rotation);
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
		canvas.save();
		View parent = (View)getParent();
		if(parent instanceof XKPLayout) {
			mPathClipRef = ((XKPLayout)parent).getPathClip();
			if(mPathClipRef != null && !mPathClipRef.isEmpty())
				canvas.clipPath(mPathClipRef, Region.Op.REPLACE);
		}
		if(mBitmap != null) {
			canvas.drawBitmap(mBitmap, null, new RectF(mX1, mY1, mX2, mY2), null);
		}
		if(mDrawable != null) {
			mDrawable.getPaint().set(mPaintFill);
			mDrawable.draw(canvas);
			mDrawable.getPaint().set(mPaintStroke);
			mDrawable.draw(canvas);
		}
		canvas.restore();
	}
	protected Boolean getIsDrawable() {
		return true;
	}
	public void setRotation(double rotation) {
		mRotation = rotation;
		int centerX = mLeftTop.x + (int) (mDX / 2);
		int centerY = mLeftTop.y + (int) (mDY / 2);
		mMtxRotation.reset();
		mMtxRotation.setRotate((float)mRotation, centerX, centerY);
		updateShapePosition();
		invalidate();
	}
	public double getRotation() {
		return mRotation;
	}
	public void setLineWidth(Integer width) {
		mLineWidth = width;
		mPaintStroke.setStrokeWidth(mLineWidth);
		mPaintFill.setStrokeWidth(mLineWidth);
		invalidate();
	}
	public Integer getLineWidth() {
		return mLineWidth;
	}
	protected Integer parseStringColor(String color) {
		Integer resId = getResources().getIdentifier(color, "color", getContext().getPackageName());
		Integer iColor = getResources().getInteger(resId);
		return iColor;
	}
	public void setStrokeColor(Integer color) {
		mStrokeColor = color;
		mPaintStroke.setColor(mStrokeColor);
		invalidate();
	}
	public void setStrokeColor(String color) {
		Integer iColor = parseStringColor(color);
		setStrokeColor(iColor);
	}
	public Integer getStrokeColor() {
		return mStrokeColor;
	}
	public void setFillColor(Integer color) {
		mFillColor = color;
		mPaintFill.setColor(mFillColor);
		invalidate();
	}
	public Integer getFillColor() {
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
	public void setRadius(Integer radius) {
		mRadius = radius;
		mX2 = mRadius * 2;
		mY2 = mRadius * 2;
		invalidate();
	}
	public Integer getRadius() {
		return mRadius;
	}
	public void setPosition(Integer x1, Integer y1, Integer x2, Integer y2) {
		mX1 = x1;
		mY1 = y1;
		if(mX2 == -1 && x2 == -1) mX2 = mX1 + DEFAULT_SIZE; else mX2 = x2;
		if(mY2 == -1 && y2 == -1) mY2 = mY1 + DEFAULT_SIZE; else mY2 = y2;
		mLeftTop.set(Math.min(mX1, mX2), Math.min(mY1, mY2));
		mBottomRight.set(Math.max(mX1, mX2), Math.max(mY1, mY2));
		mRectF.set(mX1, mY1, mX2, mY2);
		mDX = Math.abs(mX2 - mX1);
		mDY = Math.abs(mY2 - mY1);
		if(mDX == 0) mDX = 1;
		if(mDY == 0) mDY = 1;
		mRadius = Math.max(mDX, mDY) / 2;
		setRotation(mRotation);
		updateShapePosition();
		invalidate();
	}
	public void setPosition(Integer x1, Integer y1) {
		setPosition(x1, y1, getX2(), getY2());
	}
	public void setFigureSize(Integer width, Integer height) {
		setPosition(mX1, mY1, mX1 + width, mY1 + height);
	}
	public void setFigureWidth(Integer width) {
		setPosition(mX1, mY1, mX1 + width, mY2);
	}
	public void setFigureHeight(Integer height) {
		setPosition(mX1, mY1, mX2, mY1 + height);
	}
	public void setLeft(Integer x) {
		int delthaX = mX2 - mX1;
		setPosition(x, mY1, x + delthaX, mY2);
	}
	public void setTop(Integer y) {
		int delthaY = mY2 - mY1;
		setPosition(mX1, y, mX2, y + delthaY);
	}
	public void setWidth(Integer width) {
		setPosition(mX1, mY1, mX1 + width, mY2);
	}
	public void setHeight(Integer height) {
		setPosition(mX1, mY1, mX2, mY1 + height);
	}
	public Integer getDX() {
		return mDX;
	}
	public Integer getDY() {
		return mDY;
	}
	public void setX1(Integer x1) {
		setPosition(x1, mY1, mX2, mY2);
	}
	public void setX2(Integer x2) {
		setPosition(mX1, mY1, x2, mY2);
	}
	public void setY1(Integer y1) {
		setPosition(mX1, y1, mX2, mY2);
	}
	public void setY2(Integer y2) {
		setPosition(mX1, mY1, mX2, y2);
	}
	public Integer getX1() {
		return mX1;
	}
	public Integer getX2() {
		return mX2;
	}
	public Integer getY1() {
		return mY1;
	}
	public Integer getY2() {
		return mY2;
	}
	public Integer getFigureWidth() {
		return mDX;
	}
	public Integer getFigureHeight() {
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
