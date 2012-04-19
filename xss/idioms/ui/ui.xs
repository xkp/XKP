on pre_process(obj)
{
	if(obj.id == '')
		obj.id = compiler.genid(obj.class_name);
}

on render_js_includes()
{
	out()
	{
		<script type="text/javascript" src="../js/ms-ui.js"></script>
	}
}

on render_initialization()
{
	out()
	{
		if(drawingCanvas){
			var client =
			{
				width: <xss:e value="application.width"/>,
				height: <xss:e value="application.height"/>,
				canvas: drawingCanvas
			};
            var streamer = new ms.streamer.Streamer();							
			var ui = new ms.ui.Manager(client, streamer);			
			var g_ui = ui;
			var g_ui_root = ui.root;
			var mouse_pressed = false;
		}
	}
}

on render_types()
{
	compiler.log("Rendering UI Types...");
    for(var ut in user_types)
    {		
		/* *** no need
        for(var inst in ut.instances)
		{
			string parent_id = inst.parent.output_id;
			if (parent_id == ut.output_id)
				parent_id = "this";
			string manager_id = parent_id + ".manager";    
			var value_type = compiler.type_of(parent_id);
			inst.add_property("parent", parent_id, value_type);	
			inst.add_property("manager", manager_id, value_type);
		}
        */

        compiler.log(ut.id);
        var full_path = compiler.full_path("component.xss");		
		compiler.xss("../common-js/class.xss", ut, renderer = full_path, context = ut);			
    }
}

on render_instances()
{
	compiler.log("Rendering UI...");
	out(){	
	drawingCanvas.onmousemove = function(ev)
	{
		ui.mousemove(ev.clientX - canvas_position.x, ev.clientY - canvas_position.y);                  
	};
	drawingCanvas.onmousedown = function(ev)
	{
		ui.mousedown(ev.clientX - canvas_position.x, ev.clientY - canvas_position.y);                  
	};
	drawingCanvas.onmouseup = function(ev)
	{
		ui.mouseup(ev.clientX - canvas_position.x, ev.clientY - canvas_position.y);                  
	};		
	ui.events.addListener("mousedown", function(x,y)
	{    
		application.events.dispatch("mousedown", [x, y]);
	});
	ui.events.addListener("mouseup", function(x,y)
	{    
		application.events.dispatch("mouseup", [x, y]);
	});
	ui.events.addListener("mousemove", function(x,y)
	{    
		application.events.dispatch("mousemove", [x, y]);
	});
	ui.events.addListener("click", function(x,y)
	{    
		application.events.dispatch("click", [x, y]);
	});
	ui.events.addListener("drag", function(x,y)
	{    
		application.events.dispatch("drag", [x, y]);
	});
	ui.events.addListener("dragend", function(x,y)
	{    
		application.events.dispatch("dragend", [x, y]);
	});
	ui.events.addListener("keyup", function(keycode)
	{    
		application.events.dispatch("keyup", [keycode]);
	});
	ui.events.addListener("keydown", function(keycode)
	{    
		application.events.dispatch("keydown", [keycode]);
	});
	ui.events.addListener("keypress", function(keycode)
	{    
		application.events.dispatch("keydown", [keycode]);
	});
	}
	
    for(int i = 0; i < instances.size; i++)
    {			
		var inst = instances[i];		
		/* ** No need
        string parent_id = inst.parent.output_id;		
        if (parent_id == "application")
            parent_id = "g_ui_root";
		string manager_id = parent_id + ".manager";    
		var value_type = compiler.type_of(parent_id);
		inst.add_property("parent", parent_id, value_type);	
		inst.add_property("manager", manager_id, value_type);
		*/
        
        if(inst.wait_for_package)
		{			
			out()
			{
				<xss:e v="inst.wait_for_package"/>.events.addListener("loaded", function()
				<xss:open_brace/>				
			}
			compiler.xss("component.xss", inst, false);
			for(var child in inst.children)
			{
				if(child.renderer)
					compiler.xss(child.renderer, child);			
				else
					compiler.xss("component.xss", child, false);
				i++;
			}
			out(){<xss:close_brace/>);}
		}
		else if(inst.renderer)
			compiler.xss(inst.renderer, inst);			
		else
			compiler.xss("component.xss", inst, false);
    }
}