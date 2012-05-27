property comp_mat_array = [];

on pre_process(obj)
{		
	if(obj.class_name == "composite_material"){
			comp_mat_array += obj.id;			
	}
	if (obj.class_name == "scene")
		obj.id = "manager.scene";	
	for(var comp_mat in comp_mat_array){
		if(comp_mat == obj.material){
			var value = obj.material;			
			var value_type = compiler.type_of(value);
			obj.add_property("material_array", value, value_type);
			obj.material = "new THREE.MeshFaceMaterial()";				
		}
	}	
	if(obj.id == '')
		obj.id = compiler.genid(obj.class_name);
	if(obj.parent)	
		if(obj.parent.wait_for_package)
			obj.wait_for_package = obj.parent.wait_for_package;
}

on render_dependencies()
{
    var dependencies = compiler.idiom_dependencies("webgl");

    for(var dep in dependencies)
    {       
        compiler.xss("../common-js/dependency.xss", dep);
    }
}

on render_instances()
{	
	for(var inst in instances)
    {	
		if(inst.wait_for_package)
		{			
			out()
			{				
				<xss:e v="inst.wait_for_package"/>.events.addListener("loaded", function()
				<xss:open_brace/>				
			}						
		}	
		compiler.xss("inst_renderer.xss", inst);	
		if(inst.wait_for_package)
			out(){<xss:close_brace/>);}
    }	
}

on render_inst_elems()
{
	for(var inst in instances)
    {
		if(!inst.dont_render)
		{
			if(inst.wait_for_package)
			{
				out()
				{
				<xss:e v="inst.wait_for_package"/>.events.addListener("loaded", function()
				<xss:open_brace/>
				}
			}
			compiler.xss("../common-js/instance.xss", inst);
			if(inst.wait_for_package)
				out(){<xss:close_brace/>);}
		}
	}
}

on render_types()
{
	compiler.log("Rendering 3js Types...");

    for(var ut in user_types)
    {
		compiler.xss("class.xss", ut, renderer = "inst_renderer.xss");
    }
}