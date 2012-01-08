package xkp.android.Characters;
import java.util.ArrayList;
import java.util.Arrays;
import android.view.View;
			import android.app.Activity;
			import android.os.Bundle;
import xkp.android.libs.Layout.XKPLayout;
		public class ActCharacters 
						extends Activity
{
				private character aladdin;
				private character mickey;
				private character pluto;
	ActCharacters application;
			@Override
			public void onCreate(Bundle savedInstanceState) {
				super.onCreate(savedInstanceState);
				setContentView(R.layout.main);
				bindViews();
				initInstances();
                initCallers();
			}
			private void bindViews() {
				aladdin = (character) findViewById(R.id.aladdin);
				mickey = (character) findViewById(R.id.mickey);
				pluto = (character) findViewById(R.id.pluto);
	}
void initCallers() {
}
			void initInstances() {
				application = this;
			}
}
