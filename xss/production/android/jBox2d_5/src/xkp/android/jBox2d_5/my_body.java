package xkp.android.jBox2d_5;
import java.util.ArrayList;
import java.util.Arrays;
import android.view.View;
import android.view.ViewTreeObserver;
import android.view.ViewTreeObserver.OnGlobalLayoutListener;
			import android.content.Context;
			import android.content.res.TypedArray;
			import android.util.AttributeSet;
			import android.view.LayoutInflater;
import xkp.android.libs.Layout.XKPLayout;
			import xkp.android.libs.Widget.XKPUtils;
						import xkp.android.libs.Graphics.XKPGraphics.OnClickInsideFigureListener;
						import xkp.android.libs.Graphics.XKPImage;
						import xkp.android.libs.Widget.XKPUtils;
		public class my_body 
						extends XKPImage
{
			public static XKPUtils util;
		private XKPImage layoutmy_body;
	private boolean mLayoutStarted = false;
			public my_body(Context context) {
				this(context, null);
			}
			public my_body(Context context, AttributeSet attrs) {
				super(context, attrs);
				initCallers();
				LayoutInflater li = (LayoutInflater) context.
					getSystemService(Context.LAYOUT_INFLATER_SERVICE);
				// TIPS: link xml file with inflater
				XKPImage root = (XKPImage) li.inflate(R.layout.my_body, null);
				// TIPS: and with all exists views
		// TIPS: create an observer for notify layout updated
		layoutmy_body = (XKPImage) root.findViewById(R.id.layoutmy_body);
		ViewTreeObserver mainLayoutViewTreeObserver = layoutmy_body.getViewTreeObserver();
		if(mainLayoutViewTreeObserver.isAlive()) {
			mainLayoutViewTreeObserver.addOnGlobalLayoutListener(new OnGlobalLayoutListener() {
				@Override
				public void onGlobalLayout() {
					if(!mLayoutStarted) {
						mLayoutStarted = true;
						onLayoutStarted();
					}
					onLayoutUpdated();
				}
			});
		}
			TypedArray ta = context.obtainStyledAttributes(
					attrs, R.styleable.my_body);
			ta.recycle();
			}
private void initCallers() {
			util = new XKPUtils();
			ActjBox2d_5.util.addView(this);
			util = ActjBox2d_5.util;
}
private void onLayoutUpdated() {
}
private void onLayoutStarted() {
}
			private void collisionmy_body(Object who) {
			}
}
