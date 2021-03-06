package xkp.android.Characters;
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
						import android.view.View.OnClickListener;
						import android.view.View.OnLongClickListener;
						import android.view.MotionEvent;
						import android.view.View.OnTouchListener;
						import android.view.View.OnKeyListener;
						import android.view.KeyEvent;
						import android.view.View.OnFocusChangeListener;
						import android.widget.ImageView;
						import xkp.android.libs.Widget.XKPUtils;
						import android.text.method.KeyListener;
						import android.widget.TextView;
		public class character 
						extends XKPLayout
{
				private ImageView picture;
				private TextView lname;
				private TextView lsurname;
				private TextView lemployment;
			public static XKPUtils util;
		private XKPLayout layoutcharacter;
	private boolean mLayoutStarted = false;
			public character(Context context) {
				this(context, null);
			}
			public character(Context context, AttributeSet attrs) {
				super(context, attrs);
				initCallers();
				LayoutInflater li = (LayoutInflater) context.
					getSystemService(Context.LAYOUT_INFLATER_SERVICE);
				// TIPS: link xml file with inflater
				XKPLayout root = (XKPLayout) li.inflate(R.layout.character, this);
				// TIPS: and with all exists views
		// TIPS: create an observer for notify layout updated
		layoutcharacter = (XKPLayout) root.findViewById(R.id.layoutcharacter);
		ViewTreeObserver mainLayoutViewTreeObserver = layoutcharacter.getViewTreeObserver();
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
				picture = (ImageView) root.findViewById(R.id.picture);
				lname = (TextView) root.findViewById(R.id.lname);
				lsurname = (TextView) root.findViewById(R.id.lsurname);
				lemployment = (TextView) root.findViewById(R.id.lemployment);
			TypedArray ta = context.obtainStyledAttributes(
					attrs, R.styleable.character);
				name_set(ta.getString(R.styleable.character_name));
				surname_set(ta.getString(R.styleable.character_surname));
				employment_set(ta.getString(R.styleable.character_employment));
				img_src_set(ta.getString(R.styleable.character_img_src));
			ta.recycle();
			}
private void initCallers() {
			util = new XKPUtils();
			ActCharacters.util.addView(this);
			util = ActCharacters.util;
}
private void onLayoutUpdated() {
}
private void onLayoutStarted() {
}
			public String name_get() {
				return lname.getText().toString();
			}
			public void name_set(String value) {
				lname.setText(value);
			}
			public String surname_get() {
				return lsurname.getText().toString();
			}
			public void surname_set(String value) {
				lsurname.setText(value);
			}
			public String employment_get() {
				return lemployment.getText().toString();
			}
			public void employment_set(String value) {
				lemployment.setText(value);
			}
			private String img_src;
			public void img_src_set(String value) {
				img_src = value;
				util.setResourceImageView(picture, value);
			}
		public void foo() {
			name_set("foo");
		}
		public void bar() {
			surname_set("bar");
		}
}
