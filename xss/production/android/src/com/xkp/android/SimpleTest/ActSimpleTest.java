package com.xkp.android.SimpleTest;

import android.app.Activity;
import android.os.Bundle;
	import android.widget.TextView;
	import android.widget.Button;
	import android.widget.EditText;

public class ActSimpleTest extends Activity
{
    	private TextView privlblFullName;
    	private Button privbtnFullName;
    	private EditText privedtLastName;
    	private TextView privlblLastName;
    	private EditText privedtFirstName;
    	private TextView privlblFirstName;

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
	
        	privlblFullName = (TextView) findViewById(R.id.lblFullName);
        	privbtnFullName = (Button) findViewById(R.id.btnFullName);
        	privedtLastName = (EditText) findViewById(R.id.edtLastName);
        	privlblLastName = (TextView) findViewById(R.id.lblLastName);
        	privedtFirstName = (EditText) findViewById(R.id.edtFirstName);
        	privlblFirstName = (TextView) findViewById(R.id.lblFirstName);
    }
}

