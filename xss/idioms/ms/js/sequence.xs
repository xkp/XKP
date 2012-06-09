on pre_process(obj)
{
	if (obj.id == '')
        obj.id = compiler.genid(obj.class_name);

    if (obj.parent)
    {
        if (obj.class_name == 'sequence' && obj.parent.class_name == 'sequence')
        {
            obj.parent.add_property(obj.id, obj, obj.type);
        }
    }
}

on compile_dependency(dep)
{
    if (!dep.idiom)
        return;

    if (dep.idiom.id != "sequence")
        return;

    var path = project.js_path;
    if (!path)
        path = "../js";

    dep.href = path + '/' + dep.href;
}

on render_initialization()
{
	out()
	{
		var g_sequence_manager = new state.Manager();

        function default_interpolate(a, b, t)
        {
            return a + (b - a)*t;
        }		
	}    
}

on render_types()
{
	compiler.log("Rendering Sequence Types...");

    for(var ut in user_types)
    {
        var full_path = compiler.full_path("../sequence.xss");
		compiler.xss("../../common-js/resig-class.xss", ut, renderer = full_path);
    }
}

on render_instances()
{
	//and then instances
    for(var i in instances)
    {
		compiler.xss("../sequence.xss", i, is_class = false);
    }
}

on render_update()
{
	out()
	{
		g_sequence_manager.update(g_delta);
	}
}

on render_updater()
{
	out()
	{
        var g_elapsed = -1;
        var g_delta = 0.0;	
        function start(resolution)  
        <xss:open_brace/>  
	        function updater()  
	        <xss:open_brace/>
		        var now   = new Date().getTime();
                if (g_elapsed < 0)
                    g_delta = 0; //first update
                else
		            g_delta  = now - g_elapsed;  
		
                g_elapsed = now; 
    }          

    compiler.inject("render_update");

    out()
    {
                window.setTimeout(updater, resolution);  
	        <xss:close_brace/>      
	        window.setTimeout(updater, resolution);  
        <xss:close_brace/>      
        
        start();  
    }
}

//java script delegates
method begin_interpolator(prop, string iid, string assign, string path)
{
    out()
    {
        var <xss:e v="iid"/> = new state.Interpolator(
                         
            default_interpolate,
            function(value)
            {
                <xss:e v="assign"/>;
            },
            [
    }
}

method render_key(iid, key)
{
    out()
    {
        {t: <xss:e v="key.t"/>, value: <xss:e v="key.value"/>},
    }
}

method close_interpolator(iid, seq_id)
{
    out()
    {
            ]);
            <xss:e v="seq_id"/>.addHandler(<xss:e v="iid"/>);	              
    }
}

method begin_caller(methd, mid, time)
{
    out()
    {
        var <xss:e v="mid"/> = new state.Caller(<xss:e v="time"/>,function()
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
        var <xss:e v="aid"/> = new state.Every(<xss:e v="time"/>, function(t)
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
        var <xss:e v="shid"/> = new state.SequenceHandler(<xss:e v="nested_id"/>);
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

method run_method(mthd, path)
{
    out() 
    {
        <xss:e v="path"/>.<xss:e v="mthd.method_name"/>();
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
