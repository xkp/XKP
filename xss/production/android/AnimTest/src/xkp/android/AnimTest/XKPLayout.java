package xkp.android.AnimTest;
import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewParent;
public class XKPLayout extends ViewGroup {
	private static final boolean DEBUG = false;
	private int mPaddingLeft = 0;
	private int mPaddingRight = 0;
	private int mPaddingTop = 0;
	private int mPaddingBottom = 0;
	public static final int PL_NONE   = 0;
	public static final int PL_TOP    = 1;
	public static final int PL_LEFT   = 2;
	public static final int PL_RIGHT  = 3;
	public static final int PL_BOTTOM = 4;
	public static final int PL_CLIENT = 5;
	public static final int PL_CENTER = 6;
	private static final int PL_MIN_VALUE = 0;
	private static final int PL_MAX_VALUE = 7;
	private XKPLayout.LayoutParams mXKPLayoutParams;
	public XKPLayout(Context context) {
		super(context);
		updatePadding();
	}
	public XKPLayout(Context context, AttributeSet attrs) {
		super(context, attrs);
		mXKPLayoutParams = (XKPLayout.LayoutParams) generateLayoutParams(attrs);
		updatePadding();
	}
	public XKPLayout(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		mXKPLayoutParams = (XKPLayout.LayoutParams) generateLayoutParams(attrs);
		updatePadding();
	}
	private void updatePadding() {
		mPaddingLeft = getPaddingLeft();
		mPaddingTop = getPaddingTop();
		mPaddingRight = getPaddingRight();
		mPaddingBottom = getPaddingBottom();
	}
    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
		int widthLayout = MeasureSpec.getSize(widthMeasureSpec);
		int heightLayout = MeasureSpec.getSize(heightMeasureSpec);
		int maxWidth = widthLayout;
		int maxHeight = heightLayout;
		// Account for padding too
		maxWidth += mPaddingLeft - mPaddingRight;
		maxHeight += mPaddingTop - mPaddingBottom;
		// Find out how big everyone wants to be
		measureChildren(widthMeasureSpec, heightMeasureSpec);
		//Rect bounds = new Rect(0, 0, widthLayout, heightLayout);
		Rect bounds = new Rect(mPaddingLeft, mPaddingTop, maxWidth, maxHeight);
        final int childCount = getChildCount();
    	for(int i = 0; i < childCount; ++i) {
    		final View child = getChildAt(i);
    		if(child.getVisibility() != GONE) {
				XKPLayout.LayoutParams lp
					= (XKPLayout.LayoutParams) child.getLayoutParams();
				// update origins dimensions 
				if(lp.origins.right < 0)
					lp.origins.right = lp.origins.left + mPaddingLeft + child.getMeasuredWidth() - mPaddingRight;
				if(lp.origins.bottom < 0)
					lp.origins.bottom = lp.origins.top + mPaddingTop + child.getMeasuredHeight() - mPaddingBottom;
		        int childWidth = lp.origins.width();
		        int childHeight = lp.origins.height();
		        if(DEBUG) {
					Log.d("XKPLayout", "onMeasure => placement:  " + lp.placement);
					Log.d("XKPLayout", "onMeasure => left:" + lp.origins.left + " top:" + lp.origins.top +
							" right:" + lp.origins.right + " bottom:" + lp.origins.bottom);
		        }
				switch(lp.placement) {
					case PL_NONE:
						lp.x = lp.origins.left;
						lp.y = lp.origins.top;
						break;
					case PL_TOP:
						childWidth = bounds.width();
						lp.x = bounds.left;
						lp.y = bounds.top;
						bounds.top += childHeight;
						break;
					case PL_BOTTOM:
						childWidth = bounds.width();
						lp.x = bounds.left;
						lp.y = bounds.bottom - childHeight;
						bounds.bottom -= childHeight;
						break;
					case PL_LEFT:
		                childHeight = bounds.height();
						lp.x = bounds.left;
						lp.y = bounds.top;
						bounds.left += childWidth;
		                break;
					case PL_RIGHT:
		                childHeight = bounds.height();
						lp.x = bounds.right - childWidth;
						lp.y = bounds.top;
						bounds.right -= childWidth;
						break;
					case PL_CLIENT:
						childWidth = bounds.width();
		                childHeight = bounds.height();
						lp.x = bounds.left;
						lp.y = bounds.top;
		                break;
					case PL_CENTER:
						lp.x = bounds.left + bounds.width() / 2 - childWidth / 2;
						lp.y = bounds.top + bounds.height() / 2 - childHeight / 2;
		                break;
				} //switch
				child.measure(
						MeasureSpec.makeMeasureSpec(childWidth, MeasureSpec.EXACTLY), 
						MeasureSpec.makeMeasureSpec(childHeight, MeasureSpec.EXACTLY));
				if(mXKPLayoutParams instanceof XKPLayout.LayoutParams) { 
					if(mXKPLayoutParams.autosize_x) {
						if(child.getRight() > widthLayout) {
							maxWidth = Math.max(maxWidth, child.getRight());
						}
					}
					if(mXKPLayoutParams.autosize_y) {
						if(lp.origins.bottom > heightLayout) {
							maxHeight = Math.max(maxHeight, lp.origins.bottom);
						}
					}
				}
				maxWidth = Math.max(maxWidth, childWidth);
				maxHeight = Math.max(maxHeight, childHeight);
    		}
    	} //for
		// Check against minimum height and width
		maxWidth = Math.max(maxWidth, getSuggestedMinimumWidth());
		maxHeight = Math.max(maxHeight, getSuggestedMinimumHeight());
		int resvSizeW = resolveSize(
				maxWidth,
				MeasureSpec.makeMeasureSpec(maxWidth, MeasureSpec.EXACTLY)); 
		int resvSizeH = resolveSize(
				maxHeight,
				MeasureSpec.makeMeasureSpec(maxHeight, MeasureSpec.EXACTLY));
		setMeasuredDimension(resvSizeW, resvSizeH);
		ViewParent v = getParent();
		if (v != null) {
			if(DEBUG) {
				Log.d("XKPLayout", "onMeasure => " + v.toString());
			}
			v.requestLayout();
		}
    }
	@Override
	public String toString() {
		return "xkp.android.XKPLayout";
    }
	@Override
	protected ViewGroup.LayoutParams generateDefaultLayoutParams() {
		return new LayoutParams(android.view.ViewGroup.LayoutParams.WRAP_CONTENT, android.view.ViewGroup.LayoutParams.WRAP_CONTENT, 0, 0);
	}
	@Override
	protected void onLayout(boolean changed, int l, int t, int r, int b) {
        final int childCount = getChildCount();
    	for(int i = 0; i < childCount; ++i) {
    		final View child = getChildAt(i);
    		if (child.getVisibility() != GONE) {
    			XKPLayout.LayoutParams lp =
					(XKPLayout.LayoutParams) child.getLayoutParams();
    			if(DEBUG) {
					Log.d("XKPLayout", "onLayout => left:" + lp.origins.left + " top:" + lp.origins.top +
							" right:" + lp.origins.right + " bottom:" + lp.origins.bottom);
    			}
				int childLeft = mPaddingLeft + lp.x;
				int childTop = mPaddingTop + lp.y;
				child.layout(childLeft, childTop,
						childLeft + child.getMeasuredWidth(),
						childTop + child.getMeasuredHeight());
				if (lp.placement == PL_NONE) {
					lp.origins.set(lp.x, lp.y, lp.x + lp.width, lp.y + lp.height);
				}
			}
		}
	}
	@Override
	public ViewGroup.LayoutParams generateLayoutParams(AttributeSet attrs) {
		return new XKPLayout.LayoutParams(getContext(), attrs, this);
	}
	// Override to allow type-checking of LayoutParams.
	@Override
	protected boolean checkLayoutParams(ViewGroup.LayoutParams p) {
		return p instanceof XKPLayout.LayoutParams;
	}
	//@Override
	//protected ViewGroup.LayoutParams generateLayoutParams(ViewGroup.LayoutParams p) {
	//	return new LayoutParams(p);
	//}
	@Override
	protected ViewGroup.LayoutParams generateLayoutParams(ViewGroup.LayoutParams p) {
		return new LayoutParams( (XKPLayout.LayoutParams)p );
	}
	//protected XKPLayout.LayoutParams generateLayoutParams(XKPLayout.LayoutParams p) {
	//	return new LayoutParams(p);
	//}
	/*
	Per-child layout information associated with AbsoluteLayout.
	* See
	* {@link android.R.styleable#AbsoluteLayout_Layout Absolute Layout Attributes}
	* for a list of all child view attributes that this class supports. 
	*/
	public static class LayoutParams extends ViewGroup.LayoutParams {
		/* 
		 * The horizontal, or X, location of the child within the view group.
		 */
		public int x;
		/* 
		 * The vertical, or Y, location of the child within the view group.
		 */
		public int y;
		/* 
		 * The placement location of the child within the view group.
		 */
		//public Placement placement; 
		private int placement;
		/*
		 * Backup of original coordinates. 
		 */
		public Rect origins;
		/*
		 * Autoadjust width measures when a new component is added to a ViewGroup.
		 */
		private boolean autosize_x;
		/**
		 * Autoadjust height measures when a new component is added to a ViewGroup.
		 */
		private boolean autosize_y;
		/**
		 * Reference to parent XKPLayout.
		 */
		private XKPLayout parentXkpLayout;
		/** 
		 * Creates a new set of layout parameters with the specified width,
		 * height and location.
		 * @param width the width, either {@link #MATCH_PARENT},
		 * {@link #WRAP_CONTENT} or a fixed size in pixels
		 * @param height the height, either {@link #MATCH_PARENT},
		 * {@link #WRAP_CONTENT} or a fixed size in pixels
		 * @param x the X location of the child
		 * @param y the Y location of the child
		 */
		public LayoutParams(int width, int height, int x, int y) {
			super(width, height);
			this.x = x;
			this.y = y;
			this.placement = PL_NONE;
			this.origins = new Rect(x, y, x + width, y + height);
			this.autosize_x = false;
			this.autosize_y = false;
			this.parentXkpLayout = null;
		}
		/* 
		* Creates a new set of layout parameters. The values are extracted from
		* the supplied attributes set and context. The XML attributes mapped
		* to this set of layout parameters are:
		* &lt;ul>
		* <li><code>layout_x</code>: the X location of the child</li>
		* <li><code>layout_y</code>: the Y location of the child</li>
		* <li>All the XML attributes from
		* {@link android.view.ViewGroup.LayoutParams}</li>
		* </ul>
		* @param c the application environment
		* @param attrs the set of attributes from which to extract the layout
		* parameters values
		*/ 
		public LayoutParams(Context c, AttributeSet attrs, XKPLayout layRef) {
			super(c, attrs);
			TypedArray a = c.obtainStyledAttributes(attrs,
					R.styleable.XKPLayout);
			this.x = a.getDimensionPixelOffset(
					R.styleable.XKPLayout_layout_x, 0);
			this.y = a.getDimensionPixelOffset(
					R.styleable.XKPLayout_layout_y, 0);
			this.autosize_x = a.getBoolean(
					R.styleable.XKPLayout_autosize_x, false);
			this.autosize_y = a.getBoolean(
					R.styleable.XKPLayout_autosize_y, false);
			int pl = a.getInt(
					R.styleable.XKPLayout_placement, 0);
			a.recycle();
			int newWidth = width;
			if(width >= 0)
				newWidth = x + width;
			int newHeight = height;
			if(height >= 0)
				newHeight = y + height;
			this.origins = new Rect(x, y, newWidth, newHeight);
			this.parentXkpLayout = layRef;
			setPlacement(pl);
			if(DEBUG) {
				Log.d("XKPLayout", "LayoutParams => x:" + x + " y:" + y +
						" w:" + width + " h:" + height);
				Log.d("XKPLayout", "LayoutParams => placement = " + pl);
			}
		}
		public int getPlacement() {
			return placement;
		}
		public void setPlacement(int pl) {
			if (pl < PL_MIN_VALUE) {
				pl = Math.abs(pl);
			}
			pl %= PL_MAX_VALUE;
			this.placement = pl;
			if (parentXkpLayout != null) {
				parentXkpLayout.requestLayout();
			}
		}
		public boolean getAutoSizeX() {
			return autosize_x;
		}
		public boolean getAutoSizeY() {
			return autosize_y;
		}
		public void setAutoSizeX(boolean value) {
			this.autosize_x = value;
			if (parentXkpLayout != null) {
				parentXkpLayout.requestLayout();
			}
		}
		public void setAutoSizeY(boolean value) {
			this.autosize_y = value;
			if (parentXkpLayout != null) {
				parentXkpLayout.requestLayout();
			}
		}
		public void setLeft(int left) {
			this.x = left;
			this.origins.left = left;
			if (parentXkpLayout != null) {
				parentXkpLayout.requestLayout();
			}
		}
		public void setTop(int top) {
			this.y = top;
			this.origins.top = top;
			if (parentXkpLayout != null) {
				parentXkpLayout.requestLayout();
			}
		}
		public void setWidth(int width) {
			this.width = width;
			this.origins.right = this.origins.left + width; 
			if (parentXkpLayout != null) {
				parentXkpLayout.requestLayout();
			}
		}
		public void setHeight(int height) {
			this.height = height;
			this.origins.bottom = this.origins.top + height; 
			if (parentXkpLayout != null) {
				parentXkpLayout.requestLayout();
			}
		}
		public LayoutParams(XKPLayout.LayoutParams source) {
			super(source);
			this.x = source.x;
			this.y = source.y;
			this.autosize_x = source.autosize_x;
			this.autosize_y = source.autosize_y;
			this.placement = source.placement;
			this.origins = new Rect(source.origins);
			this.parentXkpLayout = source.parentXkpLayout;
		}
	}
}
