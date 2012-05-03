property comp_mat_array = [];

on pre_process(obj)
{
	if(obj.class_name == "composite_material"){
			comp_mat_array += obj.id;			
	}
	if (obj.class_name == "scene")
		obj.id = "scene";	
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
}

on compile_dependency(dep)
{
    if (!dep.idiom)
        return;

    if (dep.idiom.id != "threejs")
        return;

    var path = project.threejs_path;
    if (!path)
        path = "../js/three";

    dep.href = path + '/' + dep.href;
}

on render_instances()
{	
	for(int i = 0; i < instances.size; i++)
    {		
		var inst = instances[i];
		if(inst.wait_for_package)
		{			
			out()
			{				
				<xss:e v="inst.wait_for_package"/>.events.addListener("loaded", function()
				<xss:open_brace/>				
			}
			compiler.xss("inst_renderer.xss", inst);
			if(!inst.dont_render)
				compiler.xss("../common-js/instance.xss", inst);
			for(var child in inst.children)
			{
				compiler.xss("inst_renderer.xss", child);
				if(!child.dont_render)
					compiler.xss("../common-js/instance.xss", child);
				i++;
			}
			out(){<xss:close_brace/>);}
		}
		else
		{
			compiler.xss("inst_renderer.xss", inst);
			if(!inst.dont_render)
				compiler.xss("../common-js/instance.xss", inst);
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