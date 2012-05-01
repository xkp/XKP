on pre_process(obj)
{			
	if(obj.id == '')
		obj.id = compiler.genid(obj.class_name);
}

on render_js_includes()
{
	out()
	{
		<script type="text/javascript" src="../js/pixastic/pixastic.core.js"></script>
		<script type="text/javascript" src="../js/pixastic/actions.js"></script>	
		<script type="text/javascript" src="../js/pixastic/pixastic-manager.js"></script>	
		<script type="text/javascript" src="../js/pixastic/pixastic-utils.js"></script>
	}
}

on render_dependencies()
{
    var dependencies = compiler.idiom_dependencies("pixastic");
    for(var dep in dependencies)
    {   
        compiler.xss("../common-js/dependency.xss", dep);
    }
}

on render_instances()
{
	for(var inst in instances)
    {
        compiler.log("Rendering Pix instances");
		compiler.xss("pixastic.xss", inst);
    }
}

