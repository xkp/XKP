"ms.ui".namespace();

ms.ui.Manager = Class.create(
{
	initialize: function(client, streamer)
	{
		this.streamer = streamer;

	    //setup our components
	    this.root = new ms.ui.Component(this);
	    this.root.rect(0, 0, client.width, client.height);

	    //mouse handling
	    this.mouse_over = this.root;
	    this.dragging 		= false;
		this.mouse_pressed	= false;
	    this.lastx 		= 0;
	    this.lasty 		= 0;

	},	

	//Resource handling
	load_resources: function(callback)
	{
		var resources = [];
		for(var i = 0; i < this.streamer.resources.length; i++)
		{
			var res = this.streamer.resources[i];
			if(!res.loaded)
				resources.push({ type: res.type, url: res.asset});			
		}

		if (resources.length > 0)
		{
			var this_ = this;
			var stream_client =
			{
				resource_loaded: function(res, data)
				{					
				},

				finished_loading: function()
				{
					callback();
				}
			};

			this.streamer.request(resources, stream_client);
		}
		else callback();
	},

	//Keyboard managemet
	keydown: function(keycode)
	{	
		for(var i = 0; i < this.root.components.length; i++)
        {			
            var cmp = this.root.components[i];	
			if(cmp.physics)
				cmp.physics.SetAwake(true);
        }		
	    application.events.dispatch("keydown", [keycode]);    
	},

	keyup: function(keycode)
	{
		for(var i = 0; i < this.root.components.length; i++)
        {			
            var cmp = this.root.components[i];	
			if(cmp.physics)
				cmp.physics.SetAwake(true);
        }	
	    application.events.dispatch("keyup", [keycode]);    
	},

	keypress: function(keycode)
	{
		for(var i = 0; i < this.root.components.length; i++)
        {			
            var cmp = this.root.components[i];	
			if(cmp.physics)
				cmp.physics.SetAwake(true);
        }	
	    application.events.dispatch("keypress", [keycode]);    
	},	
	
	//Mouse management
	mousemove: function(x, y)
	{
		x = x - canvas_position.x;
		y = y - canvas_position.y;
		if(this.mouse_over.physics){
			this.mouse_over.physics.SetAwake(true);
			if(this.mouse_over.physics.mouse_joint)
				this.create_mouse_joint(x, y, this.mouse_over.physics);			
		}	
	    if(this.mouse_pressed)
			this.dragging = true;
	    if (this.dragging)
	    {			
			this.mouse_over.events.dispatch("drag", [x, y]);
	        this.lastx = x;
	        this.lasty = y;
	        return true;
	    }

	    var over = this.root.find(x, y);
	    if (!over)
	        over = this.root;

	    if (over != this.mouse_over)
	    {
	    	this.mouse_over.events.dispatch("mouseout", [this.mouse_over]);

	        over.events.dispatch("mousein", [over]);

	        this.mouse_over = over;
	    }

	    
	    over.events.dispatch("mousemove", [x, y, over]);

	    this.lastx = x;
	    this.lasty = y;

	    return this.mouse_over != this.root;
	},
	
	mousedown: function(x, y)
	{
		x = x - canvas_position.x;
		y = y - canvas_position.y;
		if(this.mouse_over.physics)
			this.mouse_over.physics.SetAwake(true);
		
	    this.mouse_over.events.dispatch("mousedown", [x, y]);
	    
		this.mouse_pressed = true;

	    return this.mouse_over != this.root;
	},

	mouseup: function(x, y)
	{
		x = x - canvas_position.x;
		y = y - canvas_position.y;
		if(this.mouse_over.physics)
			this.mouse_over.physics.SetAwake(true);
			
		this.mouse_pressed = false;
	    if (this.dragging)
	    {
	        this.dragging = false;
	        
	        this.mouse_over.events.dispatch("dragend", [x, y]);
	        return true;
	    }
	    
	    this.mouse_over.events.dispatch("mouseup", [x, y]);
	    	    
	    this.mouse_over.events.dispatch("click", []);	   

	    return this.mouse_over != this.root;
	},
	
	create_mouse_joint: function(x, y, body){
		var handledMouseX = x  / 30;
		var handledMouseY = y  / 30;
		if(this.mouse_pressed && (!this.mouseJoint)) {
			var md = new b2MouseJointDef();
            md.bodyA = g_world.GetGroundBody();
            md.bodyB = body;
            md.target.Set(handledMouseX, handledMouseY);
            md.collideConnected = true;
            md.maxForce = 300.0 * body.GetMass();
            this.mouseJoint = g_world.CreateJoint(md);
        }
        if(this.mouseJoint) {
           if(this.mouse_pressed) {
              this.mouseJoint.SetTarget(new b2Vec2(handledMouseX, handledMouseY));
           } else {
              g_world.DestroyJoint(this.mouseJoint);
              this.mouseJoint = null;
           }
        }	
	},

    screen_pos: function(cmp)
    {
        var p = cmp.parent;
        var result = {x: cmp.x, y: cmp.y};
        while(p)
        {
            result.x += p.x;
            result.y += p.y;

            p = p.parent;
        }
        return result;
    },

    invalidate_all: function()
    {
        this.redraw_ = true;
    },

	invalidate: function(cmp)
	{
        this.invalidate_all();
        return;

        //td: make it work
        var gpos = this.screen_pos(cmp);
        gpos.w = cmp.w;
        gpos.h = cmp.h;

        var found = false;
        
        for(var i = 0; i < this.dirt_.length; i++)
        {
            var rect = this.dirt_[i];
            if (rect.intersects(gpos))
            {
                if (rect.x > cmp.x)
                    rect.x = cmp.x; 

                if (rect.y > cmp.y)
                    rect.y = cmp.y; 

                if (rect.x + rect.w < cmp.x + cmp.w)
                    rect.w = cmp.x + cmp.w - rect.x; 

                if (rect.y + rect.h < cmp.y + cmp.h)
                    rect.h = cmp.y + cmp.h - rect.y; 

                    found = true;
                break;
            }
        }

        if (!found)
        {
            this.dirt_.push(
            {
                x: gpos.x,
                y: gpos.y,
                w: gpos.w,
                h: gpos.h,

                intersects: function(r)
                {
                    if (this.x + this.w < r.x)
                        return false;

                    if (r.x + r.w < this.x)
                        return false;

                    if (this.y + this.h < r.y)
                        return false;

                    if (r.y + r.h < this.y)
                        return false;

                    return true;
                },

                isContainedBy: function(r)
                {
                    return  r.x <= this.x && 
                            r.y <= this.y &&
                            r.x + r.w >= this.x + this.w &&
                            r.y + r.h >= this.y + this.h;
                },
            });
        }
	},
    
    dirt_: [],
    backgroundFill: '#FFFFFF',
    fill_background: true,

    findContainer: function(rect)
    {
        var path = [];
        function do_find(x, y, cmp, r)
        {
            var rr = {x: cmp.x + x, y: cmp.y + y, w: cmp.w, h: cmp.h};
            if (r.isContainedBy(rr))
            {
                path.push(cmp);
                for(var i = 0; i < cmp.components.length; i++)
                {
                    do_find(x + cmp.y, y + cmp.y, cmp.components[i], r);    
                }
            }
        }

        do_find(0, 0, this.root, rect);

        var last = null;
        for(var i = path.length - 1; i >= 0; i--)
        {
            var cmp = path[i];
            if (cmp != this.root)
                last = cmp;

            if (cmp.opaque())
                return cmp;
        }

        return last;
    },

    update: function(elapsed, context)
    {
        if (this.redraw_)
        {
            this.redraw_ = false;

            this.draw(context);
            this.dirt_ = [];
            return;
        }


        var bg_dirt    = [];
        var to_draw    = [];
        
        for(var i = 0; i < this.dirt_.length; i++)
        {
            var rect = this.dirt_[i];
            var cmp  = this.findContainer(rect);

            if (cmp)
            {
                to_draw.push(cmp);
                
                if (!cmp.opaque())
                {
                    bg_dirt.push(rect);                    
                }
            }
            else
                bg_dirt.push(rect);                    
        }

        for(var i = 0; i < bg_dirt.length; i++)
        {
            var rect = bg_dirt[i];
            context.fillStyle = this.backgroundFill;

            context.fillRect(rect.x, rect.y, rect.w, rect.h);
        }

		for(var i = 0; i < to_draw.length; i++)
        {
            var cmp  = to_draw[i];
            var gpos = this.screen_pos(cmp.parent);

            cmp.draw(context, gpos.x, gpos.y);
        }

        this.dirt_ = [];
    },

    draw: function(context)
    {
        //background
        if (this.fill_background)
        {
            context.fillStyle = this.backgroundFill;
            context.fillRect(this.root.x, this.root.y, this.root.w, this.root.h);
        }
        this.root.draw(context, 0, 0);
    },
});

