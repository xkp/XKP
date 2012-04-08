<xss:parameter id="appName"/>
<xss:parameter id="base_namespace"/>

package <xss:e value="base_namespace"/>.libs.JBox2d;

import org.jbox2d.callbacks.DebugDraw;
import org.jbox2d.common.Color3f;
import org.jbox2d.common.OBBViewportTransform;
import org.jbox2d.common.Transform;
import org.jbox2d.common.Vec2;

import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.Log;

public class XKPDebugDraw extends DebugDraw {

	private Canvas 		mRefCanvas;
	private XKPDDView 	mDDView;
	
	//TODO: draw all figures with Drawable class
	//TODO: create a method for convert float to android colors 
	public XKPDebugDraw(XKPDDView view) {
		super(new OBBViewportTransform());
		
		//viewportTransform.setYFlip(true);
		//viewportTransform.setExtents(320, 420);
		mDDView = view;
		mRefCanvas = view.getCanvas();
		
		//setCamera(0, 0, 320 / 100);
	}
	
	public XKPDDView getDDView() {
		return mDDView;
	}
	
	@Override
	public void drawCircle(Vec2 center, float radius, Color3f color) {
		Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);
		paint.setColor(Color.rgb((int)color.x, (int)color.y, (int)color.z));
		mDDView.getCanvas().drawCircle(center.x, center.y, radius, paint);
		Log.d(this.getClass().getSimpleName(), "drawCircle(...)");
	}

	@Override
	public void drawPoint(Vec2 point, float radiusOnScreen, Color3f color) {
		Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);
		paint.setColor(Color.rgb((int) (color.x * 100), (int) (color.y * 100), (int) (color.z * 100) ));
		mDDView.getCanvas().drawPoint(point.x, point.y, paint);
		//Log.d(this.getClass().getSimpleName(), "drawPoint(...)");
	}

	@Override
	public void drawSegment(Vec2 p1, Vec2 p2, Color3f color) {
		Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);
		paint.setColor(Color.rgb((int) (color.x * 100), (int) (color.y * 100), (int) (color.z * 100) ));
		mDDView.getCanvas().drawLine(p1.x, p1.y, p2.x, p2.y, paint);
		//Log.d(this.getClass().getSimpleName(), "drawSegment(...)");
	}

	@Override
	public void drawSolidCircle(Vec2 center, float radius, Vec2 axis, Color3f color) {
		Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);
		paint.setColor(Color.rgb((int) (color.x * 100), (int) (color.y * 100), (int) (color.z * 100) ));
		mDDView.getCanvas().drawCircle(center.x, center.y, radius, paint);
		//Log.d(this.getClass().getSimpleName(), "drawSolidCircle(...)");
	}

	@Override
	public void drawSolidPolygon(Vec2[] vertices, int vertexCount, Color3f color) {
		Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);
		paint.setColor(Color.rgb((int) (color.x * 100), (int) (color.y * 100), (int) (color.z * 100) ));
		for(int i = 0; i < vertexCount; ++i) {
			int ind = (i + 1 < vertexCount) ? i + 1 : (i + 1 - vertexCount);
			Vec2 v1 = vertices[i];
			Vec2 v2 = vertices[ind];
			mDDView.getCanvas().drawLine(v1.x, v1.y, v2.x, v2.y, paint);
		}
		//Log.d(this.getClass().getSimpleName(), "drawSolidPolygon(...)");
	}

	@Override
	public void drawString(float arg0, float arg1, String arg2, Color3f arg3) {
		Log.d(this.getClass().getSimpleName(), "drawString(...)");
	}

	@Override
	public void drawTransform(Transform arg0) {
		Log.d(this.getClass().getSimpleName(), "drawTransform(...)");
	}

}
