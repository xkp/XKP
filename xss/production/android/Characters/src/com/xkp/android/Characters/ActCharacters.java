package com.xkp.android.Characters;
import java.util.ArrayList;
import java.util.Arrays;
import android.app.Activity;
import android.os.Bundle;
import android.view.View;
public class ActCharacters 
				extends Activity
{
				private character aladdin;
				private character pluto;
				private character mickey;
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        bindViews();
    }
    private void bindViews() {
				aladdin = (character) findViewById(R.id.aladdin);
				pluto = (character) findViewById(R.id.pluto);
				mickey = (character) findViewById(R.id.mickey);
				}
}
