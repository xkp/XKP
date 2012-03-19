<xss:parameter id="appName"/>
<xss:parameter id="base_namespace"/>

package <xss:e value="base_namespace"/>.libs.Widget;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Handler;
import android.provider.MediaStore.Audio;
import android.provider.MediaStore.Video;

public class XKPPackage {
	
	public static final int BITMAP_RESOURCE 	= 0xff000001;
	public static final int VIDEO_RESOURCE 		= 0xff000002;
	public static final int AUDIO_RESOURCE 		= 0xff000003;
	
	protected OnResourcePackageListener mOnResourcePackageListener;
	
	private Resources			mResources;
	private boolean				mResourceLoaded;
	
	// xkp with xml component example:
	// <image id="img01" src="photo_1.jpg"/>
	
	// resource name of xkp identifier assigned with id property; 	ex: img01
	private ArrayList<String> 	mResourceXKPName;
	
	// resource name of android identifier; 						ex: photo_1
	private ArrayList<String> 	mResourceDroidName;
	
	// type of resource; 											ex: BITMAP_RESOURCE
	private ArrayList<Integer>	mResourceType;
	
	// resource identifier assigned by android; 					ex: R.drawable.photo_1
	private ArrayList<Integer>	mResourceId;
	
	// the key of hashmap is an android id; ex: R.drawable.photo_1 that is a number
	private HashMap<Integer, Bitmap> 	mBitmapResources;
	private HashMap<Integer, Video> 	mVideoResources;
	private HashMap<Integer, Audio> 	mAudioResources;
	
	public XKPPackage(Context ctx) {
		mResources 		= ctx.getResources();
		mResourceLoaded = false;
		
		mBitmapResources = new HashMap<Integer, Bitmap>();
		mVideoResources = new HashMap<Integer, Video>();
		mAudioResources = new HashMap<Integer, Audio>();
	}
	
	public XKPPackage(Context ctx, String [] resXKPName, String [] resDroidName, 
			Integer [] resType, Integer [] resId) {
		this(ctx);
		
		mResourceXKPName = new ArrayList<String>(Arrays.asList(resXKPName));
		mResourceDroidName = new ArrayList<String>(Arrays.asList(resDroidName));
		mResourceType = new ArrayList<Integer>(Arrays.asList(resType));
		mResourceId	= new ArrayList<Integer>(Arrays.asList(resId));
	}
	
	public XKPPackage(Context ctx, String [] resXKPName, String [] resDroidName, 
			Integer [] resType, Integer [] resId, boolean loadRes) {
		this(ctx, resXKPName, resDroidName, resType, resId);
		
		if(loadRes) {
			load();
		}
	}
	
	final Handler handler = new Handler();
	final Runnable runnablePackageLoaded = new Runnable() {
		@Override
		public void run() {
			performResourceLoaded();
		}
	};
	
	public boolean load() {
		if(mResourceLoaded)
			return true;

		Thread loader = new Thread() {
			@Override
			public void run() {
				try {
					//Thread.sleep(5000);
					loadResource();
				//} catch(InterruptedException e) {
				//	e.printStackTrace();
				} finally {
					mResourceLoaded = true;
					handler.post(runnablePackageLoaded);
				}
			}
		};
		loader.start();
		
		return mResourceLoaded;
	}
	
	public boolean isResourceLoaded() {
		return mResourceLoaded;
	}
	
	synchronized protected void loadResource() {
		for(int i = 0; i < mResourceId.size(); i++) {
			switch(mResourceType.get(i)) {
			case BITMAP_RESOURCE:
				Integer resId = mResourceId.get(i);
				Bitmap bitmap = BitmapFactory.decodeResource(mResources, resId);
				if(bitmap == null) {
					//throw(I)
				}
				mBitmapResources.put(resId, bitmap);
				break;
			case VIDEO_RESOURCE:
				break;
			case AUDIO_RESOURCE:
				break;
			}
		}
	}
	
	synchronized private void performResourceLoaded() {
		if(mResourceLoaded && mOnResourcePackageListener != null)
			mOnResourcePackageListener.onResourcePackageLoaded(this);
	}
	
	public Bitmap getBitmapResource(int resId) {
		return mBitmapResources.get(resId);
	}
	
	public Bitmap getBitmapResourceFromPosition(int position) {
		if(position >= 0 && position < mBitmapResources.size()) {
			int resId = mResourceId.get(position);
			return mBitmapResources.get(resId);
		}
		return null;
	}
	
	public int getResourceIdFromXKPName(String resName) {
		for(int i = 0; i < mResourceXKPName.size(); i++) {
			String name = mResourceXKPName.get(i); 
			if(name.contains(resName) || resName.contains(name))
				return mResourceId.get(i);
		}
		return -1;
	}
	
	public int getPositionFromXKPName(String resName) {
		for(int i = 0; i < mResourceXKPName.size(); i++) {
			String name = mResourceXKPName.get(i);
			if(name.contains(resName) || resName.contains(name))
				return i;
		}
		return -1;
	}
	
	public int getResourceIdFromDroidName(String resName) {
		for(int i = 0; i < mResourceDroidName.size(); i++) {
			String name = mResourceDroidName.get(i); 
			if(name.contains(resName) || resName.contains(name))
				return mResourceId.get(i);
		}
		return -1;
	}
	
	public int getPositionFromDroidName(String resName) {
		for(int i = 0; i < mResourceDroidName.size(); i++) {
			String name = mResourceDroidName.get(i); 
			if(name.contains(resName))
				return i;
		}
		return -1;
	}
	
	public int getResourcePosition(String resource) {
		// first, look at in xkp names
		Integer position = getPositionFromXKPName(resource);
		// then, in source identifiers
		if(position == -1)
			position = getPositionFromDroidName(resource);
		
		return position;
	}
	
	public int getResourceIdentifier(String resource) {
		// first, look at in xkp names
		Integer identifier = getResourceIdFromXKPName(resource);
		// then, in source identifiers
		if(identifier == -1)
			identifier = getResourceIdFromDroidName(resource);
		
		return identifier;
	}
	
	public int getResourceCount() {
		return mResourceXKPName.size();
	}
	
	public void setOnResourcePackageListener(OnResourcePackageListener l) {
		mOnResourcePackageListener = l;
	}
	
	public interface OnResourcePackageListener {
		void onResourcePackageLoaded(XKPPackage pack);
	}
}
