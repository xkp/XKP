
on render_initialization()
{
	out()
	{
		var g_streamer = new ms.streamer.Streamer();
	}
}

on render_resources()
{
	compiler.log("Rendering Resources...");

    out()
    {
        var def_package_items = [];
    }

    for(var res in instances)
    {			
        if (res.class_name == "package")
        {
            string pack_id = compiler.genid("__p");
            out()
            {
                var <xss:e v="pack_id"/> = 
                [
            }
                
            for(var resource in res.children)
            {
                render_resource(resource); compiler.out(",");
            }
                
            out()
            {
                ];
                    
                <xss:e v="res.id"/> = new ms.streamer.Package(streamer, <xss:e v="pack_id"/>);
				<xss:e v="res.id"/>.load();	
                       
            }
                
            if (res.auto_load)
            {
                out()
                {
                    <xss:e v="res.id"/>.load();
                }
            }
        }
        else
        {
            out()
            {
                def_package_items.push(
            }
            render_resource(res);
            compiler.out(");");
        }
		
    }

    out()
    {
        var g_startup_package = new ms.streamer.Package(g_streamer, def_package_items);	
		g_startup_package.load();	
    }
}

method render_resource(resource)
{
	var type = compiler.get_type(resource.class_name); 
    if (!type)
        compiler.error("Unknown resource type", type = resource.class_name);

	compiler.out("{");
	compiler.xss("renderer.xss", resource);
	compiler.out("}");	    
}
