<xss:parameter id="base_namespace"/>

package <xss:e value="base_namespace"/>.libs.Graphics;

import android.content.Context;
import android.graphics.Path.Direction;
import android.graphics.drawable.ShapeDrawable;
import android.graphics.drawable.shapes.PathShape;
import android.util.AttributeSet;

public class XKPRectangle extends XKPGraphics {
	
	public XKPRectangle(Context context) {
		this(context, null);
	}
	
	public XKPRectangle(Context context, AttributeSet attrs) {
		super(context, attrs);
		
		updateShapePosition();
	}
	
	@Override
	protected void updateShapePosition() {
		mPathShape.reset();
		mPathShape.addRect(mLeftTop.x, mLeftTop.y, mBottomRight.x, mBottomRight.y, Direction.CCW);
		
		mPathShape.computeBounds(mBounds, true);
		mPathShape.transform(mMtxAngle);
		
		mDrawable = new ShapeDrawable(new PathShape(mPathShape, mBounds.width(), mBounds.height()));
		mDrawable.setBounds(0, 0, (int) mBounds.width(), (int) mBounds.height());
	}
}
