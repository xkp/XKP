"ms.event".namespace();

ms.event.EventHolder = Class.create(
{
	addListener: function(eventName, funcHandler)
    {
		var handlerList = this[eventName];
		if (handlerList == undefined)
			this[eventName] = [funcHandler];
		else
			handlerList.push(funcHandler);
	},
	dispatch: function(eventName, eventData)
    {
		var handlerList = this[eventName];
        if (handlerList)
        {
            for(var i = 0; i < handlerList.length; i++)
				handlerList[i].apply(this, eventData);
		}
    },
});

ms.event.ThreeEventManager = Class.create(
{
	initialize: function()     
	{		
		this.intersects = [];
		this.dragging 		= false;
		this.mouse_pressed	= false;		
	},		
	mouseup: function(x, y)
	{
		x = x - canvas_position.x;
		y = y - canvas_position.y;
		this.mouse_pressed = false;
	    if (this.dragging)
	    {
	        this.dragging = false;
	        application.events.dispatch("dragend", [x, y]);
			if(this.intersects.length > 0 && this.intersects[0].object.events)
				this.intersects[0].object.events.dispatch("dragend", [x, y]);
	        return true;
	    }
		application.events.dispatch("mouseup", [x, y]);
		application.events.dispatch("click", [x, y]); 
		if(this.intersects.length > 0 && this.intersects[0].object.events){
			this.intersects[0].object.events.dispatch("mouseup", [x, y]);	
			this.intersects[0].object.events.dispatch("click", [x, y]);		
		}
	},	
	mousedown: function(x, y)
	{
		x = x - canvas_position.x;
		y = y - canvas_position.y;
		this.mouse_pressed = true;
		application.events.dispatch("mousedown", [x, y]); 
		if(this.intersects.length > 0 && this.intersects[0].object.events)
			this.intersects[0].object.events.dispatch("mousedown", [x, y]);		
	},	
	mousemove: function(x, y)
	{
		x = x - canvas_position.x;
		y = y - canvas_position.y;
		this.intersects = this.get_3js_intersects(x, y);
		if(this.intersects.length > 0)			
			if(!(this.intersects[0].object.parent instanceof THREE.Scene))
				this.intersects[0].object = this.intersects[0].object.parent;
		if(this.mouse_pressed)
			this.dragging = true;
	    if (this.dragging)
	    {
			application.events.dispatch("drag", [x, y]);
			if(this.intersects.length > 0 && this.intersects[0].object.events)
				this.intersects[0].object.events.dispatch("drag", [x, y]);
	        return true;
	    }
		application.events.dispatch("mousemove", [x, y]); 
		objects = scene.objects;		
		for ( i = 0; i < objects.length; i++ ) {
				object = objects[i];
				if (object.events) 
					object.events.dispatch("mouseout", [x, y]);
		}
		if(this.intersects.length > 0 && this.intersects[0].object.events){
			this.intersects[0].object.events.dispatch("mousemove", [x, y]);
			this.intersects[0].object.events.dispatch("mousein", [x, y]);			
		}				
	},		
	keydown: function(keycode)
	{
	    application.events.dispatch("keydown", [keycode]);    
	},

	keyup: function(keycode)
	{
	    application.events.dispatch("keyup", [keycode]);    
	},

	keypress: function(keycode)
	{
	    application.events.dispatch("keypress", [keycode]);    
	},

	get_3js_intersects: function(x, y){
		if(application.are_obj_events){
			this.intersects = [];
			var vector = new THREE.Vector3( ( x / app_width ) * 2 - 1, - ( y / app_height ) * 2 + 1, 0.5 );
			projector = new THREE.Projector();
			projector.unprojectVector( vector, active_camera );
			var ray = new THREE.Ray( active_camera.position, vector.subSelf( active_camera.position ).normalize() );
			var intersects = ray.intersectScene( scene );	
			return intersects;
		}
		return [];
	}
});





		