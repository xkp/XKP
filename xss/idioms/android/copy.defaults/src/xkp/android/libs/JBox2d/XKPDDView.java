<xss:parameter id="appName"/>
<xss:parameter id="base_namespace"/>

package <xss:e value="base_namespace"/>.libs.JBox2d;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.Canvas;
import android.view.View;

public class XKPDDView extends View {

	private Canvas 			mCanvas;
	private Bitmap			mBitmap;
	private XKPDebugDraw 	mDrawn;
	
	public XKPDDView(Context context, int width, int height) {
		super(context);
		mDrawn = new XKPDebugDraw(this);
		
		mBitmap = Bitmap.createBitmap(width, height, Config.ARGB_8888);
		mCanvas = new Canvas(mBitmap);
	}
	
	public XKPDebugDraw getDebugDraw() {
		return mDrawn;
	}
	
	public Canvas getCanvas() {
		return mCanvas;
	}
	
	public void render() {
		invalidate();
	}
	
	@Override
	public void onDraw(Canvas canvas) {
		super.onDraw(canvas);
		
		if(mBitmap != null) {
			canvas.drawBitmap(mBitmap, 0, 0, null);
			
			//TIPS: hacky hoo for clean bitmap
			mBitmap.eraseColor(0);
		}
	}
}
