package xkp.android.Characters;
import java.util.ArrayList;
import java.util.Arrays;
import android.view.View;
			import android.content.Context;
			import android.content.res.TypedArray;
			import android.util.AttributeSet;
			import android.view.LayoutInflater;
						import android.widget.ImageView;
						import android.view.View.OnClickListener;
						import android.view.View.OnLongClickListener;
						import android.view.MotionEvent;
						import android.view.View.OnTouchListener;
						import android.view.View.OnKeyListener;
						import android.view.View.OnFocusChangeListener;
						import android.widget.TextView;
						import android.text.method.KeyListener;
		public class character 
						extends XKPLayout
{
				private ImageView picture;
					XKPUtils util = new XKPUtils();
				private TextView lname;
				private TextView lsurname;
				private TextView lemployment;
			public character(Context context) {
				this(context, null);
			}
			public character(Context context, AttributeSet attrs) {
				super(context, attrs);
				LayoutInflater li = (LayoutInflater) context.
					getSystemService(Context.LAYOUT_INFLATER_SERVICE);
				// link xml file with inflater
				View root = li.inflate(R.layout.character, this);
				// and with all views
				picture = (ImageView) root.findViewById(R.id.picture);
				lname = (TextView) root.findViewById(R.id.lname);
				lsurname = (TextView) root.findViewById(R.id.lsurname);
				lemployment = (TextView) root.findViewById(R.id.lemployment);
			TypedArray ta = context.obtainStyledAttributes(
					attrs, R.styleable.character);
				prop_character_name_set(ta.getString(R.styleable.character_name));
				prop_character_surname_set(ta.getString(R.styleable.character_surname));
				prop_character_employment_set(ta.getString(R.styleable.character_employment));
				prop_character_img_src_set(ta.getString(R.styleable.character_img_src));
			ta.recycle();
			}
void initCallers() {
}
			public String prop_character_name_get() {
				return lname.getText().toString();
			}
			public void prop_character_name_set(String value) {
				lname.setText(value);
			}
			public String prop_character_surname_get() {
				return lsurname.getText().toString();
			}
			public void prop_character_surname_set(String value) {
				lsurname.setText(value);
			}
			public String prop_character_employment_get() {
				return lemployment.getText().toString();
			}
			public void prop_character_employment_set(String value) {
				lemployment.setText(value);
			}
			private String prop_character_img_src;
			public void prop_character_img_src_set(String value) {
				prop_character_img_src = value;
				util.setResourceImageView(R.id.picture, value);
			}
		public void mthd_character_foo() {
			prop_character_name_set("foo");
		}
		public void mthd_character_bar() {
			prop_character_surname_set("bar");
		}
					public class XKPUtils {
						void setResourceImageView(int id, String resource) {
			int rpos = resource.lastIndexOf("/");
			String n_res = resource.substring(rpos + 1);
			rpos = n_res.lastIndexOf(".");
			String res = n_res.substring(0, rpos);
			int resId = getResources().getIdentifier(
							res, "drawable", 
							getContext().getPackageName());
			ImageView img = (ImageView) findViewById(id);
			img.setImageResource(resId);
		}
			}
}
