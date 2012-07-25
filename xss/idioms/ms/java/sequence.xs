on pre_process(obj)
{
	if(obj.id == "")
		obj.id = compiler.genid(obj.class_name);
		//obj.output_id = compiler.genid(obj.class_name);
	
	//compiler.log("[android]: " + obj.id + " - " + obj.output_id);

	// flatting properties
	for(var p in obj.properties)
	{
		if(p.parent)
			if(p.parent.type)
				continue;
		
		if(p.user_defined)
			p.output_id = "prop_" + obj.output_id + "_" + p.name;
	}
	
	// flatting methods
	for(var m in obj.methods)
		if(m.user_defined)
			m.output_id = "mthd_" + obj.output_id + "_" + m.name;
}

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
        var full_path = compiler.full_path("../sequence.xss");
		compiler.xss("../../java/class.xss", ut, renderer = full_path);
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

on copy_default_files(bns, plibs)
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
method begin_interpolator(object prop, string iid, string assign, string path)
{
	var prop_type = prop.type.output_id;
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

method render_key(iid, key)
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

method begin_caller(mthd, mid, time)
{
    out()
    {
        Caller <xss:e v="mid"/> = new Caller(<xss:e v="time"/>);
		<xss:e v="mid"/>.setCaller(new Runnable() 
        {
            public void run() 
            {
                <xss:e v="mthd.output_id"/>();
            }
        });
        <xss:e v="mid_caller"/>.addHandler(<xss:e v="mid"/>);
    }
}

method close_caller(seq_id, mid)
{
    out()
    {
        <xss:e v="seq_id"/>.addHandler(<xss:e v="mid"/>);
    }
}

method begin_when_event(ev, seq_id)
{
    compiler.error("When events are not yet implemented in java");
    out()
    {
        <xss:e v="seq_id"/>.events.addListener("<xss:e v="ev.property_name"/>",  new EventHolder.Implementor() 
        <xss:open_brace/>
	        public void call(Object ev)
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
			<xss:close_brace/>
		<xss:close_brace/>);
    }
}

method begin_every(aid, time)
{
    out()
    {
        Every <xss:e v="aid"/> = new Every(<xss:e v="time"/>);
    }
}

method close_every(aid, seq_id)
{
    out()
    {
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
	string sClassName = "";
	if(seq.type)
		sClassName = seq.type.output_id;
	
	out(indent = 1, marker = "declarations", marker_source = "previous")
	{
		private <xss:e value="sClassName"/> <xss:e value="seq.output_id"/> = <xss:e value="compiler.instantiate(seq)"/>;
	}

    compiler.xss("../../java/instance.xss", seq, marker = "declarations", marker_source="previous", render_events = false);
    compiler.xss("../../java/instance.xss", seq, event_renderer = "event.xss", render_properties = false, render_methods = false);
}
        
method notify_parented(seq_id, parent_id)
{
    out() 
    { 
        <xss:e v="seq_id"/>.setParent(<xss:e v="parent_id"/>); 
    }
}

method begin_key_expressions(seq_id)
{
    out()
    {
        <xss:e v="seq_id"/>.events.addListener("start",  new EventHolder.Implementor() 
        <xss:open_brace/>
	        public void call(Object ev) 
            <xss:open_brace/>
    }
}

mehod render_key_expression(key_xpr)
{
    out()
    {
        <xss:e v="key_xpr.interp"/>.setKeyValue(<xss:e v="key_xpr.idx"/>, <xss:e v="key_xpr.expr"/>);
    }
}

method close_key_expressions()
{
    out()
    {
            <xss:close_brace/>
        <xss:close_brace/>);
    }
}    

method begin_frame_conditions(seq_id)
{
    out()
    {
        <xss:e v="seq_id"/>.events.addListener("update",  new EventHolder.Implementor() 
        <xss:open_brace/>
	        public void call(Object ev) 
            <xss:open_brace/>
    }
}

method close_frame_conditions()
{
    out()
    {
            <xss:close_brace/>
        <xss:close_brace/>);
    }
}

method run_method(expr, path)
{
    var mthd = expr.methd;
    out() 
    {
        <xss:e v="mthd.output_id"/>();
    }
}    

method run_sequence(seq, parent_id)
{
    if (seq.this_property)
    {
		out() 
        {
            <xss:e v="parent_id"/>.<xss:e v="seq.identifier"/>.start();
        }
    }
    else
    {
		out() 
        {
            <xss:e v="seq.identifier"/>.start();
        }
    }
}    

method run_expression(expr, seq, seq_id)
{
    var expr_this = seq;
    var this_str = seq_id;
    if (seq.target)
    {
        expr_this = compiler.get_instance(seq.target);
        this_str += ".target";
    }
    else if (seq.target_type)
    {
        expr_this = compiler.get_type(seq.target_type);
        this_str += ".target";
    }
                
    string result = compiler.render_expression(expr, expr_this);
    result = compiler.replace_identifier(result, "this.", this_str + ".");
            
    out() 
    {
        <xss:e v="result"/>;
    }
}
