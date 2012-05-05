on pre_process(obj)
{			
	if(obj.id == '')
		obj.id = compiler.genid(obj.class_name);
}

on compile_dependency(dep)
{
    if (dep.shared)
    {
        var js_path = project.js_path;
        if (!js_path)
            js_path = "../js";

        dep.href = js_path + '/' + dep.href;
        return;
    }
        
    if (!dep.idiom)
        return;

    if (dep.idiom.id != "pixastic")
        return;

    var path = project.pixastic_path;
    if (!path)
        path = "../js/pixastic";

    dep.href = path + '/' + dep.href;
}

on render_instances()
{
	for(var inst in instances)
    {
		compiler.xss("pixastic.xss", inst);
    }
}

