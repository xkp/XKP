on pre_process(obj)
{
	if(obj.id == '')
		obj.id = compiler.genid(obj.class_name);
	if(obj.parent)	
		if(obj.parent.wait_for_package)
			obj.wait_for_package = obj.parent.wait_for_package;
}

on compile_dependency(dep)
{
    if (!dep.idiom)
        return;

    if (dep.idiom.id != "ui")
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
		if(drawingCanvas){
			var client =
			{
				width: <xss:e value="application.width"/>,
				height: <xss:e value="application.height"/>,
				canvas: drawingCanvas
			};            						
			var ui_ = new ui.Manager(client, streamer);			
			var g_ui = ui_;
			var g_ui_root = ui_.root;
			var mouse_pressed = false;
		}		
	}
}

on render_types()
{
	compiler.log("Rendering UI Types...");
    for(var ut in user_types)
    {		       
        var full_path = compiler.full_path("component.xss");		
		compiler.xss("../common-js/resig-class.xss", ut, renderer = full_path, context = ut);			
    }
}

on render_inst_elems()
{
	for(var inst in instances)
    {
		if(!inst.dont_render)
		{
			if(inst.wait_for_package)
			{
				out()
				{
				<xss:e v="inst.wait_for_package"/>.events.addListener("loaded", function()
				<xss:open_brace/>
				}
			}
			compiler.xss("../common-js/instance.xss", inst);
			if(inst.wait_for_package)
				out(){<xss:close_brace/>);}
		}
	}
}

on render_instances()
{
	compiler.log("Rendering UI...");
	
    for(var inst in instances)
    {					
        if(inst.wait_for_package)
		{			
			out()
			{
				<xss:e v="inst.wait_for_package"/>.events.addListener("loaded", function()
				<xss:open_brace/>				
			}
		}			
		if(inst.renderer)
			compiler.xss(inst.renderer, inst);			
		else
			compiler.xss("component.xss", inst, false);
		if(inst.wait_for_package)	
			out(){<xss:close_brace/>);}		
    }
}