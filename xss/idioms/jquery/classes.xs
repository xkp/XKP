
on pre_process(obj)
{
	if(obj.id == "")
		obj.id = compiler.genid(obj.class_name);
}

on render_instances()
{
	compiler.log("Rendering jQuery...");
    
	var rd_instances = instances;
	rd_instances += application;
	
	//first one, generate all instances declarations
	for(var ri in rd_instances)
	{
		string jq_object = '$("#' + ri.id + '")';
		
		string jq_classname = "";
		if(ri.jq_classname && ri.jq_classname != "")
			jq_classname = "." + ri.jq_classname + "()";
		
		out()
		{
			var <xss:e value="ri.id"/> = <xss:e value="jq_object"/><xss:e value="jq_classname"/>;
		}
	}
	
	//and then instances
    for(var i in rd_instances)
    {
		compiler.log("Rendering instance with id = " + i.id);
        compiler.xss("instance.xss", it = i, is_class = false);
		//compiler.xss("../common-js/instance.xss", i, event_renderer = "../jquery/event.xss");
    }
}
