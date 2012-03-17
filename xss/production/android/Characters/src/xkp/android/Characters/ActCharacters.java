package xkp.android.Characters;

import java.util.ArrayList;
import java.util.Arrays;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.app.Activity;
import android.graphics.Bitmap;
import android.os.Bundle;
import xkp.android.libs.Layout.XKPLayout;
import xkp.android.libs.Widget.XKPPackage;

public class ActCharacters 
		extends Activity
{
	private character aladdin;
	private character mickey;
	private character pluto;
	private XKPPackage __resources1;
	
	private String [] mResources___resources1_XKPName = {
		"i_aladdin",
		"i_mickey",
		"i_pluto",
		"a_audio1",
		"a_audio2",
		"v_mickey",
		"v_pluto"
	};
	private String [] mResources___resources1_DroidName = {
		"aladdin.png",
		"mickey.png",
		"pluto.png",
		"audio1.ogg",
		"audio2.ogg",
		"mickey.3gp",
		"pluto.3gp"
	};
	private Integer [] mResources___resources1_Type = {
		XKPPackage.BITMAP_RESOURCE,
		XKPPackage.BITMAP_RESOURCE,
		XKPPackage.BITMAP_RESOURCE,
		XKPPackage.AUDIO_RESOURCE,
		XKPPackage.AUDIO_RESOURCE,
		XKPPackage.VIDEO_RESOURCE,
		XKPPackage.VIDEO_RESOURCE
	};
	private Integer [] mResources___resources1_Id = {
		R.drawable.aladdin,
		R.drawable.mickey,
		R.drawable.pluto,
		R.raw.audio1,
		R.raw.audio2,
		R.raw.mickey,
		R.raw.pluto
	};
	
	ActCharacters application;
	public static XKPUtils util;
			
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		bindViews();
		initInstances();
        initCallers();
		setContentView(R.layout.main);
	}
	private void bindViews() {
		aladdin = (character) findViewById(R.id.aladdin);
		mickey = (character) findViewById(R.id.mickey);
		pluto = (character) findViewById(R.id.pluto);
	}
	
	void initCallers() {
	}
	
	void initInstances() {
		 util = new XKPUtils(this);
		 
		 application = this;
		__resources1 = new XKPPackage(this, 
			mResources___resources1_XKPName, mResources___resources1_DroidName, 
			mResources___resources1_Type, mResources___resources1_Id, true);
		util.addXKPPackage(__resources1);
		__resources1.load();
	}
	
	public class XKPUtils {
		private Activity 				mActivity;
		private ArrayList<ViewGroup>	mViewGroup;
		private ArrayList<XKPPackage> 	mPackages;
		
		public XKPUtils(Activity act) {
			mActivity = act;
			mViewGroup = new ArrayList<ViewGroup>();
			mPackages = new ArrayList<XKPPackage>();
		}
		
		public void addViewGroup(ViewGroup viewgrp) {
			mViewGroup.add(viewgrp);
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
	}
}
