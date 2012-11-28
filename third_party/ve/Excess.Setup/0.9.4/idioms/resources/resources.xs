on pre_process(obj)
{	
	if(obj.id == '')		
		obj.id = compiler.genid(obj.class_name);
	if(obj.src)
	{		
		obj.src = "../resources/" + obj.src;		
	}	
}

on compile_dependency(dep)
{
    compiler.log("on resources.compile_dependency");
    if (!dep.idiom)
        return;

    if (dep.idiom.id != "resources")
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
		var streamer = new stream.Streamer();		
		var ResourceUtils = new stream.ResourceUtils();
	}	
}

on render_resources()
{
	compiler.log("Rendering Resources...");

    out() 
	{        
		var global_package = new stream.Package(streamer);	
    }

    for(var res in instances)
    {
		if(res.class_name == "package")
		{
			out()
			{
				var <xss:e v="res.id"/> = new stream.Package(streamer);				
			}
			for(var ev in res.events)
			{
				compiler.xss("../common-js/event.xss", ev, res, is_class = false, path = path);
			}
		}
		else if(res.renderer)
		{
			compiler.xss(res.renderer, res);
		}
		else if (res.parent.class_name == "package")
        {
            out()
            {
                <xss:e v="res.parent.id"/>.add_item(
            }
            render_resource(res);
            compiler.out(");");
			out()
			{
				var <xss:e v="res.id"/>;
				<xss:e v="res.parent.id"/>.events.addListener("loaded", function()
				{
					<xss:e v="res.id"/> = streamer.get_resource("<xss:e v="res.id"/>");
				});
			}
        }
        else
        {
            out()
            {
                global_package.add_item(
            }
            render_resource(res);
            compiler.out(");");
			out()
			{	
				var <xss:e v="res.id"/>;
				global_package.events.addListener("loaded", function()
				{
					<xss:e v="res.id"/> = streamer.get_resource("<xss:e v="res.id"/>");
				});
			}
        }
		compiler.xss("children_renderer.xss", res);	
    }	
    out()
    {				
		global_package.load();	
    }
}

method render_resource(resource)
{
	var type = compiler.get_type(resource.class_name); 
    if (!type)
        compiler.error("Unknown resource type", type = resource.class_name);	
	compiler.xss("renderer.xss", resource);			
}
