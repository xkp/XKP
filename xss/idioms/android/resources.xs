on render_instances(app)
{
	compiler.log("Copying resource files...");
	
	//TRACE: log
	//compiler.log("Begin Rendering Resources Instances...");
	
	string pattern_numbers = "0123456789";
	string pattern_string  = "abcdefghijklmnopqrstuvwxyz";
	string pattern_others  = "._";
	string pattern_all     = pattern_numbers + pattern_string + pattern_others;

	string project_res_path = "resources/";
	
	//TODO: iterate through resources for make collection to render
	for(var inst in instances)
	{
		compiler.xss("../java/instance.xss", marker = "handlers", it = inst);
		
		//compiler.log(inst.id);
		string parent = "resources";
		if(inst.parent.id != "")
			parent = inst.parent.id;
		
		//TIP: patch??? ;)
		if(inst.res_container == true)
		{
			string sClassName = "";
			if(inst.type)
				sClassName = inst.type.output_id;
			
			out(indent = 1, marker = "declarations")
			{
				private <xss:e value="sClassName"/> <xss:e value="inst.output_id"/>;
			}
		
			string load_main_resource = "";
			if(inst.res_main_container == true)
			{
				load_main_resource = ", true";
			}
			
			out(indent = 1)
			{
				<xss:e value="inst.output_id"/> = new <xss:e value="sClassName"/>(this, 
					mResources_<xss:e value="inst.output_id"/>_XKPName, mResources_<xss:e value="inst.output_id"/>_DroidName, 
					mResources_<xss:e value="inst.output_id"/>_Type, mResources_<xss:e value="inst.output_id"/>_Id
					<xss:e value="load_main_resource"/>);
				util.addXKPPackage(<xss:e value="inst.output_id"/>);
			}
		}
		
		//render events
		for(var e in inst.events)
		{
			if(e.interface && e.implemented)
			{
				string handler = "set" + e.interface;
				if(e.set_handle)
					handler = e.set_handle;
				
				out(indent = 2)
				{
					<xss:e value="inst.output_id"/>.<xss:e value="handler"/>(new <xss:e value="e.interface"/>() <xss:open_brace/>
						@Override
						public void <xss:e value="e.output_id"/>(<xss:e value="e.def_args"/>) <xss:open_brace/>
							<xss:e>e.output_id</xss:e><xss:e value="inst.output_id"/>(<xss:e>e.args.render()</xss:e>);
						<xss:close_brace/>
					<xss:close_brace/>);
				}
			}
		}
		
		if(!inst.resrc)
			continue;
		
		//TIPS: stupid stripped string... live is hard :(
		string project_path = compiler.project_path() + "/";
		string output_path = app + "/res/" + inst.droid_location + "/";
		
		string simg = inst.src;
		if(!simg)
			continue;
		
		int sz = String.size(simg);
		
		int pos = String.find_last(simg, "\\/");
		int rpos = pos + 1;
		int npos = sz - pos - 1;
		string fn_img = String.substr(simg, rpos, npos);
		
		//check resources filename and
		int fd1 = String.find_first_not(fn_img, pattern_all);
		int fd2 = String.find_first(fn_img, pattern_numbers);

		if(fd1 != -1 || fd2 == 0)
			compiler.error("Resource filename is incorrect", filename = fn_img);
		
		//then
		//string srcf = project_path + simg;
		string srcf = project_path + project_res_path + simg;
		string dstf = output_path + fn_img;
		
		//copying resource
		//TRACE: log
		//compiler.log("Copying resource file " + srcf);
		compiler.copy_file(src = srcf, dst = dstf);
		
		pos = String.find_last(fn_img, ".");
		string droidId = String.substr(fn_img, 0, pos);
		inst.droidId = String.substr(fn_img, 0, pos);
		inst.assignValue = "@" + inst.droid_location + "/" + inst.droidId;
	}
	
	//TIP: well, need to iterate through instances any more
	for(var instRes in instances)
	{
		render_resource(instRes);
	}
	
	//TRACE: log
	//compiler.log("End Rendering Resources Instances...");
}

