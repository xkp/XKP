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
	mouseup: function(manager, x, y)
	{
		x = x - manager.canvas_position.x;
		y = y - manager.canvas_position.y;
		this.mouse_pressed = false;
	    if (this.dragging)
	    {
	        this.dragging = false;
			this.dragend = manager.events.dispatch("dragend", [x, y]);
			if(this.intersects.length > 0 && this.intersects[0].object.events)
				this.over_dragend = this.intersects[0].object.events.dispatch("dragend", [x, y]);	        
	    }
		if(!this.drag && !this.dragend)			
			{
				manager.events.dispatch("mouseup", [x, y]);
				manager.events.dispatch("click", [x, y]); 
			}
		if(!this.over_drag && !this.over_dragend)
			if(this.intersects.length > 0 && this.intersects[0].object.events){
				this.intersects[0].object.events.dispatch("mouseup", [x, y]);	
				this.intersects[0].object.events.dispatch("click", [x, y]);		
			}
		if(this.intersects.length > 0 && this.intersects[0].object.geometry)
		{
			this.text_coord = this.get_texture_coord();
			if(this.intersects[0].object.material)
			{
				var texture;
				if(this.intersects[0].face.materialIndex)
					texture = this.intersects[0].object.geometry.materials[this.intersects[0].face.materialIndex].map;
				else					
					texture = this.intersects[0].object.material.map;
				manager.events.dispatch("t_mouseup", [this.text_coord.x, this.text_coord.y, texture]);
			}
		}
		this.drag = this.over_drag = this.dragend = this.over_dragend = false;
	},	
	mousedown: function(manager, x, y)
	{
		x = x - manager.canvas_position.x;
		y = y - manager.canvas_position.y;
		this.mouse_pressed = true;
		manager.events.dispatch("mousedown", [x, y]); 		
		if(this.intersects.length > 0 && this.intersects[0].object.events)
			this.intersects[0].object.events.dispatch("mousedown", [x, y]);		
		if(this.intersects.length > 0 && this.intersects[0].object.geometry)
		{
			this.text_coord = this.get_texture_coord();
			if(this.intersects[0].object.material)
			{
				var texture;
				if(this.intersects[0].face.materialIndex)
					texture = this.intersects[0].object.geometry.materials[this.intersects[0].face.materialIndex].map;
				else					
					texture = this.intersects[0].object.material.map;
				manager.events.dispatch("t_mousedown", [this.text_coord.x, this.text_coord.y, texture]);
			}
		}
	},	
	mousemove: function(manager, x, y)
	{				
		x = x - manager.canvas_position.x;
		y = y - manager.canvas_position.y;
		this.intersects = [];
		this.get_3js_intersects(manager, x, y);		
		if(this.intersects.length > 0){
			if(!(this.intersects[0].object.parent.mouse_thru))
				this.intersects[0].object = this.intersects[0].object.parent;			
		}
		if(this.mouse_pressed)
			this.dragging = true;
	    if (this.dragging)
	    {
			this.drag = manager.events.dispatch("drag", [x, y]);
			if(this.intersects.length > 0 && this.intersects[0].object.events)
				this.over_drag = this.intersects[0].object.events.dispatch("drag", [x, y]);	        
	    }
		if(!this.over_drag && !this.over_dragend)
			manager.events.dispatch("mousemove", [x, y]); 
		objects = manager.scene.__objects;		
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
			if(this.intersects[0].object.material)
			{
				var texture;
				if(this.intersects[0].face.materialIndex)
					texture = this.intersects[0].object.geometry.materials[this.intersects[0].face.materialIndex].map;
				else					
					texture = this.intersects[0].object.material.map;
				manager.events.dispatch("t_mousemove", [this.text_coord.x, this.text_coord.y, texture]);
			}
		}
	},
	keydown: function(manager, keycode)
	{
		manager.events.dispatch("keydown", [keycode]);    
	},

	keyup: function(manager, keycode)
	{
		manager.events.dispatch("keyup", [keycode]);    
	},

	keypress: function(manager, keycode)
	{
		manager.events.dispatch("keypress", [keycode]);    
	},

	get_3js_intersects: function(manager, x, y){		
			var vector = new THREE.Vector3( ( x / manager.renderer.domElement.width ) * 2 - 1, - ( y / manager.renderer.domElement.height ) * 2 + 1, 0.5 );
			projector = new THREE.Projector();
			projector.unprojectVector( vector, active_camera );
			var ray = new THREE.Ray( active_camera.position, vector.subSelf( active_camera.position ).normalize() );
			this.intersects = ray.intersectObjects( manager.scene.__objects );					
	},
	
	get_texture_coord: function(){		
		var face = this.intersects[0].face, a = new THREE.Vector3(),
		b = new THREE.Vector3(), c = new THREE.Vector3(),
		d = new THREE.Vector3(),
		pa, bp, pd, da, ab, s_apd, s_bpa, hx, hy,
		object = this.intersects[0].object, hx_coord, hy_coord,
		geometry = object.geometry, hitPoint = this.intersects[0].point,
		vertices = geometry.vertices,
		objMatrix = object.matrixWorld;
		a = objMatrix.multiplyVector3( a.copy( vertices[ face.a ] ) );
		b = objMatrix.multiplyVector3( b.copy( vertices[ face.b ] ) );
		c = objMatrix.multiplyVector3( c.copy( vertices[ face.c ] ) );		
		d = face instanceof THREE.Face4 ? d = objMatrix.multiplyVector3( d.copy( vertices[ face.d ] ) ) : null;
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





		