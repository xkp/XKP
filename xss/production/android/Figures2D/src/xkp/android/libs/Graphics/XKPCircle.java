package xkp.android.libs.Graphics;
import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Path.Direction;
import android.graphics.drawable.ShapeDrawable;
import android.graphics.drawable.shapes.PathShape;
import android.util.AttributeSet;
import xkp.android.Figures2D.R;
public class XKPCircle extends XKPGraphics {
	public XKPCircle(Context context) {
		this(context, null);
	}
	public XKPCircle(Context context, AttributeSet attrs) {
		super(context, attrs);
		TypedArray ta = context.obtainStyledAttributes(
				attrs, R.styleable.XKPCircle);
		mRadius = ta.getDimensionPixelOffset(R.styleable.XKPCircle_radius, 1);
		ta.recycle();
		preCalcPosition(mX1, mY1, mX1 + 2 * mRadius, mY1 + 2 * mRadius);
		updateShapePosition();
	}
	@Override
	protected void updateShapePosition() {
		mPathShape.reset();
		mPathShape.addCircle(mX1 + mRadius, mY1 + mRadius, mRadius, Direction.CCW);
		mPathShape.computeBounds(mBounds, true);
		mPathShape.transform(mMtxAngle);
		mDrawable = new ShapeDrawable(new PathShape(mPathShape, mBounds.width(), mBounds.height()));
		mDrawable.setBounds(0, 0, (int) mBounds.width(), (int) mBounds.height());
	}
}
