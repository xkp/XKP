<xss:parameter id="appName"/>
<xss:parameter id="base_namespace"/>

package <xss:e value="base_namespace"/>.libs.JBox2d;

import java.lang.reflect.Constructor;
import java.util.ArrayList;
import java.util.List;

import org.jbox2d.common.Vec2;

import xkp.android.libs.Graphics.XKPGraphics;
import xkp.android.libs.Layout.XKPLayout;
import android.content.Context;

public class XKPSpawner {

	private Context		mContext;
	private XKPLayout	mViewLayout;
	private float 		mRotation = 0.0f;
	private Vec2 		mPosition = new Vec2(0, 0);
	private float		mFrequency = 1.0f;
	
	private String 		mClassName;
	private Class		mClass;
	private Constructor mConstructor;
	
	private List 		mInstances = new ArrayList();
	
	private android.os.Handler mHandler;

	public XKPSpawner(Context ctx, XKPLayout viewLayout, String className, Vec2 position, float rotation) {
		mContext = ctx;
		mViewLayout = viewLayout;
		mClassName = "<xss:e value="base_namespace"/>.<xss:e value="appName"/>." + className;
		mRotation = rotation;
		if (position != null)
			mPosition = position;
		
		prepareClass();
	}

	public XKPSpawner(Context ctx, XKPLayout viewLayout, String className, Integer x, Integer y, float rotation) {
		this(ctx, viewLayout, className, new Vec2(x, y), rotation);
	}
	
	public XKPSpawner(Context ctx, XKPLayout viewLayout, String className) {
		this(ctx, viewLayout, className, null, 0);
	}

	public void start() {
		mHandler = new android.os.Handler();
		mHandler.post(mRunnableSpawner);
	}

	public void stop() {
		mHandler.removeCallbacks(mRunnableSpawner);
	}
	
	private final Runnable mRunnableSpawner = new Runnable() {
		public void run() {
			spawn();
			mHandler.postDelayed(mRunnableSpawner, (long) (mFrequency * 1000));
		}
	};

	private void prepareClass() {
		try {
			Class mClass = Class.forName(mClassName);
			mConstructor = mClass.getConstructor(Context.class);
			
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public void spawn() {
		try {
			XKPGraphics newInstance = (XKPGraphics) mConstructor.newInstance(mContext);
			newInstance.setPosition((int) mPosition.x, (int) mPosition.y);
			newInstance.setRotation(mRotation);

			mInstances.add(newInstance);
			mViewLayout.addView(newInstance);
			
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public boolean isAlive() {
		return false;
	}

	public void setRotation(float angle) {
		mRotation = angle;
	}

	public float getRotation() {
		return mRotation;
	}

	public void setPosition(Vec2 position) {
		if (position == null)
			return;
		mPosition = position;
	}

	public void setPosition(Integer x, Integer y) {
		mPosition = new Vec2(x, y);
	}
	
	public Vec2 getPosition() {
		return mPosition;
	}
	
	public void setFrequency(float frequency) {
		mFrequency = frequency;
	}
	
	public float getFrequency() {
		return mFrequency;
	}
}
