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

on render_js_includes()
{
	out()
	{
		<script type="text/javascript" src="../js/sequence.js"></script>
	}
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