ms.ui.Component = Class.create(
{
	initialize: function(manager, parent)
	{
		if(manager == null) manager = g_ui_root.manager;
		if(parent == null) parent = g_ui_root;

		this.manager    = manager;
		this.components = [];
		this.parent 	= parent;	
		this.events = new ms.event.EventHolder();

		this.x 			= 0; 
        this.y 			= 0; 
        this.w 			= 0;
        this.h 			= 0;
		this.rotation 	= 0;		

        this.opacity = null; //this means, no opacity

		if (parent)
			parent.addComponent(this);
	},

    count: function()
    {
        return this.components.length;
    },

    opaque: function()
    {
        return this.opacity == null;
    },

    get_alpha: function()
    {
        if (this.opacity == null)
            return 100;
        return this.opacity*100;
    },

    alpha: function(value)
    {
        if (value < 0)
            value = 0;
		
        this.opacity = value/100;
        this.invalidate();
    },

	set_rotation: function(value)
    {
        this.rotation = value * Math.PI/180;
        this.invalidate();
    },
	
	get_rotation: function()
	{
		return this.rotation*180/Math.PI;
	},

	rect: function(x, y, w, h)
	{
		this.x = x;
		this.y = y;
		this.w = w;
		this.h = h;

        //td: notify
		this.positioned();
		this.resized();
	},

	position: function(x, y)
	{
		this.x = x;
		this.y = y;

        //td: notify
        this.positioned();
	},

	size: function(w, h)
	{
		this.w = w;
		this.h = h;

        //td: notify
		this.resized();
	},

    get_width: function()
    {
        return this.w;
    },

    get_height: function()
    {
        return this.h;
    },

    set_x: function(value)
    {
        this.position(value, this.y);
    },

    set_y: function(value)
    {
        this.position(this.x, value);
    },

    set_width: function(value)
    {
        this.size(value, this.h);
    },

    set_height: function(value)
    {
        this.size(this.w, value);
    },

	client: function()
	{
		this.rect(0, 0, this.parent.w, this.parent.h);
	},

	center: function(w, h)
	{
		this.rect(this.parent.w/2 - w/2, this.parent.h/2 - h/2, w, h);
	},

	bottom: function(h)
	{
		this.rect(0, this.parent.h - h, this.parent.w, h);
	},

	top: function(h)
	{
		this.rect(0, 0, this.parent.w, h);
	},

	right: function(w)
	{
		this.rect(this.parent.w - w, 0, w, this.parent.h);
	},

	left: function(h)
	{
		this.rect(0, 0, w, this.parent.h);
	},

	hide: function()
	{
        if (this.visible_)
            this.invalidate();

        this.visible_ = false;
	},

	show: function()
	{
        if (!this.visible_)
            this.invalidate();

		this.visible_ = true;
	},

	setVisible: function( visible )
	{
        if (this.visible_ != visible)
            this.invalidate();

		this.visible_ = visible;
	},

	isVisible: function()
	{
		return this.visible_;
	},

	positioned: function() 
    {
        this.events.dispatch("moved", [this]);
        this.invalidate()
    },

    resized: function() 
    {
        this.events.dispatch("resized", [this]);
        this.invalidate()
    },

    visible_:   true,

    invalidate: function()
    {
        this.manager.invalidate(this);
    },

	find: function(x, y)
	{
	    if (this.contains(x, y))
	    {
	        x -= this.x;
	        y -= this.y;

	        for(var i = 0; i < this.components.length; i++)
	        {
	            cmp = this.components[i];

	            var visible = cmp.isVisible();

	            if (cmp.mouse_ghost || !visible)
	            	continue;

                var child_result = cmp.find(x, y);
                if (child_result)
                    return child_result;
	        }

	        return this.mouse_thru? null : this;
	    }

	    return null;
	},

	contains: function(x, y)
	{	

		var A_x = - this.w/2;
		var A_y = - this.h/2;

		var ev_x = x - (this.x + this.w/2);
		var ev_y = y - (this.y + this.h/2);
		var ev_x_rotated = ev_x * Math.cos(-this.rotation) - ev_y * Math.sin(-this.rotation);
		var ev_y_rotated = ev_x * Math.sin(-this.rotation) + ev_y * Math.cos(-this.rotation);
		var result = (ev_x_rotated >= A_x && ev_x_rotated <= (A_x + this.w) && 
				 ev_y_rotated >= A_y && ev_y_rotated <= (A_y + this.h));
		return result;		
	},

	//child management
	addComponent: function(cmp)
	{
		this.components.push(cmp);
		this.componentAdded(cmp);
	},

	componentAdded: function(cmp)
	{
	},

    removeComponent: function(cmp)
    {
        var idx = this.components.indexOf(cmp); 
        if (idx!=-1) 
        {
            this.components.splice(idx, 1); 
            this.invalidate();
        }
    },

    draw: function(context, x, y)
    {
        var clip_inside = true;
		for(var i = 0; i < this.components.length; i++)
        {	
			var cmp = this.components[i];
			if(cmp.clip){		
				if(clip_inside){
					context.save();
					clip_inside = false;
				}
				cmp.draw(context, this.x + x, this.y + y);
			}				
		}		
		for(var i = 0; i < this.components.length; i++)
        {			
            var cmp = this.components[i];	
            if (cmp.isVisible())
            {
                cmp.draw(context, this.x + x, this.y + y);
            }
        }
		if(!clip_inside)
			context.restore();
    },	
});

