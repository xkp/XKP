on pre_process(obj)
{
    if (obj.class_name == "quake_camera" || obj.class_name == "camera")
    	obj.id = "camera";
	else if (obj.class_name == "scene")
		obj.id = "scene";
}

on render_instances()
{	
	for(var i in instances)
    {
		compiler.xss("inst_renderer.xss", i);
		compiler.xss("../common-ja/instance.xss", i);
    }
}

on render_types()
{
	compiler.log("Rendering 3js Types...");

    for(var ut in user_types)
    {
        var full_path = compiler.full_path("sequence.xss");
		compiler.xss("../common-ja/class.xss", ut, renderer = full_path);
    }
}
