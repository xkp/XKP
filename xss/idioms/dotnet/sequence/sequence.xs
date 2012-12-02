
on declare_instances()
{
    for(var it in instances)
    {
        var type = it.type;
        out()
        {
            private <xss:e>compiler.type_to_string(type)</xss:e> <xss:e v="it.output_id"/>;
        }
    }
}

on create_instances()
{
    for(var it in instances)
    {
        out()
        {
            <xss:e v="it.output_id"/> = <xss:e v="compiler.instantiate(it)"/>;
        }
    }
}

on init_instances()
{
    for(var it in instances)
    {
        compiler.xss("idioms/sequence/sequence.xss", it);
    }
}

//sequence.xss handlers
method begin_interpolator(prop, string iid, string assign, string path)
{
    string prop_type = compiler.type_to_string(prop.type);
    out()
    {
        Interpolator<<xss:e v="prop_type"/>> <xss:e v="iid"/> = new Interpolator<<xss:e v="prop_type"/>>();
        <xss:e v="iid"/>.onUpdate += delegate(<xss:e v="prop_type"/> value)
        {
            <xss:e v="assign"/>;
        };
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

method begin_caller(methd, mid, time)
{
    out()
    {
        Caller <xss:e v="mid"/> = new Caller(<xss:e v="time"/>);
        xss:e v="mid"/>.onUpdate += delagate ()
        <xss:open_brace/>
    }
}

method close_caller(seq_id, mid)
{
    out()
    {
        <xss:close_brace/>;
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
        var <xss:e v="aid"/> = new Every(<xss:e v="time"/>);
        <xss:e v="aid"/>.onUpdate += delegate(int t)
        <xss:open_brace/>
    }
}

method close_every(aid, seq_id)
{
    out()
    {
        <xss:close_brace/>;
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
        <xss:e v="seq_id"/>.onStart += delegate()
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
        <xss:close_brace/>;
    }
}    

method begin_frame_conditions(seq_id)
{
    out()
    {
        <xss:e v="seq_id"/>.onUpdate += delegate(int elapsed, int last)
        <xss:open_brace/>
    }
}

method close_frame_conditions()
{
    out()
    {
        <xss:close_brace/>;
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

method run_expression(expr_info, seq, seq_id)
{
    compiler.render_expression(expr_info.expr);
    compiler.out(";");
}
