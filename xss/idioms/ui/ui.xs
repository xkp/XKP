
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
            							
			var ui = new ms.ui.Manager(client, streamer);
		
			var g_ui = ui;
			var g_ui_root = ui.root;
		}
	}
}

on render_types()
{
	compiler.log("Rendering UI Types...");

    for(var ut in user_types)
    {
        var full_path = compiler.full_path("component.xss");
		compiler.xss("../common-js/class.xss", ut, renderer = full_path, context = ut);
    }
}

on render_ui_instances()
{
	compiler.log("Rendering UI...");
	
	out(){
	drawingCanvas.onmousemove = function(ev)
	{
		ui.mousemove(ev.clientX, ev.clientY);                  
	};
	drawingCanvas.onmousedown = function(ev)
	{
		ui.mousedown(ev.clientX, ev.clientY);                  
	};
	drawingCanvas.onmouseup = function(ev)
	{
		ui.mouseup(ev.clientX, ev.clientY);                  
	};	
	}
	
    for(var i in instances)
    {		
		if(i.renderer)
			compiler.xss(i.renderer, i);			
		else
			compiler.xss("component.xss", i, false);
    }
}