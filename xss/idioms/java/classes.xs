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
	compiler.log("Rendering Java instances...");
	
	for(var i in instances)
	{
		compiler.xss("instance.xss", i);
	}
}