ms.ui.Image = Class.create(ms.ui.Component,
{	
	initialize: function($super, image, manager, parent)
	{				
		$super(manager, parent);
        
        var resource = streamer.get_resource(image);
        
        if (!resource)
            throw "Unknow resource: " + image;
         		
		var texture = resource.data;
		if (image && !texture && image != 'null')
			throw "Image " + image + " not loaded";		
	    this.texture = texture;		
	},

	fill: function(f)
	{
        this.fill_ = f;
        this.invalidate();
	},

    src: function()
    {
        return this.source;
    },

    src: function(image)
    {
        this.source = image;
	    this.texture = streamer.get_resource(image).data;
        this.invalidate();
    },

	image: function(texture)
	{
        this.source = "";
	    this.texture = texture;
        this.invalidate();
	},           
                                  
	draw: function($super, context, x, y)
    {
		var old_alpha;

		context.save();
		context.translate(this.x + x + this.w/2, this.y + y + this.h/2);
		context.rotate(this.rotation);

		if (this.opacity != null)
        {
            old_alpha = context.globalAlpha;
            context.globalAlpha = this.opacity;
        }

        if (this.texture)
        {
            context.drawImage(this.texture, - this.w/2, - this.h/2, this.w, this.h);
        }
        else if (this.fill_)
        {
            context.fillStyle = this.fill_;
            context.fillRect(-this.w/2, -this.h/2, this.w, this.h);
        }

        if (this.opacity != null)
        {
            context.globalAlpha = old_alpha;
        }

		context.restore();  

        $super(context);
    }
});

