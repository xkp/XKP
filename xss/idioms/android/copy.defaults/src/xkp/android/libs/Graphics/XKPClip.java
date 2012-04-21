<xss:parameter id="appName"/>
<xss:parameter id="base_namespace"/>

package <xss:e value="base_namespace"/>.libs.Graphics;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path.Direction;
import android.util.AttributeSet;
import android.view.View;

import <xss:e value="base_namespace"/>.<xss:e value="appName"/>.R
import <xss:e value="base_namespace"/>.libs.Layout.XKPLayout;

public class XKPClip extends XKPGraphics {

	public static final int FRM_RECT 	= 0;
	public static final int FRM_CIRCLE	= 1;
	public static final int FRM_POLYGON	= 2;
	
	protected Integer mForm	= FRM_RECT;
	
	//private final static boolean CLIP_BOUNDS = true;
	private final static boolean CLIP_BOUNDS = false;
	
	public XKPClip(Context context) {
		this(context, null);
	}
	
	public XKPClip(Context context, AttributeSet attrs) {
		super(context, attrs);

		TypedArray ta = context.obtainStyledAttributes(
				attrs, R.styleable.XKPClip);
		
		mForm = ta.getInteger(R.styleable.XKPClip_form, FRM_RECT);
		
		ta.recycle();
	}
	
	@Override
	protected void onDraw(Canvas canvas) {
		
		View parent = (View)getParent();
		if(parent instanceof XKPLayout) {
			mPathClipRef = ((XKPLayout)parent).getPathClip();
		} else {
			super.onDraw(canvas);
			return;
		}
		
		// apply clip to the canvas
	    Paint paint = new Paint();
	    paint.setStyle(Paint.Style.STROKE);
	    paint.setStrokeWidth(1);
	    paint.setColor(Color.RED);
	    paint.setAntiAlias(true);
    
    	switch(mForm) {
    	case FRM_RECT:
    		if(CLIP_BOUNDS) canvas.drawRect(mRectF, paint);
    		mPathClipRef.addRect(mRectF, Direction.CCW);
    		break;
    		
    	case FRM_CIRCLE:
    		if(CLIP_BOUNDS) canvas.drawArc(mRectF, 0, 360, true, paint);
    		mPathClipRef.addOval(mRectF, Direction.CCW);
    		break;
    		
    	case FRM_POLYGON:
    		break;
    	}
	}
	
	public void setForm(Integer form) {
		mForm = form;
	}
	
	public Integer getForm() {
		return mForm;
	}
}
