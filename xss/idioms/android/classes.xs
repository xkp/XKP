on pre_process(obj)
{
	if(obj.id == "")
		obj.id = compiler.genid(obj.class_name);
		//obj.output_id = compiler.genid(obj.class_name);

	// flatting properties
	for(var p in obj.properties)
	{
		if(p.parent)
			if(p.parent.type)
				continue;
		
		if(p.user_defined)
			p.output_id = "prop_" + obj.output_id + "_" + p.name;
	}
	
	// flatting methods
	for(var m in obj.methods)
		if(m.user_defined)
			m.output_id = "mthd_" + obj.output_id + "_" + m.name;
}

on render_instances(app)
{

}

on render_types(app, bns)
{
	compiler.log("Rendering Android Types...");
	
	//TRACE: log
	//compiler.log("### Begin Rendering Android Types...");
	
	var idiom = compiler.get_idiom("android");
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
			if(it.utils) idiom.need_utils = true;
		}
		for(var ch in ut.children)
		{
			//compiler.log("child: " + ch.id);
			ch.idiom = this;
			if(ch.utils) idiom.need_utils = true;
		}
		
		string output_filename = app + "/src/xkp/android/" + app + "/" + ut.output_id + ".java";
		compiler.xss("script.java.xss", output_file = output_filename, clazz = ut, appName = app, base_namespace = bns, is_type = true);
	
		output_filename = app + "/res/layout/" + ut.output_id + ".xml";
		compiler.xss("layout.xml.xss", output_file = output_filename, clazz = ut, appName = app, base_namespace = bns);
    }
	
	//TRACE: log
	//compiler.log("### End Rendering Android Types...");
}

on render_type_initialization(clazz, bns, app)
{
	initialization(clazz, bns, app);
}

on render_initialization(clazz, bns, app)
{
	initialization(clazz, bns, app);
}

method initialization(clazz, bns, app)
{
	render_imports(clazz, bns);
	
	// render xkp widget files
	for(var inst in clazz.instances)
	{
		if(!inst.xkp_files)
			continue;
		
		for(var file in inst.xkp_files)
		{
			string src_path = file.src_path;
			string src_filename = file.src_filename;
			string dst_path = file.dst_path;
			string dst_filename = file.dst_filename;
			
			if(!dst_path) dst_path = src_path;
			if(!dst_filename) dst_filename = src_filename;
			
			string srcf = compiler.full_path("/copy.defaults" + src_path + src_filename);
			string dstf = app + dst_path + dst_filename;

			//TRACE: log
			//compiler.log("Rendering widget file: " + srcf);
			
			compiler.xss(src = srcf, output_file = dstf, appName = app, base_namespace = bns);
		}
	}
}

method render_imports(clazz, bns)
{
	compiler.log("Rendering Android Imports...");
	
	//TRACE: log
	//compiler.log("### Begin Rendering Android Imports...");

	array<string> imports    = [];
	
	for(var inst in clazz.instances)
	{
		//TRACE: log
		//compiler.log("View: " + inst.id);
		if(inst.no_script || inst.no_render)
			continue;
		
		//find necessary imports without duplicates
		if(inst.imports)
		{
			//TIPS: live is hard, and very long; :)
			//TODO: it's necessary to implement vector, stack, queue and set containers in vm
			for(var i in inst.imports)
			{
				//TRACE: log
				//compiler.log("Import: " + i.import);
				bool found1 = false;
				for(var imp1 in imports)
				{
					if(imp1 == i.import)
					{
						found1 = true;
						break;
					}
				}
				
				if(!found1)
				{
					//TRACE: log
					//compiler.log("Adding import " + i.import + " on " + clazz.id);
					imports += i.import;
					
					out(indent = 0, marker = "imports")
					{
						import <xss:e value="i.import"/>;
					}
				}
			}
		}
		else
		if(inst.sub_ns)
		{
			string import = bns + "." + inst.sub_ns + "." + inst.type.output_type;
			
			//TIPS: live is hard, and very long; :)
			//TODO: it's necessary to implement vector, stack, queue and set containers in vm
			//TODO: repeated below script... :(  reimplement this!
			bool found2 = false;
			for(var imp2 in imports)
			{
				if(imp2 == import)
				{
					found2 = true;
					break;
				}
			}
			
			if(!found2)
			{
				//TRACE: log
				//compiler.log("Adding import " + import + " on " + clazz.id);
				imports += import;
				
				out(indent = 0, marker = "imports")
				{
					import <xss:e value="import"/>;
				}
			}
		}
	}
	
	//TRACE: log
	//compiler.log("### End Rendering Android Imports...");
}

on copy_default_files(app, bns, plibs)
{
	// file list for copy
	array<string> cp_files = [
		"/res/values/colors.xml",
		"/res/values/styleable_XKPLayout.xml",
		"/res/drawable/icon.png",
		"/res/drawable-ldpi/icon.png",
		"/res/drawable-mdpi/icon.png",
		"/res/drawable-hdpi/icon.png",
		"/assets/.empty",
		"/bin/.empty",
		"/gen/.empty"
	];
	
	compiler.log("Copying default files...");
	for(string f1 in cp_files)
	{
		string srcf1 = compiler.full_path("/copy.defaults" + f1);
		string dstf1 = app + f1;

		//TRACE: log
		//compiler.log("Copying default file: " + srcf1);
		
		compiler.copy_file(src = srcf1, dst = dstf1);
	}
	
	// file list for render
	array<string> res_files = [
		"/src/xkp/android/libs/Layout/XKPLayout.java"
	];
	
	compiler.log("Rendering default files...");
	for(string f2 in res_files)
	{
		string srcf2 = compiler.full_path("/copy.defaults" + f2);
		string dstf2 = app + f2;
		
		//TRACE: log
		//compiler.log("Rendering default file: " + srcf2);
		
		compiler.xss(src = srcf2, output_file = dstf2, appName = app, base_namespace = bns);
	}
}

on render_idiom_scripts(main)
{

}