ms.ui.ProgressBar = Class.create(ms.ui.Component,
{
	initialize: function($super , images, manager, parent)
	{
		$super(manager, parent);
		if (!images)
		{
			images = ['images/progress.empty.png', 'images/progress.full.png'];
		}		
		this.left    = new ms.ui.Image(images[1], this.manager, this);
		this.right   = new ms.ui.Image(images[0], this.manager, this);
		this.percent = 0;
		this.resized();
	},

	value: function(val)
	{
		if (val < 0)
			val = 0;

		if (val > 100)
			val = 100;

		this.percent = val;
		this.resized();
	},

    get_value: function()
    {
        return this.percent;
    },

	resized: function($super)
	{
		var ww = this.w*(this.percent/100);
	    this.left.rect(0, 0, ww, this.h);
	    this.right.rect(ww, 0, this.w - ww, this.h);
        $super();
	}
});

ms.ui.Label = Class.create(ms.ui.Component,
{
	initialize: function($super, font, manager, parent)
	{
		$super(manager, parent);
	    this.font = font;
	    this.value = "";
		this.color_value = 'black';
	},

	text: function(text)
	{
		this.value = text;
		this.manager.invalidate_all(); //td: do extents!!!
	},
	
	set_color: function(value)
	{
		this.color_value = value;	
		this.manager.invalidate_all();
	},

	extents: function()
	{
        //td:
	},

    draw: function($super, context, x, y)
    {
		context.save();

		context.translate(this.x + x + this.w/2, this.y + y + this.h/2);
		context.rotate(this.rotation);

        context.font = this.font;
		context.fillStyle = this.color_value; //td:
        context.fillText(this.value, -this.w/2, -this.h/2);

		context.restore(); 
        $super(context);
    },
});

