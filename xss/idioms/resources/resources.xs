on pre_process(obj)
{
	if(obj.id == '')
		obj.id = compiler.genid(obj.class_name);	
}

on render_js_includes()
{
	out()
	{		
		<script type="text/javascript" src="../js/ms-streamer.js"></script>
	}
}

on render_initialization()
{
	out()
	{
		var streamer = new ms.streamer.Streamer();		
	}	
}

on render_resources()
{
	compiler.log("Rendering Resources...");

    out()
    {
        var global_package_items = [];
    }

    for(var res in instances)
    {	
		if(res.class_name == "package")
		{
			out()
			{
				var <xss:e v="res.id"/> = new ms.streamer.Package(streamer);				
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
        }
        else
        {
            out()
            {
                global_package_items.push(
            }
            render_resource(res);
            compiler.out(");");
        }		
    }	
    out()
    {	
		global_package_items.push({
			id:				"invalid_res",
			resource_type:	RESOURCE_IMAGE,
			src:			"images/no_res.png",
			frame_width:	null,
			frame_height:	null,
			animations:		[]
		});
        var global_package = new ms.streamer.Package(streamer, global_package_items);	
		global_package.load();	
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
