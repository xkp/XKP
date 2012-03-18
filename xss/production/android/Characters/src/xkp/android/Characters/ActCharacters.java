package xkp.android.Characters;
import java.util.ArrayList;
import java.util.Arrays;
import android.view.View;
			import android.app.Activity;
			import android.os.Bundle;
import xkp.android.libs.Layout.XKPLayout;
			import xkp.android.libs.Widget.XKPUtils;
						import xkp.android.libs.Widget.XKPPackage;
		public class ActCharacters 
						extends Activity
{
				private character aladdin;
				private character mickey;
				private character pluto;
			public static XKPUtils util;
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
			@Override
			public void onCreate(Bundle savedInstanceState) {
				super.onCreate(savedInstanceState);
                initCallers();
				initInstances();
				setContentView(R.layout.main);
				bindViews();
			}
			private void bindViews() {
				aladdin = (character) findViewById(R.id.aladdin);
				mickey = (character) findViewById(R.id.mickey);
				pluto = (character) findViewById(R.id.pluto);
	}
void initCallers() {
			util = new XKPUtils();
			ActCharacters.util.addView(this);
			util = ActCharacters.util;
}
			void initInstances() {
				application = this;
				__resources1 = new XKPPackage(this, 
					mResources___resources1_XKPName, mResources___resources1_DroidName, 
					mResources___resources1_Type, mResources___resources1_Id, true);
				util.addXKPPackage(__resources1);
					__resources1.load();
			}
}