ms.ui.Button = Class.create(ms.ui.Image,
{
	initialize: function($super, normal, over, manager, parent)
	{
		$super(normal, manager, parent);

		var normal_texture = streamer.get_resource(normal).data;
		var over_texture   = streamer.get_resource(over).data;	
		var ev_parent = this;
		this.events.addListener('mousein', function()
		{
			ev_parent.image(over_texture);
		});	
		this.events.addListener('mouseout', function()
		{
			ev_parent.image(normal_texture);
		});
				
	},	
});

ms.ui.StateButton = Class.create(ms.ui.Image,
{
	initialize: function($super, active, inactive, manager, parent)
	{
		$super(active, manager, parent);

		var active_texture 		= streamer.get_resource(active).data;
		var inactive_texture   	= streamer.get_resource(inactive).data;

		this.active = true;		
		var ev_parent = this;		
		this.events.addListener('click', function()
		{		
			ev_parent.active  = !ev_parent.active;
			ev_parent.image(ev_parent.active? active_texture : inactive_texture);
		});
		
	},	
});

ms.ui.Switch = Class.create(ms.ui.Component,
{
	active_: 0,

	componentAdded: function(cmp)
	{
        cmp.rect(0, 0, this.w, this.h);

		if (this.components.length == (this.active_ + 1))
			cmp.show();
		else
			cmp.hide();
	},

	active: function( idx )
	{
		if (idx == this.active_)
			return;

		this.components[this.active_].hide();

		this.active_ = idx;

		this.components[this.active_].show();

        this.invalidate();
	},

    get_active: function()
    {
        return this.active_;
    },
});

