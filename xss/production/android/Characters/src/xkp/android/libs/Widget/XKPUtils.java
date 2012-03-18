package xkp.android.libs.Widget;
import java.util.ArrayList;
import android.app.Activity;
import android.graphics.Bitmap;
import android.view.ViewGroup;
					import android.widget.ImageView;
public class XKPUtils {
	private Activity 				mActivity;
	private ArrayList<ViewGroup>	mViewGroup;
	private ArrayList<XKPPackage> 	mPackages;
	public XKPUtils() {
		mViewGroup = new ArrayList<ViewGroup>();
		mPackages = new ArrayList<XKPPackage>();
	}
	public void addView(ViewGroup viewgrp) {
		mViewGroup.add(viewgrp);
	}
	public void addView(Activity act) {
		mActivity = act;
	}
	public void addXKPPackage(XKPPackage pack) {
		mPackages.add(pack);
	}
	private int getResourceIdentifierFromPackages(String resource, XKPPackage [] outWrapperPack) {
		for(int i = 0; i < mPackages.size(); i++) {
			XKPPackage pack = mPackages.get(i);
			int resId = pack.getResourceIdentifier(resource);
			if(resId != -1) {
				outWrapperPack[0] = pack;
				return resId;
			}
		}
		outWrapperPack[0] = null;
		return -1;
	}
	public Bitmap getImageFromResource(String resource) {
		XKPPackage pack = null;
		XKPPackage [] wrapperPack = new XKPPackage [] { pack };  
		int resId = getResourceIdentifierFromPackages(resource, wrapperPack);
		pack = wrapperPack[0];
		if(pack != null) {
			return pack.getBitmapResource(resId);
		}
		return null;
	}
			public void setResourceImageView(int id, String resource) {
			ImageView img = (ImageView) mActivity.findViewById(id);
			if(img == null) {
				for (ViewGroup viewgrp : mViewGroup) {
					img = (ImageView) viewgrp.findViewById(id);
					if(img != null) break;
				}
			}
			if(img != null) img.setImageBitmap(getImageFromResource(resource));
		}
		public void setResourceImageView(ImageView img, String resource) {
			if(img != null) img.setImageBitmap(getImageFromResource(resource));
		}
}
