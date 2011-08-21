
on render_initialization()
{
	out()
	{
		var g_sequence_manager = new ms.state.Manager();
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