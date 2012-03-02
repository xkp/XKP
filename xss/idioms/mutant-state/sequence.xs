on pre_process(obj)
{
	if(obj.id == '')
			obj.id = compiler.genid(obj.class_name);
}

on render_initialization()
{
	out()
	{
		var g_sequence_manager = new ms.state.Manager();

        function default_interpolate(a, b, t)
        {
            return a + (b - a)*t;
        }		
	}
}

on render_js_includes()
{
	out()
	{		
        <script type="text/javascript" src="../js/ms-state.js"></script>
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
		g_sequence_manager.update(g_delta);
	}
}
