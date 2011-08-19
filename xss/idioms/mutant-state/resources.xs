
property root : object;

on render_initialization()
{
	out()
	{
		var g_streamer = new ms.streamer.Streamer();
	}
}

on pre_process(obj, ev)
{
    root = obj;
    ev.final = true; //we will handle our children
}

on render_resources()
{
	compiler.log("Rendering Resources...");

    out()
    {
        var def_package_items = [];
    }

    for(var res in root.children)
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
                resources.register_package("<xss:e v="res.id"/>", <xss:e v="res.id"/>);            
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
        g_streamer.register_package(g_startup_package);
    }
}

method render_resource(resource)
{
    var type = compiler.get_type(resource.class_name); 
    if (!type)
        compiler.error("Unknown resource type", type = resource.class_name);

    string renderer = type.renderer;
    if (!renderer || renderer == "")
        compiler.error("Resource types must have a renderer", type = resource.class_name);

    var full_path = compiler.idiom_type_path(type, );

    compiler.out("{");
        compiler.xss(full_path, resource);
    compiler.out("}");
}