on render_types(app)
{
}

//TIP: live is hard,... from here, begin duplicate code of android idiom...
on render_type_initialization(clazz, bns, app)
{
}

on render_initialization(clazz, bns, app)
{
	clazz = this;
	
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

method render_resource(resource)
{
	if(resource.resource_idiom_processed)
		return;
	
	//compiler.log(resource.id);
	
	var xkpName 	= [];
	var droidName 	= [];
	var typeName 	= [];
	var identifier 	= [];
	
	//compiler.log(resource.children.size);
	if(resource.children.size > 0)
	{
		for(var res in resource.children)
		{
			//compiler.log(res.resrc);
			if(res.resrc == true)
			{
				xkpName 	+= res.id;
				droidName 	+= res.src;
				typeName 	+= res.droid_type;
				identifier	+= res.droid_location + "." + res.droidId;
			}
			else
			{
				render_resource(res);
			}
		}
		
		string comma = ",";
		
		//TIP: declare mResourcesXKPName constant array
		out(indent = 1, marker = "declarations")
		{
			private String [] mResources_<xss:e value="resource.output_id"/>_XKPName = <xss:open_brace/>
		}
		
		for(int i1 = 0; i1 < xkpName.size; i1++)
		{
			var item1 = xkpName[i1];
			if(i1 == xkpName.size - 1) comma = "";
			
			out(indent = 2, marker = "declarations")
			{
				"<xss:e value="item1"/>"<xss:e value="comma"/>
			}
		}
		
		out(indent = 1, marker = "declarations")
		{
			<xss:close_brace/>;
		}
		
		//TIP: declare mResourcesDroidName constant array
		out(indent = 1, marker = "declarations")
		{
			private String [] mResources_<xss:e value="resource.output_id"/>_DroidName = <xss:open_brace/>
		}
		
		comma = ",";
		for(int i2 = 0; i2 < droidName.size; i2++)
		{
			var item2 = droidName[i2];
			if(i2 == droidName.size - 1) comma = "";
			
			out(indent = 2, marker = "declarations")
			{
				"<xss:e value="item2"/>"<xss:e value="comma"/>
			}
		}
		
		out(indent = 1, marker = "declarations")
		{
			<xss:close_brace/>;
		}
		
		//TIP: declare mResourcesType constant array
		out(indent = 1, marker = "declarations")
		{
			private Integer [] mResources_<xss:e value="resource.output_id"/>_Type = <xss:open_brace/>
		}
		
		comma = ",";
		for(int i3 = 0; i3 < typeName.size; i3++)
		{
			var item3 = typeName[i3];
			if(i3 == typeName.size - 1) comma = "";
			
			out(indent = 2, marker = "declarations")
			{
				<xss:e value="resource.type.output_id"/>.<xss:e value="item3"/><xss:e value="comma"/>
			}
		}
		
		out(indent = 1, marker = "declarations")
		{
			<xss:close_brace/>;
		}
		
		//TIP: declare mResourcesId constant array
		out(indent = 1, marker = "declarations")
		{
			private Integer [] mResources_<xss:e value="resource.output_id"/>_Id = <xss:open_brace/>
		}
		
		comma = ",";
		for(int i4 = 0; i4 < identifier.size; i4++)
		{
			var item4 = identifier[i4];
			if(i4 == identifier.size - 1) comma = "";
			
			out(indent = 2, marker = "declarations")
			{
				R.<xss:e value="item4"/><xss:e value="comma"/>
			}
		}
		
		out(indent = 1, marker = "declarations")
		{
			<xss:close_brace/>;
		}
	}
	
	resource.resource_idiom_processed = true;
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
}

on render_idiom_scripts(main)
{

}
