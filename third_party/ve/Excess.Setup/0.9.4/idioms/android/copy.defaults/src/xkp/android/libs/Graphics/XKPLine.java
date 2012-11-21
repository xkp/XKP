<xss:parameter id="base_namespace"/>

package <xss:e value="base_namespace"/>.libs.Graphics;

import android.content.Context;
import android.graphics.drawable.ShapeDrawable;
import android.graphics.drawable.shapes.PathShape;
import android.util.AttributeSet;

public class XKPLine extends XKPGraphics {
	
	public XKPLine(Context context) {
		this(context, null);
	}

	public XKPLine(Context context, AttributeSet attrs) {
		super(context, attrs);
		
		updateShapePosition();
	}
	
	@Override
	protected void updateShapePosition() {
		mPathShape.reset();
		mPathShape.moveTo(mX1, mY1);
		mPathShape.lineTo(mX2, mY2);
		
		mPathShape.computeBounds(mBounds, true);
		mPathShape.transform(mMtxAngle);
		
		mDrawable = new ShapeDrawable(new PathShape(mPathShape, mBounds.width(), mBounds.height()));
		mDrawable.setBounds(0, 0, (int) mBounds.width(), (int) mBounds.height());
	}
}
