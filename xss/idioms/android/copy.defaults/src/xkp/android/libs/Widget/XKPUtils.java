<xss:parameter id="appName"/>
<xss:parameter id="base_namespace"/>
<xss:parameter id="bundle"/>
<xss:parameter id="idiom_path"/>

package <xss:e value="base_namespace"/>.libs.Widget;

<xss:code>
	if(bundle)
	{
		for(var item in bundle)
		{
			string srcf = idiom_path + "/class.xss/" + item.file_name;
			compiler.xss(srcf, marker = "methods");
			
			if(item.import)
			{
				out(marker = "imports")
				{
					import <xss:e value="item.import"/>;
				}
			}
		}
	}
</xss:code>

import java.util.ArrayList;

import android.app.Activity;
import android.graphics.Bitmap;
import android.view.ViewGroup;
<xss:marker id="imports"/>

public class XKPUtils {
	private Activity 				mActivity;
	private ArrayList<ViewGroup>	mViewGroup;
	private ArrayList<XKPPackage> 	mPackages;
	<xss:marker id="declarations"/>
	
	public XKPUtils() {
		mViewGroup = new ArrayList<ViewGroup>();
		mPackages = new ArrayList<XKPPackage>();
		<xss:marker id="initializers"/>
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
	
	<xss:marker id="methods"/>
}
