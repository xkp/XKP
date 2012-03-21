
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
    for(var ut in user_types)
    {
        var full_path = compiler.full_path("sequence.xss");
		compiler.xss("../common-js/class.xss", ut, renderer = full_path);
    }
}

on render_instances()
{
    for(var i in instances)
    {
		compiler.xss("../sequence.xss", i, is_class = false);
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
	
	compiler.log("Copying files...");
	
	string ns = bns + ".libs.Sequence";
	for(string f in files)
	{
		string srcf = compiler.full_path("/libs/" + f);
		string dstf = plibs + "Sequence/" + f;
		
		compiler.xss(srcf, output_file = dstf, ns = ns);
	}
}

on render_idiom_scripts(main)
{
	if(main == 1)
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
}

//delegates
method begin_interpolator(string iid, string assign)
{
    out()
    {
        final Interpolator <xss:e v="iid"/> = new Interpolator();
	    <xss:e v="iid"/>.setAssigner(new Assign(){
	        public void put(Object value)
	        {
			    <xss:e v="compiler.property_set(prop, path, 'value')"/>;
	        }
	    });
	    <xss:e v="iid"/>.setInterpolator(Manager.getInstance().interpolator("<xss:e v="prop_type"/>"));	
    }
}

method render_key(key)
{
    out()
    {
        <xss:e v="iid"/>.addKey(<xss:e v="key.t"/>, <xss:e v="key.value"/>);
    }
}

method close_interpolator(iid, seq_id)
{
    out()
    {
        <xss:e v="seq_id"/>.addHandler(<xss:e v="iid"/>);	              
    }
}

method begin_caller(mid, time)
{
    out()
    {
        var <xss:e v="mid"/> = new ms.state.Caller(<xss:e v="time"/>,function()
        <xss:open_brace/>
    }
}

method close_caller(seq_id, mid)
{
    out()
    {
        <xss:close_brace/>);
        <xss:e v="seq_id"/>.addHandler(<xss:e v="mid"/>);						  
    }
}

method begin_when_event(ev, seq_id)
{
    out()
    {
        if (<xss:e v="ev.path"/>.events)
        <xss:open_brace/>
            <xss:e v="ev.path"/>.events.addListener("<xss:e v="ev.property_name"/>", function()
            <xss:open_brace/>
                if (<xss:e v="seq_id"/>.running)
                <xss:open_brace/>
    }                        
}

method close_when_event()
{
    out()
    {
                <xss:close_brace/>
            <xss:close_brace/>);
        <xss:close_brace/>
    }
}

method begin_every(aid, time)
{
    out()
    {
        var <xss:e v="aid"/> = new ms.state.Every(<xss:e v="time"/>, function(t)
        <xss:open_brace/>
    }
}

method close_every(aid, seq_id)
{
    out()
    {
        <xss:close_brace/>);
        <xss:e v="seq_id"/>.addHandler(<xss:e v="aid"/>);	                    
    }
}

method register_nested_sequence(seq_id, nested_id, shid)
{
    out()
    {
        var <xss:e v="shid"/> = new ms.state.SequenceHandler(<xss:e v="nested_id"/>);
        <xss:e v="seq_id"/>.addHandler(<xss:e v="shid"/>);						  
    }
}

method render_instance(seq, seq_id, parent_id, path)
{
    //declare variable on the top level if no path is supplied, could be confusing
    if (!path && !parent_id)
        out(marker = "variables", marker_source = "entry") {var <xss:e v="seq_id"/>;}

    //initialize it here
    out() {<xss:e v="seq_id"/> =} compiler.xss("../../common-js/instantiator.xss", seq);
            
    if (parent_id)
        compiler.xss("../../common-js/instance.xss", seq, path = parent_id);
    else
        compiler.xss("../../common-js/instance.xss", seq, path = path);
}
        
method notify_parented(seq_id)
{
    out() 
    { 
        <xss:e v="seq_id"/>.parent_sequence = true; 
    }
}

method begin_key_expressions(seq_id)
{
    out()
    {
        <xss:e v="seq_id"/>.events.addListener('start', function()
        <xss:open_brace/>
    }
}

mehod render_key_expression(key_xpr)
{
    out()
    {
        <xss:e v="key_xpr.interp"/>.keys[<xss:e v="key_xpr.idx"/>].value = <xss:e v="key_xpr.expr"/>;
    }
}

method close_key_expressions()
{
    out()
    {
        <xss:close_brace/>);
    }
}    

method begin_frame_conditions(seq_id)
{
    out()
    {
        <xss:e v="seq_id"/>.events.addListener('update',function(elapsed, last)
        <xss:open_brace/>
    }
}

method close_frame_conditions()
{
    out()
    {
        <xss:close_brace/>);
    }
}
