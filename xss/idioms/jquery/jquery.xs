
on pre_process(obj)
{
	//assign name if it dont have, must optimize
    if(obj.id == '')
		obj.id = compiler.genid(obj.class_name);

    //isolate css for easier handling
    var css = object();
    for(var prop in obj.properties)
    {
        if (prop.value == null)
            continue;

        if (prop.css)
        {
            compiler.add_object_property(css, prop.output_id, value = prop.value, postfix = prop.postfix, as_plain_text=prop.as_plain_text);
        }
            
    }

    obj.css = css;
}

on compile_dependency(dep)
{
    if (!dep.idiom)
        return;

    if (dep.idiom.id != "jquery")
        return;

    var jquery_path = project.jquery_path;
    if (!jquery_path)
        jquery_path = "../js";

    var css_path = project.css_path;
    if (!css_path)
        css_path = "../css";

    if (dep.stylesheet)
        dep.href = css_path + '/' + dep.href;
    else
        dep.href = jquery_path + '/' + dep.href;
}

on render_instances()
{
    out()
    {
        var application = {};
    }

	//first one, generate all instances declarations
	for(var ri in instances)
	{
        if (ri.id == "")
            continue;

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

    //finally the application
    compiler.xss("../common-js/instance.xss", application, event_renderer = "../jquery/app-event.xss");
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
