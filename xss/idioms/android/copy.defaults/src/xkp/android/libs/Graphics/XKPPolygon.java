<xss:parameter id="appName"/>
<xss:parameter id="base_namespace"/>

package <xss:e value="base_namespace"/>.libs.Graphics;

import java.util.ArrayList;

import android.content.Context;
import android.graphics.Path.FillType;
import android.graphics.Point;
import android.graphics.drawable.ShapeDrawable;
import android.graphics.drawable.shapes.PathShape;
import android.util.AttributeSet;

public class XKPPolygon extends XKPGraphics {

	public ArrayList<Point> mPoints = new ArrayList<Point>();
	
	public XKPPolygon(Context context) {
		this(context, null);
	}

	public XKPPolygon(Context context, AttributeSet attrs) {
		super(context, attrs);
		
		mPathShape.setFillType(FillType.EVEN_ODD);
	}
	
	@Override
	protected void updateShapePosition() {
		if(mPoints == null) return;
		
		mPathShape.reset();
		
		Boolean first = true;
		for(Point point : mPoints) {
			if(first) {
				mPathShape.moveTo(point.x, point.y);
				mLeftTop.set(point.x, point.y);
				mBottomRight.set(point.x, point.y);
				first = false;
			} else {
				mPathShape.lineTo(point.x, point.y);
			}
			
			mLeftTop.set(Math.min(mLeftTop.x, point.x), Math.min(mLeftTop.y, point.y));
			mBottomRight.set(Math.max(mBottomRight.x, point.x), Math.max(mBottomRight.y, point.y));
		}
		
		preCalcPosition(mLeftTop.x, mLeftTop.y, mBottomRight.x, mBottomRight.y);
		
		mPathShape.close();
		mPathShape.transform(mMtxAngle);
		
		mPathShape.computeBounds(mBounds, true);
		
		mDrawable = new ShapeDrawable(new PathShape(mPathShape, mBounds.width(), mBounds.height()));
		mDrawable.setBounds(0, 0, (int) mBounds.width(), (int) mBounds.height());
	}

	public void addPoint(Point point) {
		mPoints.add(point);
		
		updateShapePosition();
		setAngle(mAngle);
		
		invalidate();
	}
	
	public void addPoint(Integer x, Integer y) {
		addPoint(new Point(x, y));
	}
	
	public Point getPoint(Integer index) {
		if(index >= 0 && index < mPoints.size()) {
			return mPoints.get(index);
		}
		
		return new Point(-1, -1);
	}
	
	public Integer getPointX(Integer index) {
		Point p = getPoint(index);
		
		return p.x;
	}
	
	public Integer getPointY(Integer index) {
		Point p = getPoint(index);
		
		return p.y;
	}
	
	public Integer getPointsCount() {
		return mPoints.size();
	}
	
	public void setPoint(Integer index, Point point) {
		if(index >= 0 && index < mPoints.size()) {
			mPoints.get(index).set(point.x, point.y);
			updateShapePosition();
			setAngle(mAngle);
			
			invalidate();
		}
	}
	
	public void setPoint(Integer index, Integer x, Integer y) {
		setPoint(index, new Point(x, y));
	}
	
	public void setPointX(Integer index, Integer x) {
		setPoint(index, new Point(x, getPointY(index)));
	}
	
	public void setPointY(Integer index, Integer y) {
		setPoint(index, new Point(getPointX(index), y));
	}
}
