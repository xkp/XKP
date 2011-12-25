on render_instances(app)
{

}

on render_types(app, bns)
{
	compiler.log("### Begin Rendering Android Types...");

	//compiler.log(this.id);
	//compiler.log(this.user_types.size);
    for(var ut in user_types)
    {
		//TODO: think very well how to proceed with this
		// set idiom to custom clazz with instances
		ut.idiom = this;
		//compiler.log("type: " + ut.id);
		for(var it in ut.instances)
		{
			//compiler.log("instance: " + it.id);
			it.idiom = this;
		}
		for(var ch in ut.children)
		{
			//compiler.log("child: " + ch.id);
			ch.idiom = this;
		}
		
		string output_filename = app + "/src/xkp/android/" + app + "/" + ut.output_id + ".java";
		compiler.xss("script.java.xss", output_file = output_filename, clazz = ut, appName = app, base_name_space = bns, is_type = true);
	
		output_filename = app + "/res/layout/" + ut.output_id + ".xml";
		compiler.xss("layout.xml.xss", output_file = output_filename, clazz = ut, appName = app, base_name_space = bns);
    }
	
	compiler.log("### End Rendering Android Types...");
}

on render_type_initialization(clazz)
{
	render_imports(clazz);
}

on render_initialization(clazz)
{
	render_imports(clazz);
}

method render_imports(clazz)
{
	compiler.log("### Begin Rendering Android Imports...");

	array<string> imports    = [];
	
	for(var inst in clazz.instances)
	{
		//TRACE: log
		//compiler.log("View: " + inst.id);
		if(inst.no_script || inst.no_render)
			continue;
		
		//find necessary imports without duplicates
		if(inst.imports && !inst.xkpview)
		{
			//TIPS: live is hard, and very long; :)
			//TODO: it's necessary to implement vector, stack, queue and set containers in vm
			for(var i in inst.imports)
			{
				//TRACE: log
				//compiler.log("Import: " + i.import);
				bool found1 = false;
				for(var imp in imports)
				{
					if(imp == i.import)
					{
						found1 = true;
						break;
					}
				}
				
				if(!found1)
				{
					compiler.log("Adding import " + i.import + " on " + clazz.id);
					imports += i.import;
					
					out(indent = 0, marker = "imports")
					{
						import <xss:e value="i.import"/>;
					}
				}
			}
		}
	}
	
	compiler.log("### End Rendering Android Imports...");
}

on copy_default_files(app, bns, plibs)
{
	array<string> files = [
		"/res/values/attrs.xml",
		"/res/drawable/icon.png",
		"/res/drawable-ldpi/icon.png",
		"/res/drawable-mdpi/icon.png",
		"/res/drawable-hdpi/icon.png",
		"/assets/.empty",
		"/bin/.empty",
		"/gen/.empty"
	];
	
	for(string f in files)
	{
		string srcf = compiler.full_path("/copy.defaults" + f);
		string dstf = app + f;
		
		compiler.log("Copying default file: " + srcf);
		
		compiler.copy_file(src = srcf, dst = dstf);
	}
}

on render_idiom_scripts()
{

}
