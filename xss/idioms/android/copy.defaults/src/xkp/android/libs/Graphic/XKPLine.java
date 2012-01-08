<xss:parameter id="appName"/>
<xss:parameter id="base_namespace"/>

package <xss:e value="base_namespace"/>.libs.Graphic;

import android.content.Context;
import android.graphics.RectF;
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
		
		RectF bounds = new RectF();
		mPathShape.computeBounds(bounds, true);
		
		mDrawable = new ShapeDrawable(new PathShape(mPathShape, bounds.width(), bounds.height()));
		mDrawable.setBounds(0, 0, (int) bounds.width(), (int) bounds.height());
	}
	
	@Override
	protected void updateShapePosition() {
		mPathShape.reset();
		mPathShape.moveTo(mX1, mY1);
		mPathShape.lineTo(mX2, mY2);
	}
}