ms.ui.Line = Class.create(ms.ui.Component,
{
	initialize: function($super, manager, parent)
	{
		$super(manager, parent);
	    this.x1         = 0;
	    this.y1         = 0;
	    this.x2         = 0;
	    this.y2         = 0;
        this.lineWidth  = 2;
        this.lineCap    = '';
        this.fillStyle  = 'black'
	},

	coords: function(x1, y1, x2, y2)
	{
	    this.x1         = x1;
	    this.y1         = y1;
	    this.x2         = x2;
	    this.y2         = y2; 

		this.invalidate(); //td: do extents!!!
	},

    set_x1: function(v) { this.coords(v, this.y1, this.x2, this.y2); },
    set_x2: function(v) { this.coords(this.x1, this.y1, v, this.y2); },
    set_y1: function(v) { this.coords(this.x1, v, this.x2, this.y2); },
    set_y2: function(v) { this.coords(this.x1, this.y1, this.x2, v); },

    set_lineWidth: function(v) 
    {
        this.lineWidth = v;
        this.invalidate();
    },

    set_lineCap: function(v) 
    {
        this.lineCap = v;
        this.invalidate();
    },

    positioned: function($super)
    {
        $super();
	    this.x1 = this.x;
	    this.y1 = this.y;
	    this.x2 = this.x + this.w;
	    this.y2 = this.y + this.h;
    },

    resized: function($super)
    {
        $super();
	    this.x2 = this.x + this.w;
	    this.y2 = this.y + this.h;
    },

    draw: function($super, context, x, y)
    {
		old_line_width = context.lineWidth;
        context.fillStyle = this.fillStyle;
        context.beginPath();
        context.lineWidth = this.lineWidth;
        context.moveTo(x + this.x1, y + this.y1);
        context.lineTo(x + this.x2, y + this.y2);
        context.stroke();
		context.lineWidth = old_line_width;
		$super(context);
    },
});

ms.ui.Painter = Class.create(ms.ui.Component,
{	
	initialize: function($super, manager, parent)
	{				
		$super(manager, parent);		
	},

	draw: function($super, context, x, y)
    {
		var old_alpha;

		context.save();
		context.translate(this.x + x + this.w/2, this.y + y + this.h/2);
		context.rotate(this.rotation);
		context.translate(-this.w/2, -this.h/2);

		if (this.opacity != null)
        {
            old_alpha = context.globalAlpha;
            context.globalAlpha = this.opacity;
        }

        if('paint' in this)
		{
			this.paint(context);	
		}

        if (this.opacity != null)
        {
            context.globalAlpha = old_alpha;
        }

		context.restore();  

        $super(context);
    }
});

ms.ui.ClipArea = Class.create(ms.ui.Component,
{	
	initialize: function($super, shape, manager, parent)
	{				
		$super(manager, parent);
		this.clip = true;
		this.shape = shape;
	},	
	draw: function($super, context, x, y)
	{
		context.beginPath();
		if(this.shape == "rect")				
			context.rect(this.x + x, this.y + y, this.w, this.h);					
		else
			context.arc(this.x + x + this.w/2, this.y + y + this.h/2, 
						this.w/2, 0, Math.PI*2, true); 
		context.clip();
	}
});

ms.ui.Rectangle = Class.create(ms.ui.Component,
{	
	initialize: function($super, fill, stroke, line_width, manager, parent)
	{				
		$super(manager, parent);
		this.fill = fill;
		this.stroke = stroke;
		this.line_width = line_width;
	},	

	set_fill: function(value)
	{
		this.fill = value;
		this.invalidate();
	},

	set_stroke: function(value)
	{
		this.stroke = value;
		this.invalidate();
	},

	set_line_width: function(value)
	{
		this.line_width = value;
		this.invalidate();
	},

	draw: function($super, context, x, y)
	{		
		old_line_width = context.lineWidth;
		context.save();
		context.translate(this.x + x + this.w/2, this.y + y + this.h/2);
		context.rotate(this.rotation);
		context.beginPath();		
		if (this.opacity != null)
        {
            old_alpha = context.globalAlpha;
            context.globalAlpha = this.opacity;
        }		
		context.rect(-this.w/2, -this.h/2, this.w, this.h);					
		
		if(this.line_width){
			context.lineWidth = this.line_width;			
		}		
		if(this.fill){
			context.fillStyle = this.fill;
			context.fill();
		}		
		if(this.stroke){
			context.strokeStyle = this.stroke;
			context.stroke();
		}		
		if (this.opacity != null)
        {
            context.globalAlpha = old_alpha;
        }			
		context.restore();
		context.lineWidth = old_line_width;		
        $super(context);
	}
});


