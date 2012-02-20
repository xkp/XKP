package xkp.android.libs.Widget;
import android.content.Context;
import android.content.res.TypedArray;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;
import xkp.android.libs.Layout.XKPLayout;
import xkp.android.Contacts.R;
public class XKPSwitcher extends XKPLayout {
	private int mCurrentChild = 0;
	private int mResCurrentChild = 0;
	public XKPSwitcher(Context context) {
		super(context);
	}
	public XKPSwitcher(Context context, AttributeSet attrs) {
		super(context, attrs);
		TypedArray a = context.obtainStyledAttributes(attrs, R.styleable.XKPSwitcher);
		mResCurrentChild = a.getResourceId(R.styleable.XKPSwitcher_current, 0);
		a.recycle();
	}
    /**
     * Sets which child view will be displayed.
     * 
     * @param whichChild the index of the child view to display
     */
    public void setDisplayedChild(int whichChild) {
    	mCurrentChild = whichChild;
        if (whichChild >= getChildCount()) {
        	mCurrentChild = 0;
        } else if (whichChild < 0) {
        	mCurrentChild = getChildCount() - 1;
        }
        boolean hasFocus = getFocusedChild() != null;
        // This will clear old focus if we had it
        showOnly(mCurrentChild);
        if (hasFocus) {
            // Try to retake focus if we had it
            requestFocus(FOCUS_FORWARD);
        }
    }
    /**
     * Returns the index of the currently displayed child view.
     */
    public int getDisplayedChild() {
        return mCurrentChild;
    }
    /**
     * Sets which child view will be displayed.
     * 
     * @param whichChild the resource identifier of the child view to display
     */
    public void setDisplayedChildByName(int whichChild) {
        final int count = getChildCount();
        for (int i = 0; i < count; i++) {
            final View child = getChildAt(i);
            if (whichChild == child.getId()) {
				mCurrentChild = i;
				boolean hasFocus = getFocusedChild() != null;
				// This will clear old focus if we had it
				showOnly(mCurrentChild);
				if (hasFocus) {
					// Try to retake focus if we had it
					requestFocus(FOCUS_FORWARD);
				}
				break;
            }
        }
    }
    /**
     * Returns the resource identifier of the currently displayed child view.
     */
	public int getDisplayedChildByName() {
		return getChildAt(mCurrentChild).getId();
	}
    /**
     * Manually shows the next child.
     */
    public void showNext() {
        setDisplayedChild(mCurrentChild + 1);
    }
    /**
     * Manually shows the previous child.
     */
    public void showPrevious() {
        setDisplayedChild(mCurrentChild - 1);
    }
    /**
     * Shows only the specified child. The other displays Views exit the screen
     * with the {@link #getOutAnimation() out animation} and the specified child
     * enters the screen with the {@link #getInAnimation() in animation}.
     *
     * @param childIndex The index of the child to be shown.
     */
    private void showOnly(int childIndex) {
        final int count = getChildCount();
        for (int i = 0; i < count; i++) {
            final View child = getChildAt(i);
            if (i == childIndex) {
                child.setVisibility(View.VISIBLE);
            } else {
                child.setVisibility(View.GONE);
            }
        }
    }
    @Override
    public void addView(View child, int index, ViewGroup.LayoutParams params) {
        super.addView(child, index, params);
        if (mResCurrentChild != 0 && child.getId() == mResCurrentChild) {
        	mCurrentChild = getChildCount() - 1;
        	showOnly(mCurrentChild);
        	mResCurrentChild = 0;
        } else 
        if (getChildCount() == 1) {
            child.setVisibility(View.VISIBLE);
        	mCurrentChild = getChildCount() - 1;
    	} else {
            child.setVisibility(View.GONE);
        }
    }
    @Override
    public void removeAllViews() {
        super.removeAllViews();
        mCurrentChild = 0;
    }
    @Override
    public void removeView(View view) {
        final int index = indexOfChild(view);
        if (index >= 0) {
            removeViewAt(index);
        }
    }
    @Override
    public void removeViewAt(int index) {
        super.removeViewAt(index);
        final int childCount = getChildCount();
        if (childCount == 0) {
        	mCurrentChild = 0;
        } else if (mCurrentChild >= childCount) {
            // Displayed is above child count, so float down to top of stack
            setDisplayedChild(childCount - 1);
        } else if (mCurrentChild == index) {
            // Displayed was removed, so show the new child living in its place
            setDisplayedChild(mCurrentChild);
        }
    }
    public void removeViewInLayout(View view) {
        removeView(view);
    }
    public void removeViews(int start, int count) {
        super.removeViews(start, count);
        if (getChildCount() == 0) {
        	mCurrentChild = 0;
        } else if (mCurrentChild >= start && mCurrentChild < start + count) {
            // Try showing new displayed child, wrapping if needed
            setDisplayedChild(mCurrentChild);
        }
    }
    public void removeViewsInLayout(int start, int count) {
        removeViews(start, count);
    }
    /**
     * Returns the View corresponding to the currently displayed child.
     *
     * @return The View currently displayed.
     *
     * @see #getDisplayedChild()
     */
    public View getCurrentView() {
        return getChildAt(mCurrentChild);
    }
    @Override
    public int getBaseline() {
        return (getCurrentView() != null) ? getCurrentView().getBaseline() : super.getBaseline();
    }
}
