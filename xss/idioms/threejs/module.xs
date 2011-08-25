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
		compiler.xss("../common-js/instance.xss", i);
    }
}
