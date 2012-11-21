
on pre_process(obj)
{
	// LOG:
	//compiler.log("Object id: " + obj.id);
	if(obj.id == "")
		obj.output_id = compiler.genid(obj.class_name);

	// flatting properties
	// LOG:
	//compiler.log("Flatting java properties...");
	for(var p in obj.properties)
	{
		if(p.parent)
			if(p.parent.type)
				continue;
		
		if(p.user_defined)
			p.output_id = "prop_" + obj.output_id + "_" + p.name;
	}
	
	// flatting methods
	// LOG:
	//compiler.log("Flatting java methods...");
	for(var m in obj.methods)
		if(m.user_defined)
			m.output_id = "mthd_" + obj.output_id + "_" + m.name;
}

on render_instances()
{
	/*
	compiler.log("Rendering Java instances...");
	
	instances += application;
	for(var i in instances)
	{
		compiler.xss("instance.xss", i);
	}
	*/
}

on render_types()
{
	//TRACE: log
	compiler.log("Rendering Java Types...");
	
    for(var ut in user_types)
    {
		//TODO: think very well how to proceed with this
		// set idiom to custom clazz with instances
		//ut.idiom = this;
		//compiler.log("type: " + ut.id);
		for(var it in ut.instances)
		{
			//compiler.log("instance: " + it.id);
			it.idiom = compiler.idiom_by_class(it.type.id);
			if(it.utils) idiom.need_utils = true;
		}
		for(var ch in ut.children)
		{
			//compiler.log("child: " + ch.id);
			ch.idiom = compiler.idiom_by_class(ch.type.id);
			if(ch.utils) idiom.need_utils = true;
		}
		
		string output_filename = ut.output_id + ".java";
		compiler.xss("class.xss", output_file = output_filename, clazz = ut);
    }
}

