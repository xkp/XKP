package xkp.android.Characters;
import java.util.ArrayList;
import java.util.Arrays;
import android.view.View;
import android.view.ViewTreeObserver;
import android.view.ViewTreeObserver.OnGlobalLayoutListener;
			import android.app.Activity;
			import android.os.Bundle;
import xkp.android.libs.Layout.XKPLayout;
			import xkp.android.libs.Widget.XKPUtils;
						import android.view.View.OnKeyListener;
						import android.view.KeyEvent;
						import xkp.android.libs.Widget.XKPPackage;
						import xkp.android.libs.Widget.XKPPackage.OnResourcePackageListener;
		public class ActCharacters 
						extends Activity
{
				private character aladdin;
				private character mickey;
				private character pluto;
			public static XKPUtils util;
		private XKPLayout layoutapplication;
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
	private boolean mLayoutStarted = false;
			private ActCharacters application;
			@Override
			public void onCreate(Bundle savedInstanceState) {
				super.onCreate(savedInstanceState);
                initCallers();
				initInstances();
				setContentView(R.layout.main);
				bindViews();
			}
			private void bindViews() {
		// TIPS: create an observer for notify layout updated
		layoutapplication = (XKPLayout) findViewById(R.id.layoutapplication);
		ViewTreeObserver mainLayoutViewTreeObserver = layoutapplication.getViewTreeObserver();
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
				aladdin = (character) findViewById(R.id.aladdin);
				mickey = (character) findViewById(R.id.mickey);
				pluto = (character) findViewById(R.id.pluto);
	}
private void initCallers() {
			util = new XKPUtils();
			ActCharacters.util.addView(this);
			util = ActCharacters.util;
}
			@Override
			public void onPause() {
				super.onPause();
			}
			@Override
			public void onStop() {
				super.onStop();
			}
			@Override
			public void onResume() {
				super.onResume();
			}
private void onLayoutUpdated() {
}
private void onLayoutStarted() {
}
			private void initInstances() {
				application = this;
				__resources1 = new XKPPackage(this, 
					mResources___resources1_XKPName, mResources___resources1_DroidName, 
					mResources___resources1_Type, mResources___resources1_Id
					, true);
				util.addXKPPackage(__resources1);
			}
			private void initapplication() {
			}
}
