<xss:parameter id="appName"/>
<xss:parameter id="base_namespace"/>

package <xss:e value="base_namespace"/>.libs.Graphics;

import android.content.Context;
import android.graphics.Path.Direction;
import android.graphics.drawable.ShapeDrawable;
import android.graphics.drawable.shapes.PathShape;
import android.util.AttributeSet;

public class XKPPoint extends XKPGraphics {

	public XKPPoint(Context context) {
		this(context, null);
	}

	public XKPPoint(Context context, AttributeSet attrs) {
		super(context, attrs);

		updateShapePosition();
	}
	
	@Override
	protected void updateShapePosition() {
		Integer mRadius = new Integer(1);
		mPathShape.reset();
		mPathShape.addCircle(mX1, mY1, mRadius, Direction.CCW);
		mPathShape.transform(mMtxAngle);
		
		mPathShape.computeBounds(mBounds, true);

		mDrawable = new ShapeDrawable(new PathShape(mPathShape, mBounds.width(), mBounds.height()));
		mDrawable.setBounds(0, 0, (int) mBounds.width(), (int) mBounds.height());
	}
}
