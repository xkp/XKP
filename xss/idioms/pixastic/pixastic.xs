on pre_process(obj)
{			
	if(obj.id == '')
		obj.id = compiler.genid(obj.class_name);
}

on compile_dependency(dep)
{
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

