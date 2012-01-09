
on render_initialization(clazz, bns)
{
	out(marker = "imports")
	{
        import java.util.Timer;
        import java.util.TimerTask;
        import <xss:e value="bns"/>.libs.Sequence.*;
	}
}

on render_types()
{
	compiler.log("Rendering Sequence Types...");

    for(var ut in user_types)
    {
        var full_path = compiler.full_path("sequence.xss");
		compiler.xss("../common-js/class.xss", ut, renderer = full_path);
    }
}

on render_instances()
{
	compiler.log("Rendering Sequence...");
	
	//and then instances
    for(var i in instances)
    {
		compiler.xss("sequence.xss", i, is_class = false);
    }
}

on render_update()
{
	out()
	{
		Manager.getInstance().update(delta);
	}
}

on copy_default_files(app, bns, plibs)
{
	array<string> files = [
		"Handler.java",
		"Caller.java",
		"Assign.java",
		"Every.java",
		"IExecutable.java",
		"Interpolator.java",
		"IntInterpolator.java",
		"IInterpolator.java",
		"DoubleInterpolator.java",
		"Key.java",
		"Manager.java",
		"OnUpdate.java",
		"Sequence.java",
        "EventHolder.java",
        "SequenceUpdateEvent.java"
	];
	
	string ns = bns + ".libs.Sequence";
	for(string f in files)
	{
		string srcf = compiler.full_path("/libs/" + f);
		string dstf = plibs + "Sequence/" + f;
		
		compiler.log("Rendering default file: " + srcf);
		
		compiler.xss(srcf, output_file = dstf, ns = ns);
	}
}

on render_idiom_scripts()
{
	out(indent = 0)
	{
		private double update_freq = 1/30.0;

		private void Update() <xss:open_brace/>
			runOnUiThread(new Runnable() <xss:open_brace/>
				public void run() <xss:open_brace/>
					double delta = update_freq;
	}
	
						compiler.inject("render_update");
	
	out(indent = 0)
	{
				<xss:close_brace/>
			<xss:close_brace/>);
		<xss:close_brace/>;
	}
	
	out(indent = 0)
	{
		private void Start() <xss:open_brace/>
			Timer timer = new Timer();
			timer.schedule(new TimerTask() <xss:open_brace/>
				@Override
				public void run() <xss:open_brace/>
					Update();
				<xss:close_brace/>
				
			<xss:close_brace/>, 0, (long)(update_freq * 1000)); 
		<xss:close_brace/>
	}
	
	out(indent = 0, marker = "callers")
	{
		Start();
	}
}
