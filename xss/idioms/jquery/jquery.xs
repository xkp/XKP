
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

on render_instances()
{
	var rd_instances = instances;
	rd_instances += application;
	
	//first one, generate all instances declarations
	for(var ri in rd_instances)
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
    for(var i in rd_instances)
    {
        compiler.xss("instance.xss", it = i, is_class = false);

        if (i.js_renderer)
        {
            string fname = "class.xss/" + i.js_renderer;
            compiler.xss(fname, it = i);
        }
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
