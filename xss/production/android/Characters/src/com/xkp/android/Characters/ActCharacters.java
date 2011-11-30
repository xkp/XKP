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
			public String prop_aladdin_name_get() {
				return lname.getText().toString();
			}
			public void prop_aladdin_name_set(String value) {
				lname.setText(value);
			}
			public String prop_aladdin_surname_get() {
				return lsurname.getText().toString();
			}
			public void prop_aladdin_surname_set(String value) {
				lsurname.setText(value);
			}
			public String prop_aladdin_employment_get() {
				return lemployment.getText().toString();
			}
			public void prop_aladdin_employment_set(String value) {
				lemployment.setText(value);
			}
			public String prop_pluto_name_get() {
				return lname.getText().toString();
			}
			public void prop_pluto_name_set(String value) {
				lname.setText(value);
			}
			public String prop_pluto_surname_get() {
				return lsurname.getText().toString();
			}
			public void prop_pluto_surname_set(String value) {
				lsurname.setText(value);
			}
			public String prop_pluto_employment_get() {
				return lemployment.getText().toString();
			}
			public void prop_pluto_employment_set(String value) {
				lemployment.setText(value);
			}
			public String prop_mickey_name_get() {
				return lname.getText().toString();
			}
			public void prop_mickey_name_set(String value) {
				lname.setText(value);
			}
			public String prop_mickey_surname_get() {
				return lsurname.getText().toString();
			}
			public void prop_mickey_surname_set(String value) {
				lsurname.setText(value);
			}
			public String prop_mickey_employment_get() {
				return lemployment.getText().toString();
			}
			public void prop_mickey_employment_set(String value) {
				lemployment.setText(value);
			}
}
