<xss:parameter id="appName"/>
<xss:parameter id="base_namespace"/>

package <xss:e value="base_namespace"/>.libs.Graphic;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.RectF;
import android.graphics.Path.Direction;
import android.graphics.drawable.ShapeDrawable;
import android.graphics.drawable.shapes.PathShape;
import android.util.AttributeSet;

import <xss:e value="base_namespace"/>.<xss:e value="appName"/>.R;
public class XKPCircle extends XKPGraphics {
	
	private Integer mRadius = 1;
	
	public XKPCircle(Context context) {
		this(context, null);
	}

	public XKPCircle(Context context, AttributeSet attrs) {
		super(context, attrs);

		TypedArray ta = context.obtainStyledAttributes(
				attrs, R.styleable.XKPCircle);
		
		mRadius = ta.getDimensionPixelOffset(R.styleable.XKPCircle_radius, 1);
		
		ta.recycle();
		
		mPathShape.reset();
		mPathShape.addCircle(mX1, mY1, mRadius, Direction.CCW);
		
		RectF bounds = new RectF();
		mPathShape.computeBounds(bounds, true);
		
		mDrawable = new ShapeDrawable(new PathShape(mPathShape, bounds.width(), bounds.height()));
		mDrawable.setBounds(0, 0, (int) bounds.width(), (int) bounds.height());
	}

	public void setRadius(Integer radius) {
		this.mRadius = radius;
		
		invalidate();
	}
	
	public Integer getRadius() {
		return mRadius;
	}
}