ms.ui.Circle = Class.create(ms.ui.Component,
{	
	initialize: function($super, fill, stroke, line_width, manager, parent)
	{				
		$super(manager, parent);
		this.fill = fill;
		this.stroke = stroke;
		this.line_width = line_width;		
	},	
	set_radius: function( value )
	{					
		this.w = 2 * value;
		this.h = 2 * value;
		this.invalidate();
	},	
	get_radius: function()
	{					
		return this.w / 2;
	},
	set_fill: function(value)
	{
		this.fill = value;
		this.invalidate();
	},
	set_stroke: function(value)
	{
		this.stroke = value;
		this.invalidate();
	},
	set_line_width: function(value)
	{
		this.line_width = value;
		this.invalidate();
	},
	draw: function($super, context, x, y)
	{	
		old_line_width = context.lineWidth;
		context.beginPath();
		if (this.opacity != null)
        {
            old_alpha = context.globalAlpha;
            context.globalAlpha = this.opacity;
        }		
		context.arc(this.x + x + this.w/2, this.y + y + this.h/2, this.w/2, 0, Math.PI*2, true); 				
		if(this.line_width){
			context.lineWidth = this.line_width;			
		}		
		if(this.fill){
			context.fillStyle = this.fill;
			context.fill();
		}		
		if(this.stroke){
			context.strokeStyle = this.stroke;
			context.stroke();
		}		
		if (this.opacity != null)
        {
            context.globalAlpha = old_alpha;
        }
		context.lineWidth = old_line_width;
		$super(context);
	}
});

ms.ui.Polygon = Class.create(ms.ui.Component,
{	
	initialize: function($super, fill, stroke, line_width, manager, parent)
	{				
		$super(manager, parent);
		this.fill = fill;
		this.stroke = stroke;
		this.line_width = line_width;
		this.points = [];
		this.x = window.innerWidth;
		this.y = window.innerHeight;
		this.w = 0;
		this.h = 0;
	},
	set_fill: function(value)
	{
		this.fill = value;
		this.invalidate();
	},
	set_stroke: function(value)
	{
		this.stroke = value;
		this.invalidate();
	},
	set_line_width: function(value)
	{
		this.line_width = value;
		this.invalidate();
	},
	setPointbyIndex: function(index, x, y)
	{					
		for(var i = 0; i < this.points.length; i++)
        {				
			if(index == i){
				this.points[i].x = x;
				this.points[i].y = y;				
			}				
		}
		this.invalidate();
	},
	setPointbyId: function(id, x, y)
	{					
		for(var i = 0; i < this.points.length; i++)
        {				
			if(this.points[i].id == id){
				this.points[i].x = x;
				this.points[i].y = y;
			}				
		}
		this.invalidate();
	},
	draw: function($super, context, x, y)
	{		
		for(var i = 0; i < this.points.length; i++)
        {	
			var point = this.points[i];			
			if(point.x < this.x)
				this.x = point.x;
			if(point.y < this.y)
				this.y = point.y;
		}		
		for(var i = 0; i < this.points.length; i++)
        {	
			var point = this.points[i];			
			if(point.x - this.x > this.w)
				this.w = point.x - this.x;
			if(point.y - this.y > this.h)
				this.h = point.y - this.y;			
		}
		context.save();		
		offset_x = this.x + this.w/2;
		offset_y = this.y + this.h/2;
		context.translate(offset_x, offset_y);
		context.rotate(this.rotation);
		context.beginPath();	
		old_line_width = context.lineWidth;
		if (this.opacity != null)
        {
            old_alpha = context.globalAlpha;
            context.globalAlpha = this.opacity;
        }		
		for(var i = 0; i < this.points.length; i++)
        {	
			var point = this.points[i];			
			if(i<0)
				context.moveTo(point.x - offset_x, point.y - offset_y);
			context.lineTo(point.x - offset_x, point.y - offset_y);						
		}
		context.closePath();		
		if(this.line_width){
			context.lineWidth = this.line_width;			
		}		
		if(this.fill){
			context.fillStyle = this.fill;
			context.fill();
		}		
		if(this.stroke){
			context.strokeStyle = this.stroke;
			context.stroke();
		}		
		if (this.opacity != null)
        {
            context.globalAlpha = old_alpha;
        }			
		context.restore();
		context.lineWidth = old_line_width;
        $super(context);
	}
});

