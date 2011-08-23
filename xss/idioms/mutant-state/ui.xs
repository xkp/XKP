
on render_initialization_unused_for_now()
{
	out()
	{
		var g_ui_manager = new ms.ui.Manager();
	}
}

on render_types()
{
	compiler.log("Rendering UI Types...");

    for(var ut in user_types)
    {
        var full_path = compiler.full_path("component.xss");
		compiler.xss("../common-js/class.xss", ut, renderer = full_path);
    }
}

on render_instances()
{
	compiler.log("Rendering UI...");
	
    for(var i in instances)
    {
		compiler.xss("component.xss", i, false);
    }
}