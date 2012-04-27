var ev = {};

ev.EventHolder = Class.extend(
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
			return true;
		}
		return false;
    },
});

ev.ThreeEventManager = Class.extend(
{
	init: function()     
	{		
		this.intersects = [];
		this.dragging 		= false;
		this.mouse_pressed	= false;
		this.text_coord;
		this.drag = false;
		this.over_drag = false;
		this.dragend = false;
		this.over_dragend = false;
	},		
	mouseup: function(x, y)
	{
		x = x - canvas_position.x;
		y = y - canvas_position.y;
		this.mouse_pressed = false;
	    if (this.dragging)
	    {
	        this.dragging = false;
			if(!ui)
				this.dragend = application.events.dispatch("dragend", [x, y]);
			if(this.intersects.length > 0 && this.intersects[0].object.events)
				this.over_dragend = this.intersects[0].object.events.dispatch("dragend", [x, y]);	        
	    }
		if(!this.drag && !this.dragend)
			if(!ui)
			{
				application.events.dispatch("mouseup", [x, y]);
				application.events.dispatch("click", [x, y]); 
			}
		if(!this.over_drag && !this.over_dragend)
			if(this.intersects.length > 0 && this.intersects[0].object.events){
				this.intersects[0].object.events.dispatch("mouseup", [x, y]);	
				this.intersects[0].object.events.dispatch("click", [x, y]);		
			}
		if(this.intersects.length > 0 && this.intersects[0].object.geometry)
		{
			this.text_coord = this.get_texture_coord();
			if(this.intersects[0].face.materials[0])
			{
				var texture = this.intersects[0].face.materials[0].map;
				application.events.dispatch("t_mouseup", [this.text_coord.x, this.text_coord.y, texture]);
			}
		}
		this.drag = this.over_drag = this.dragend = this.over_dragend = false;
	},	
	mousedown: function(x, y)
	{
		x = x - canvas_position.x;
		y = y - canvas_position.y;
		this.mouse_pressed = true;
		if(!ui)		
			application.events.dispatch("mousedown", [x, y]); 		
		if(this.intersects.length > 0 && this.intersects[0].object.events)
			this.intersects[0].object.events.dispatch("mousedown", [x, y]);		
		if(this.intersects.length > 0 && this.intersects[0].object.geometry)
		{
			this.text_coord = this.get_texture_coord();
			if(this.intersects[0].face.materials[0])
			{
				var texture = this.intersects[0].face.materials[0].map;
				application.events.dispatch("t_mousedown", [this.text_coord.x, this.text_coord.y, texture]);
			}
		}
	},	
	mousemove: function(x, y)
	{				
		x = x - canvas_position.x;
		y = y - canvas_position.y;
		this.intersects = this.get_3js_intersects(x, y);		
		if(this.intersects.length > 0){
			if(!(this.intersects[0].object.parent instanceof THREE.Scene))
				this.intersects[0].object = this.intersects[0].object.parent;			
		}
		if(this.mouse_pressed)
			this.dragging = true;
	    if (this.dragging)
	    {
			if(!ui)
				this.drag = application.events.dispatch("drag", [x, y]);
			if(this.intersects.length > 0 && this.intersects[0].object.events)
				this.over_drag = this.intersects[0].object.events.dispatch("drag", [x, y]);	        
	    }
		if(!this.over_drag && !this.over_dragend)
			if(!ui)
				application.events.dispatch("mousemove", [x, y]); 
		objects = scene.objects;		
		for ( i = 0; i < objects.length; i++ ) {
				object = objects[i];
				if (object.events) 
					object.events.dispatch("mouseout", [x, y]);
		}
		if(!this.over_drag && !this.over_dragend)
			if(this.intersects.length > 0 && this.intersects[0].object.events){
				this.intersects[0].object.events.dispatch("mousemove", [x, y]);
				this.intersects[0].object.events.dispatch("mousein", [x, y]);			
			}	
		if(this.intersects.length > 0 && this.intersects[0].object.geometry)
		{
			this.text_coord = this.get_texture_coord();
			if(this.intersects[0].face.materials[0])
			{
				var texture = this.intersects[0].face.materials[0].map;
				application.events.dispatch("t_mousemove", [this.text_coord.x, this.text_coord.y, texture]);
			}
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
	},
	
	get_texture_coord: function(){
		var face = this.intersects[0].face, a, b, c, d,
		pa, bp, pd, da, ab, s_apd, s_bpa, hx, hy,
		object = this.intersects[0].object, hx_coord, hy_coord,
		geometry = object.geometry, hitPoint = this.intersects[0].point,
		vertices = geometry.vertices,
		objMatrix = object.matrixWorld;
		a = objMatrix.multiplyVector3( vertices[ face.a ].position.clone() );
		b = objMatrix.multiplyVector3( vertices[ face.b ].position.clone() );
		c = objMatrix.multiplyVector3( vertices[ face.c ].position.clone() );
		d = face instanceof THREE.Face4 ? objMatrix.multiplyVector3( vertices[ face.d ].position.clone() ) : null;
		pa = distance3d(a, hitPoint);
		bp = distance3d(b, hitPoint);		
		pd = distance3d(d, hitPoint);
		da = distance3d(a, d);
		ab = distance3d(a, b);
		s_apd = (pa + pd + da)/2;
		s_bpa = (pa + bp + ab)/2;
		hy = 2*Math.sqrt(s_apd*(s_apd-da)*(s_apd-pa)*(s_apd-pd))/da;
		hx = 2*Math.sqrt(s_bpa*(s_bpa-ab)*(s_bpa-bp)*(s_bpa-pa))/ab;
		hx_coord = hx/da;
		hy_coord = hy/ab;
		return {x: hx_coord, y: hy_coord};
	}
});





		