ms.ui.Sprite = Class.create(ms.ui.Component,
{	
	initialize: function($super, sheet, manager, parent)
	{				
		$super(manager, parent);
		this.sheet = streamer.get_resource(sheet);
		this.image = this.sheet.data;
		this.w = this.sheet.frame_width;
		this.h = this.sheet.frame_height;		
		this.frame_count = 0;		
		this.current_step = 0;
		this.loop = true;
		this.bounce = false;		
		this.anim_queue = [];		
	},
	set_animation: function(value)
	{
		for(var i = 0; i < this.sheet.animations.length; i++)
        {	
			if(this.sheet.animations[i].id == value){
				this.anim_queue.unshift(this.sheet.animations[i]);				
				return 0;
			}
		}
		throw "Unknown sheet: " + value;			
	},	
	set_loop: function(value)
	{
		if(value)
			this.bounce = false;
		this.loop = value;		
	},
	set_bounce: function(value)
	{
		if(value)
			this.loop = false;
		this.bounce = value;		
	},
	set_lock: function(value)
	{
		var temp = this.anim_queue.shift();
		temp.is_locked = true;
		this.anim_queue.unshift(temp);
	},
	draw: function($super, context, x, y)
	{	
		if(this.curr_anim)
		{
			var old_alpha;

			context.save();
			context.translate(this.x + x + this.w/2, this.y + y + this.h/2);
			context.rotate(this.rotation);

			if (this.opacity != null)
			{
				old_alpha = context.globalAlpha;
				context.globalAlpha = this.opacity;
			}		
			context.drawImage(this.image, 
						this.w * (this.frame_count + this.curr_anim.frame_col), 
						this.h * this.curr_anim.frame_row, 
						this.w, this.h, 
						- this.w/2, - this.h/2, 
						this.w, this.h);
			
			if (this.opacity != null)
			{
				context.globalAlpha = old_alpha;
			}				
			context.restore();			
			$super(context);				
		}
		this.animate();
	},
	animate: function()
	{	
		if(!this.curr_anim)
		{
			if(this.anim_queue.length != 0)
			{
				this.curr_anim = this.anim_queue.pop();
				this.frame_count = 0;
			}
		}
		if(!this.curr_anim.is_locked && this.anim_queue.length != 0){
			var temp_anim = this.anim_queue.pop();
			if(temp_anim != this.curr_anim)
			{
				this.frame_count = 0;
				this.curr_anim = temp_anim;
			}
		}						
		if(this.current_step < this.curr_anim.step){
			this.current_step++;			
		}else{			
			if(this.frame_count > 0 && this.decrement)
				{ this.frame_count--; }			
			else if(this.frame_count < this.curr_anim.frames - 1 && !this.decrement) 
				{ this.frame_count++; } 
			else{				
				if(this.bounce)
				{
					if(this.frame_count == this.curr_anim.frames - 1)
					{
						this.decrement = true;						
					}
					if(this.frame_count == 0)
					{
						this.decrement = false;
						this.curr_anim.is_locked = false;
					}
				}else
					this.curr_anim.is_locked = false;
				if(this.loop)
					{ this.frame_count = 0; } 
			}
			this.current_step = 0;			
		}
		this.invalidate();
	},	
});

ms.ui.Sound = Class.create(
{
	play: function(src)
	{
		var audioElement = 	document.createElement('audio');
		audioElement.volume = 0.5;
		audioElement.setAttribute('src', src);	
		audioElement.play();
	},
});




