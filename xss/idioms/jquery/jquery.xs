
on pre_process(obj)
{
	if(obj.id == "")
		obj.id = compiler.genid(obj.class_name);

    //isolate css for easier handling
    var css = object();
    for(var prop in obj.properties)
    {
        if (prop.value == null)
            continue;

        if (prop.css)
        {
            compiler.add_object_property(css, prop.output_id, value = prop.value, postfix = prop.postfix);
        }
            
    }

    obj.css = css;
}

on render_dependencies()
{
    var dependencies = compiler.idiom_dependencies("jquery");

    var jquery_path = project.jquery_path;
    if (!jquery_path)
        jquery_path = "../js";

    var css_path = project.css_path;
    if (!css_path)
        css_path = "../css/ui-lightness";

    for(var dep in dependencies)
    {
        if (dep.stylesheet)
            compiler.xss("../common-js/dependency.xss", dep, path = css_path);
        else
            compiler.xss("../common-js/dependency.xss", dep, path = jquery_path);
    }
}

on render_instances()
{
    out()
    {
        var application = {};
    }

    compiler.xss("../common-js/instance.xss", application, event_renderer = "../jquery/app-event.xss");
    	
	//first one, generate all instances declarations
	for(var ri in instances)
	{
		string jq_object = '$("#' + ri.id + '")';
		
		string jq_classname = "";
		if(ri.jquery && ri.jquery != "")
        {
			jq_classname = "." + ri.jquery + "()";
        }
		
		out()
		{
			var <xss:e value="ri.id"/> = <xss:e value="jq_object"/><xss:e value="jq_classname"/>;
		}
	}
	
	//and then instances
    for(var i in instances)
    {
        compiler.xss("instance.xss", it = i, is_class = false);

        if (i.js_renderer)
        {
            string fname = "class.xss/" + i.js_renderer;
            compiler.xss(fname, it = i);
        }
    }

    out()
    {
        if (application.init)
            application.init();
    }
}

on render_css()
{
    for(var i in instances)
    {
        var css = i.css;
        if (!css)
            continue;

        if (css.empty())
            continue;
        
        compiler.xss("../common-js/css.xss", i, marker = "css");
    }
}
