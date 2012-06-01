var ui = {};

ui.Manager = Class.extend(
{
	init: function(client, streamer)
	{
		this.streamer = streamer;
		
	    //setup our components
	    this.root = new ui.Component(this);
	    this.root.rect(0, 0, client.width, client.height);

	    //mouse handling
	    this.mouse_over = this.root;
	    this.dragging 		= false;
		mouse_pressed	= false;
	    this.lastx 		= 0;
	    this.lasty 		= 0;
		this.events = new ev.EventHolder();
		this.drag = false;
		this.over_drag = false;
		this.dragend = false;
		this.over_dragend = false;
	},		
	
	//Resource handling
	load_resources: function(callback)
	{
		var resources = [];
		if(this.streamer)
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
	    this.events.dispatch("keydown", [keycode]);    
	},

	keyup: function(keycode)
	{
		for(var i = 0; i < this.root.components.length; i++)
        {			
            var cmp = this.root.components[i];	
			if(cmp.physics)
				cmp.physics.SetAwake(true);
        }	
	    this.events.dispatch("keyup", [keycode]);    
	},

	keypress: function(keycode)
	{
		for(var i = 0; i < this.root.components.length; i++)
        {			
            var cmp = this.root.components[i];	
			if(cmp.physics)
				cmp.physics.SetAwake(true);
        }	
	    this.events.dispatch("keypress", [keycode]);    
	},	
	
	//Mouse management
	mousemove: function(x, y)
	{		
		if(this.mouse_over.physics){
			this.mouse_over.physics.SetAwake(true);
			if(this.mouse_over.physics.mouse_joint)
				this.create_mouse_joint(x, y, this.mouse_over.physics);			
		}	
	    if(mouse_pressed)
			this.dragging = true;
	    if (this.dragging)
	    {	
			this.drag = this.events.dispatch("drag", [x, y]);
			this.over_drag = this.mouse_over.events.dispatch("drag", [x, y]);
	        this.lastx = x;
	        this.lasty = y;	        
	    }
	    var over = this.root.find(x, y);
	    if (!over)
	        over = this.root;
	    if (over != this.mouse_over)
	    {
	    	this.mouse_over.events.dispatch("mouseout", [x, y, this.mouse_over]);
	        over.events.dispatch("mousein", [x, y, over]);
	        this.mouse_over = over;
	    }	    
		if(!this.over_drag)
			over.events.dispatch("mousemove", [x, y, over]);
		if(!this.drag)
			this.events.dispatch("mousemove", [x, y]);
	    this.lastx = x;
	    this.lasty = y;
	    return this.mouse_over != this.root;
	},
	
	mousedown: function(x, y)
	{		
		if(this.mouse_over.physics)
			this.mouse_over.physics.SetAwake(true);
		
	    this.mouse_over.events.dispatch("mousedown", [x, y]);
	    
		mouse_pressed = true;
		
		this.events.dispatch("mousedown", [x, y]);
	    return this.mouse_over != this.root;
	},

	mouseup: function(x, y)
	{						
		if(this.mouse_over.physics)
			this.mouse_over.physics.SetAwake(true);	
		mouse_pressed = false;
	    if (this.dragging)
	    {
	        this.dragging = false;
			this.dragend = this.events.dispatch("dragend", [x, y]);
	        this.over_dragend = this.mouse_over.events.dispatch("dragend", [x, y]);	        
	    }	    
		if(!this.over_drag && !this.over_dragend)
		{
			this.mouse_over.events.dispatch("mouseup", [x, y]);	    	    
			this.mouse_over.events.dispatch("click", [x, y]);	  
		}
		if(!this.drag && !this.dragend)
		{
			this.events.dispatch("mouseup", [x, y]);
			this.events.dispatch("click", [x, y]);
		}
		this.drag = this.over_drag = this.dragend = this.over_dragend = false;		
	    return this.mouse_over != this.root;		
	},
	
	create_mouse_joint: function(x, y, body){
		var handledMouseX = x  / 30;
		var handledMouseY = y  / 30;
		if(mouse_pressed && (!this.mouseJoint)) {
			var md = new b2MouseJointDef();
            md.bodyA = g_world.GetGroundBody();
            md.bodyB = body;
            md.target.Set(handledMouseX, handledMouseY);
            md.collideConnected = true;
            md.maxForce = 300.0 * body.GetMass();
            this.mouseJoint = g_world.CreateJoint(md);
        }
        if(this.mouseJoint) {
           if(mouse_pressed) {			
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
		if(context.canvas.width != context.canvas.offsetWidth)
			context.canvas.width = context.canvas.offsetWidth;
		if(context.canvas.height != context.canvas.offsetHeight)
			context.canvas.height = context.canvas.offsetHeight;
		if(context.canvas.width != this.root.get_width())
			this.root.set_width(context.canvas.width);
		if(context.canvas.height != this.root.get_height())
			this.root.set_height(context.canvas.height);
        if(this.redraw_)
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

ui.Component = Class.extend(
{
	init: function(manager, parent)
	{
		this.manager    = manager;
		this.components = [];
		this.parent 	= parent;	
		this.events = new ev.EventHolder();
		this.visible_ = true;
		this.switchable = true;
		this.offset_right = 0;
		this.offset_left = 0;
		this.offset_top = 0;
		this.offset_bottom = 0;

		this.x 	= 0; 
        this.y 	= 0; 
        this.w 	= this.orig_w	= 0;
        this.h 	= this.orig_h	= 0;
		this.rotation 			= 0;
        this.placement  = "";
		if(this.parent)
		{	
			if(this.parent.scaleX)
				this.scaleX = this.parent.scaleX;
			if(this.parent.scaleY)
				this.scaleY = this.parent.scaleY;
		}
		else
		{
			this.scaleX = 1;
			this.scaleY = 1;			
		}
		
		if(parent)
			this.opacity = parent.opacity; 
		else
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
		for(var i = 0; i < this.components.length; i++)
        {					
            var cmp = this.components[i];
			cmp.alpha(value);            
        }
        this.invalidate();
    },

	set_placement: function(value)
	{
		this.placement = value;		
		this.invalidate();
	},
	
	set_switchable: function(value)
	{
		this.switchable = value;	
		if(!value)
			this.hide();	
		this.parent.active(this.parent.components.indexOf(this));
	},
    update_placement: function()
    {		
		if(this.parent)
		{			
			if(this.orig_w > this.parent.w - this.parent.offset_right - this.parent.offset_left)
				this.orig_w = this.parent.w - this.parent.offset_right - this.parent.offset_left;
			if(this.orig_h > this.parent.h - this.parent.offset_top - this.parent.offset_bottom)
				this.orig_h = this.parent.h - this.parent.offset_top - this.parent.offset_bottom;
		}
        if (this.placement == "client")
        {
            this.client(); 
        }    
        else if (this.placement == "center")
        {
            if (this.orig_w != 0 && this.orig_h != 0)
                this.center(this.orig_w, this.orig_h);                       
        }
        else if (this.placement == "bottom")
        {
            if (this.orig_h != 0)
                this.bottom(this.orig_h);            
        }
        else if (this.placement == "top")
        {
            if (this.orig_h != 0)
				this.top(this.orig_h);            
        }
        else if (this.placement == "right")
        {
            if (this.orig_w != 0)
                this.right(this.orig_w);            
        }
        else if (this.placement == "left")
        {
            if (this.orig_w != 0)
                this.left(this.orig_w);            
        }
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
		this.orig_w = value;
        this.size(value, this.h);
    },

    set_height: function(value)
    {
		this.orig_h = value;
        this.size(this.w, value);
    },

	client: function()
	{
	this.rect(this.parent.offset_left, 
				this.parent.offset_top, 
				this.parent.w - this.parent.offset_right - this.parent.offset_left, 
				this.parent.h - this.parent.offset_top - this.parent.offset_bottom);
		this.parent.offset_left += this.parent.w - this.parent.offset_right;
		this.parent.offset_top += this.parent.h - this.parent.offset_bottom;
	},

	center: function(w, h)
	{
		//TD
		this.rect(this.parent.w/2 - w/2, this.parent.h/2 - h/2, w, h);
	},

	bottom: function(h)
	{

		this.rect(this.parent.offset_left, 
				this.parent.h - h - this.parent.offset_bottom, 
				this.parent.w - this.parent.offset_right - this.parent.offset_left, 
				h);
		this.parent.offset_bottom += h;
	},

	top: function(h)
	{
		this.rect(this.parent.offset_left, 
				this.parent.offset_top, 
				this.parent.w - this.parent.offset_right - this.parent.offset_left, 
				h);
		this.parent.offset_top += h;
	},

	right: function(w)
	{
		this.rect(this.parent.w - w - this.parent.offset_right, 
				this.parent.offset_top, 
				w, 
				this.parent.h - this.parent.offset_top - this.parent.offset_bottom);
		this.parent.offset_right += w;
	},

	left: function(w)
	{
		this.rect(this.parent.offset_left, 
				this.parent.offset_top, 
				w, 
				this.parent.h - this.parent.offset_top - this.parent.offset_bottom);
		this.parent.offset_left += w;
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

	set_scale: function(factor)
	{
		this.scaleX = factor;
		this.scaleY = factor;
this.set_x(this.x + this.w/2 - (this.orig_w * this.scaleX)/2);
		this.set_y(this.y + this.h/2 - (this.orig_h * this.scaleY)/2);
		this.size(this.orig_w * this.scaleX, this.orig_h * this.scaleY);		
		for(var i = 0; i < this.components.length; i++)
        {					
            var cmp = this.components[i];
			cmp.set_scale(factor);            
        }
		this.invalidate();
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
		this.offset_right = 0;
		this.offset_left = 0;
		this.offset_top = 0;
		this.offset_bottom = 0;
		this.update_placement();
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

ui.Image = ui.Component.extend(
{	
	init: function(manager, parent, image)
	{				
		this._super(manager, parent);
        
        var resource = this.manager.streamer.get_resource(image);
        
        if (!resource)
		{
			console.warn("Unknow resource: " + image);
            resource = this.manager.streamer.get_resource("invalid_res");
		}            
         		
		this.texture = resource.data;
		this.tile = false;
		this.iwidth;
		this.iheight;
	},

	fill: function(f)
	{
        this.fill_ = f;
        this.invalidate();
	},
	
	set_iwidth: function(v)
	{
		this.tile = true;
		this.iwidth = v;	
	},
	
	set_iheight: function(v)
	{
		this.tile = true;
		this.iheight = v;		
	},

    src: function()
    {
        return this.source;
    },

    src: function(image)
    {
        this.source = image;
	    this.texture = this.manager.streamer.get_resource(image).data;
        this.invalidate();
    },

	image: function(texture)
	{
        this.source = "";
	    this.texture = texture;
        this.invalidate();
	},     
	
	draw: function(context, x, y)
    {			
		if(!this.iwidth)
			this.iwidth = this.w;
		if(!this.iheight)
			this.iheight = this.h;
			
		var old_alpha;
		if (this.opacity != null)
        {
            old_alpha = context.globalAlpha;
            context.globalAlpha = this.opacity;
        }	
		if(this.tile)
		{
			var ix = - this.w/2;
			var iy = - this.w/2;
			context.save();
			context.translate(this.x + x + this.w/2, this.y + y + this.h/2);
			context.rotate(this.rotation);	
			context.beginPath();					
			context.rect(- this.w/2, - this.h/2, this.w, this.h);	
			context.clip();			
			while(- this.w/2 + this.w >= ix)
			{
				iy = - this.h/2;
				while(- this.h/2 + this.h >= iy)
				{
					context.drawImage(this.texture, ix, iy, this.iwidth, this.iheight);
					iy += this.iheight;
				}
				ix += this.iwidth;
			}
			context.restore();
		}
		else
		{
			context.save();
			context.translate(this.x + x + this.w/2, this.y + y + this.h/2);
			context.rotate(this.rotation);					
			if (this.texture)
			{				
				context.drawImage(this.texture, - this.w/2, - this.h/2, this.w, this.h);
			}
			else if (this.fill_)
			{
				context.fillStyle = this.fill_;
				context.fillRect(-this.w/2, -this.h/2, this.w, this.h);
			}
			context.restore();  
		}
		
        if (this.opacity != null)
        {
            context.globalAlpha = old_alpha;
        }		
		
        this._super(context);
    }
});

ui.ProgressBar = ui.Component.extend(
{
	init: function(manager, parent, images)
	{
		this._super(manager, parent);
		if (!images)
		{
			images = ['images/progress.empty.png', 'images/progress.full.png'];
		}		
		this.left    = new ui.Image(this.manager, this, images[1]);
		this.right   = new ui.Image(this.manager, this, images[0]);
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

	resized: function()
	{
		var ww = this.w*(this.percent/100);
	    this.left.rect(0, 0, ww, this.h);
	    this.right.rect(ww, 0, this.w - ww, this.h);
		this._super();
	}
});

ui.Label = ui.Component.extend(
{
	init: function(manager, parent, font)
	{
		this._super(manager, parent);
	    this.font = font;
	    this.value = "";
		this.color_value = 'black';
	},

	text: function(text)
	{
		this.value = text;
		this.invalidate(); //td: do extents!!!
	},
	
	set_color: function(value)
	{
		if(value != null)
		{
			this.color_value = value;	
			this.invalidate();
		}
	},

	extents: function()
	{
        //td:
	},

    draw: function(context, x, y)
    {	
		context.save();		
		if (this.opacity != null)
        {
            old_alpha = context.globalAlpha;
            context.globalAlpha = this.opacity;
        }
		var measure = measureText(this.value, this.font);
		this.w = measure.width;
		this.h = measure.height;			
		context.translate(this.x + x + this.w/2, this.y + y + this.h/2);
		context.rotate(this.rotation);
        context.font = this.font;				
		context.fillStyle = this.color_value; 	
        context.fillText(this.value, -this.w/2, -this.h/2 + this.h);		
		if (this.opacity != null)
        {
            context.globalAlpha = old_alpha;
        }		
		context.restore(); 
        this._super(context);
    },
});

ui.Button = ui.Image.extend(
{
	init: function(manager, parent, normal, over)
	{
		this._super(manager, parent, normal);
		if(!this.manager.streamer.get_resource(normal))
			normal = "invalid_res";
		if(!this.manager.streamer.get_resource(over))
			over = "invalid_res";
		var normal_texture = this.manager.streamer.get_resource(normal).data;
		var over_texture   = this.manager.streamer.get_resource(over).data;	
		var this__ = this;
		this.events.addListener('mousein', function()
		{
			this__.image(over_texture);
		});	
		this.events.addListener('mouseout', function()
		{
			this__.image(normal_texture);
		});
				
	},	
});

ui.TextButton = ui.Component.extend(
{
	init: function(manager, parent, text, font)
	{
		this._super(manager, parent);
		this.mouse_into = false;
		this.clicked = false;
		var this__ = this;
		this.events.addListener('mousein', function()
		{
			this__.mouse_into = true;
			this__.invalidate();
		});	
		this.events.addListener('mouseout', function()
		{
			this__.mouse_into = false;
			this__.invalidate();
		});
		this.events.addListener('mousedown', function()
		{
			this__.clicked = true;
			this__.invalidate();
			this__.x ++;
			this__.y ++;
		});	
		this.events.addListener('mouseup', function()
		{
			this__.clicked = false;
			this__.invalidate();
			this__.x --;
			this__.y --;
		});
		this.text = text;	
		this.font = font;
	},	
	set_text: function(value)
	{
		this.text = value;
		this.invalidate();
	},	
	set_font: function(value)
	{
		this.font = value;
		this.invalidate();
	},
	round_rect: function(ctx, x, y, width, height, radius, fill, stroke) {
		if (typeof radius === "undefined") {
			radius = 5;
		}
		ctx.beginPath();
		ctx.moveTo(x + radius, y);
		ctx.lineTo(x + width - radius, y);
		ctx.quadraticCurveTo(x + width, y, x + width, y + radius);
		ctx.lineTo(x + width, y + height - radius);
		ctx.quadraticCurveTo(x + width, y + height, x + width - radius, y + height);
		ctx.lineTo(x + radius, y + height);
		ctx.quadraticCurveTo(x, y + height, x, y + height - radius);
		ctx.lineTo(x, y + radius);
		ctx.quadraticCurveTo(x, y, x + radius, y);
		ctx.closePath();
		if (stroke) {
			ctx.strokeStyle = stroke;
			ctx.stroke();
		}
		if (fill) {
			ctx.fillStyle = fill;
			ctx.fill();
		}        
	},
	draw: function(context, x, y)
    {
		var gradient;
		context.save();		
		if (this.opacity != null)
        {
            old_alpha = context.globalAlpha;
            context.globalAlpha = this.opacity;
        }
		var measure = measureText(this.text, this.font);
		this.w = measure.width + 20;
		this.h = measure.height + 20;			
		context.translate(this.x + x + this.w/2, this.y + y + this.h/2);
		context.rotate(this.rotation);
        context.font = this.font;
		gradient = context.createLinearGradient(0, -this.h/2, 
												0, -this.h/2 + this.w);
		if(!this.mouse_into)
		{			
			gradient.addColorStop(0.0, '#F3F8FE');
			gradient.addColorStop(1.0, '#DCE6F4');
		}
		else
		if(this.clicked)
		{
			gradient.addColorStop(0.0, '#C9D4E4');
			gradient.addColorStop(0.2, '#E1EBF5');
			gradient.addColorStop(1.0, '#CFDBED');			
		}else
		{
			gradient.addColorStop(0.0, '#FFFFFF');			
			gradient.addColorStop(1.0, '#BED2E6');			
		}
		this.round_rect(context, -this.w/2, -this.h/2, this.w, 
						this.h, 5, gradient, '#AABCD4');
		context.fillStyle = '#693978'; 		
        context.fillText(this.text, -this.w/2 + 10, -this.h/2 + this.h - 10);		
		if (this.opacity != null)
        {
            context.globalAlpha = old_alpha;
        }
		context.restore(); 
        this._super(context);		
	},
});

ui.RippleEffect = ui.Component.extend(
{
	init : function(manager, parent, image)
	{
		this._super(manager, parent);
		if(image)
		{
			var resource = this.manager.streamer.get_resource(image);
			this.img = resource.data;
		}
        this.first_run = false;		
		this.riprad = 3;		
		this.mapind;		
		this.ripplemap = [];
		this.last_map = [];
		this.ripple;
		this.ripple_data;
		this.texture;
		this.texture_data;
		this.is_running = true;	
		this.is_disturbed = false;						
		this.canvas = document.createElement('canvas');		
		this.context = this.canvas.getContext('2d');
		this.mouse_thru = true;
	},
	set_ripradius: function(value)
	{
		if(value != null)
		 this.riprad = value;
	},
	get_ripradius: function()
	{
		return this.riprad;
	},
	draw: function(context, x, y)
    {	
		this.canvas.width = this.w;
		this.canvas.height = this.h;
		if (this.is_disturbed) {
				this.newframe();
				var old_alpha = context.globalAlpha;
				context.globalAlpha = this.opacity;        
				this.context.putImageData(this.ripple, this.x, this.y);	
				context.drawImage(this.canvas, this.x, this.y, this.w, this.h);				
				context.globalAlpha = old_alpha;
				this.invalidate();
		}
		else{
			if(!this.first_run)
			{
				this.oldind = this.w;
				this.newind = this.w * (this.h + 3);	
				this.size = this.w * (this.h + 2) * 2;
				this.half_width = this.w >> 1;
				this.half_height = this.h >> 1;	
			}
			var old_alpha = context.globalAlpha;
			context.globalAlpha = this.opacity;
			if(this.gradient)
			{
				gradient = this.context.createLinearGradient(this.gradient.x1, this.gradient.y1, 
															 this.gradient.x2, this.gradient.y2);
				for (var i = 0; i < this.gradient.colors.length; i++) {
					gradient.addColorStop(this.gradient.colors[i].offset, this.gradient.colors[i].color);
				}				
				this.context.fillStyle = gradient;
				this.context.fillRect(this.x, this.y, this.w, this.h);
			}
			else if(this.img)
				this.context.drawImage(this.img, this.x, this.y, this.w, this.h);			
			context.drawImage(this.canvas, this.x, this.y, this.w, this.h);
			context.globalAlpha = old_alpha;
			if(!this.first_run)
			{
				for(var i = 0; i < this.size; i++) {
					this.last_map[i] = this.ripplemap[i] = 0;
				}
				this.texture = this.context.getImageData(this.x, this.y, this.w, this.h);
				this.texture_data = this.texture.data;
				this.ripple = this.context.getImageData(this.x, this.y, this.w, this.h);
				this.ripple_data = this.ripple.data;
			}
			this.first_run = true;
		}				
        this._super(context);		
    },	
	disturb: function(dx, dy)
	{		
			dx -= 2*this.x;
			dy -= 2*this.y;
			dx <<= 0;
			dy <<= 0;
			this.is_disturbed = true;
			
			for (var j = dy - this.riprad; j < dy + this.riprad; j++) {
				for (var k = dx - this.riprad; k < dx + this.riprad; k++) {
					this.ripplemap[this.oldind + (j * this.w) + k] += 512;
				}
			}
			this.invalidate();		
	},
	newframe: function() 
	{
		var i, a, b, data, cur_pixel, new_pixel, old_data;
		
		i = this.oldind;
		this.oldind = this.newind;
		this.newind = i;
		
		i = 0;
		this.mapind = this.oldind;
				
		var _width = this.w,
			_height = this.h,
			_ripplemap = this.ripplemap,
			_mapind = this.mapind,
			_newind = this.newind,
			_last_map = this.last_map,
			_rd = this.ripple.data,
			_td = this.texture.data,
			_half_width = this.half_width,
			_half_height = this.half_height,
			_is_disturbed = false;
		
		for (var y = 0; y < _height; y++) {
			for (var x = 0; x < _width; x++) {
				data = (
					_ripplemap[_mapind - _width] + 
					_ripplemap[_mapind + _width] + 
					_ripplemap[_mapind - 1] + 
					_ripplemap[_mapind + 1]) >> 1;
					
				data -= _ripplemap[_newind + i];
				data -= data >> 5;
				
				_ripplemap[_newind + i] = data;

				//where data=0 then still, where data>0 then wave
				data = 1024 - data;
				
				old_data = _last_map[i];
				_last_map[i] = data;
				
				if (old_data != data) {
					//offsets
					_is_disturbed = true;
					a = (((x - _half_width) * data / 1024) << 0) + _half_width;
					b = (((y - _half_height) * data / 1024) << 0) + _half_height;
	
					//bounds check
					if (a >= _width) a = _width - 1;
					if (a < 0) a = 0;
					if (b >= _height) b = _height - 1;
					if (b < 0) b = 0;
	
					new_pixel = (a + (b * _width)) * 4;
					cur_pixel = i * 4;
					
					_rd[cur_pixel] = _td[new_pixel];
					_rd[cur_pixel + 1] = _td[new_pixel + 1];
					_rd[cur_pixel + 2] = _td[new_pixel + 2];
					//_rd[cur_pixel + 3] = _td[new_pixel + 3];
				}				
				++_mapind;
				++i;
			}
		}		
		this.mapind = _mapind;
		this.is_disturbed = _is_disturbed;
	},
});

ui.StateButton = ui.Image.extend(
{
	init: function(manager, parent, active, inactive)
	{
		this._super(manager, parent, active);
		
		if(!this.manager.streamer.get_resource(active))
			active = "invalid_res";
		if(!this.manager.streamer.get_resource(inactive))
			inactive = "invalid_res";

		this.active_texture 		= this.manager.streamer.get_resource(active).data;
		this.inactive_texture   	= this.manager.streamer.get_resource(inactive).data;

		this.is_active = true;		
		var this__ = this;		
		this.events.addListener('click', function()
		{		
			this__.is_active  = !this__.is_active;
			this__.image(this__.is_active? this__.active_texture : this__.inactive_texture);
		});		
	},	
	set_active: function(value)
	{
		this.is_active = value;
		this.image(this.is_active? this.active_texture : this.inactive_texture);
	},
});

ui.Switch = ui.Component.extend(
{
	temp_active: 0,
	active_: 0,

	componentAdded: function(cmp)
	{
        cmp.rect(0, 0, this.w, this.h);

		if (this.components.length == (this.active_ + 1) && cmp.switchable)
			cmp.show();
		else
			cmp.hide();
	},
	
	set_width: function(value)
	{
		this.w = value;
		for (var i = 0; i < this.components.length; i++) {
			this.components[i].set_width(value);
		}
	},
	
	set_height: function(value)
	{
		this.h = value;
		for (var i = 0; i < this.components.length; i++) {
			this.components[i].set_height(value);
		}
	},

	active: function( idx )
	{
		this.temp_active = this.active_;
		if(idx >= this.active_)

			this.move_to_next( idx );
		if(idx <= this.active_)
			this.move_to_prev( idx );		
	},
	
	move_to_next: function( idx )
	{				
		if(this.components[idx].switchable)
		{
			this.components[this.active_].hide();		

			this.active_ = idx;

			this.components[this.active_].show();
			this.invalidate();
		}else
		{	
			if(idx + 1 == this.components.length)
				return
			if(idx + 1 != this.temp_active)
				this.move_to_next( idx + 1 );
			else
				return;
		}
	},
	
	move_to_prev: function( idx )
	{				
		if(this.components[idx].switchable)
		{
			this.components[this.active_].hide();		
			this.active_ = idx;
			this.components[this.active_].show();
			this.invalidate();
		}else
		{	
			if(idx - 1 == -1)
				return
			if(idx - 1 != this.temp_active)
				this.move_to_prev( idx - 1 );
			else
				return;
		}
	},

    get_active: function()
    {
        return this.active_;
    },
});

ui.Line = ui.Component.extend(
{
	init: function(manager, parent)
	{
		this._super(manager, parent);
	    this.x1         = 0;
	    this.y1         = 0;
	    this.x2         = 0;
	    this.y2         = 0;
        this.lineWidth  = 2;
        this.lineCap    = '';
        this.color_value = 'black';
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
	
	set_color: function(v) 
    {
		if(v != null)
		{
			this.color_value = v;
			this.invalidate();
		}
    },

    set_lineCap: function(v) 
    {
        this.lineCap = v;
        this.invalidate();
    },

    positioned: function()
    {
        this._super();
	    this.x1 = this.x;
	    this.y1 = this.y;
	    this.x2 = this.x + this.w;
	    this.y2 = this.y + this.h;
    },

    resized: function()
    {
        this._super();
	    this.x2 = this.x + this.w;
	    this.y2 = this.y + this.h;
    },

    draw: function(context, x, y)
    {
		if (this.opacity != null)
        {
            old_alpha = context.globalAlpha;
            context.globalAlpha = this.opacity;
        }		
		old_line_width = context.lineWidth;         
        context.lineWidth = this.lineWidth;		
		context.beginPath();
        context.moveTo(x + this.x1, y + this.y1);
        context.lineTo(x + this.x2, y + this.y2);
		context.closePath();
		context.strokeStyle = this.color_value;
		context.stroke();		
		context.lineWidth = old_line_width;
		if (this.opacity != null)
        {
            context.globalAlpha = old_alpha;
        }		
		this._super(context);
    },
});

ui.Painter = ui.Component.extend(
{	
	init: function(manager, parent)
	{				
		this._super(manager, parent);		
	},

	draw: function(context, x, y)
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

        this._super(context);
    }
});

ui.ClipArea = ui.Component.extend(
{	
	init: function(manager, parent, shape)
	{				
		this._super(manager, parent);
		this.clip = true;
		this.shape = shape;
	},	
	draw: function(context, x, y)
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

ui.Rectangle = ui.Component.extend(
{	
	init: function(manager, parent, fill, stroke, line_width)
	{				
		this._super(manager, parent);
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
	draw: function(context, x, y)
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
		if(this.gradient)
		{
			gradient = context.createLinearGradient(this.gradient.x1, this.gradient.y1, 
														 this.gradient.x2, this.gradient.y2);
			for (var i = 0; i < this.gradient.colors.length; i++) {
				gradient.addColorStop(this.gradient.colors[i].offset, this.gradient.colors[i].color);
			}				
			context.fillStyle = gradient;	
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
        this._super(context);
	}
});


ui.Circle = ui.Component.extend(
{	
	init: function(manager, parent, fill, stroke, line_width)
	{				
		this._super(manager, parent);
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
	draw: function(context, x, y)
	{	
		old_line_width = context.lineWidth;
		context.beginPath();
		if(this.opacity != null)
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
		if(this.opacity != null)
        {
            context.globalAlpha = old_alpha;
        }
		context.lineWidth = old_line_width;
		this._super(context);
	}
});

ui.Polygon = ui.Component.extend(
{	
	init: function(manager, parent, fill, stroke, line_width)
	{				
		this._super(manager, parent);
		this.fill = fill;
		this.stroke = stroke;
		this.line_width = line_width;
		this.points = [];
		this.x = window.innerWidth;
		this.y = window.innerHeight;
		this.w = 0;
		this.h = 0;
	},
	get_min_pos: function()
	{
		var min_pos = {x: window.innerWidth, y: window.innerHeight};
		for(var i = 0; i < this.points.length; i++)
        {	
			var point = this.points[i];			
			if(point.x < min_pos.x)
				min_pos.x = point.x;
			if(point.y < min_pos.y)
				min_pos.y = point.y;
		}
		return min_pos;		
	},
	get_max_pos: function()
	{
		var max_pos = {x: 0, y: 0};
		for(var i = 0; i < this.points.length; i++)
        {	
			var point = this.points[i];			
			if(point.x > max_pos.x)
				max_pos.x = point.x;
			if(point.y > max_pos.y)
				max_pos.y = point.y;
		}
		return max_pos;		
	},
	set_x: function(value)
	{		
		var offset_x = value - this.x
		for(var i = 0; i < this.points.length; i++)
        {				
			this.points[i].x += offset_x;						
		}
		this.invalidate();
	},	
	set_y: function(value)
	{		
		var offset_y = value - this.y
		for(var i = 0; i < this.points.length; i++)
        {				
			this.points[i].y += offset_y;						
		}
		this.invalidate();
	},
	set_width: function(value)
	{		
		var mid_w = this.x + this.w/2;
		var offset_w = value - this.w; 
		for(var i = 0; i < this.points.length; i++)
        {		
			if(this.points[i].x > mid_w)
				this.points[i].x += offset_w;						
		}
		this.invalidate();
	},	
	set_height: function(value)
	{		
		var mid_h = this.y + this.h/2;
		var offset_h = value - this.h; 
		for(var i = 0; i < this.points.length; i++)
        {		
			if(this.points[i].y > mid_h)
				this.points[i].y += offset_h;						
		}
		this.invalidate();
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
	draw: function(context, x, y)
	{		
		var min_pos = this.get_min_pos();
		this.x = min_pos.x;
		this.y = min_pos.y;
		var max_pos = this.get_max_pos();
		this.w = max_pos.x - min_pos.x;
		this.h = max_pos.y - min_pos.y;
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
        this._super(context);
	}
});

ui.Sprite = ui.Component.extend(
{	
	init: function(manager, parent)
	{				
		this._super(manager, parent);		
		this.frame_count = 0;		
		this.current_step = 0;
		this.loop = true;
		this.bounce = false;
		this.right_to_left = false;
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
	set_sheet: function(value)
	{
		this.sheet = this.manager.streamer.get_resource(value);
		this.image = this.sheet.data;
		this.w = this.sheet.frame_width;
		this.h = this.sheet.frame_height;		
	},
	set_right_to_left: function(value)
	{
		this.right_to_left = value;				
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
	draw: function(context, x, y)
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
			if(!this.curr_anim.right_to_left)			
			context.drawImage(this.image, 
						this.w * (this.frame_count + this.curr_anim.frame_col), 
						this.h * this.curr_anim.frame_row, 
						this.w, this.h, 
						- this.w/2, - this.h/2, 
						this.w, this.h);
			else
			context.drawImage(this.image, 
						this.w * (this.curr_anim.frame_col - this.frame_count), 
						this.h * this.curr_anim.frame_row, 
						this.w, this.h, 
						- this.w/2, - this.h/2, 
						this.w, this.h);
			
			if (this.opacity != null)
			{
				context.globalAlpha = old_alpha;
			}				
			context.restore();			
			this._super(context);				
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

ui.Video = ui.Component.extend(
{	
	init: function(manager, parent, src)
	{				
		this._super(manager, parent);
		var resource = this.manager.streamer.get_resource(src);
        if (!resource)
            throw "Unknow resource: " + src;         		
		this.vid = resource.data;
		this.mute_vol = 0;
		this.vid.loop = true;
	},	
	set_loop: function(value)
	{		
		this.vid.loop = value;		
	},
	draw: function(context, x, y)
	{		
		this._super(context);		
		var old_alpha;
		context.save();
		context.translate(this.x + x + this.w/2, this.y + y + this.h/2);
		context.rotate(this.rotation);
		if (this.opacity != null)
		{
			old_alpha = context.globalAlpha;
			context.globalAlpha = this.opacity;
		}
		context.drawImage(this.vid, -this.w/2, -this.h/2, this.w, this.h);
		if (this.opacity != null)
		{
			context.globalAlpha = old_alpha;
		}				
		context.restore();	
		if(!(this.vid.paused || this.vid.ended))
			this.invalidate();
		this._super(context);
	},
	play: function()
	{				
		this.vid.play();
		this.invalidate();
	},
	stop: function()
	{		
		this.vid.pause();
		this.vid.currentTime = 0;	
		this.invalidate();	
	},
	pause: function()
	{				
		this.vid.pause();
		this.invalidate();
	},
	mute: function()
	{		
		var temp_vol = this.mute_vol;
		this.mute_vol = this.vid.volume;
		this.vid.volume = temp_vol;
	}, 
});

ui.Sound = ui.Component.extend(
{
	init: function(manager, parent, src)
	{				
		this._super(manager, parent);
		var resource = this.manager.streamer.get_resource(src);
        if (!resource)
            throw "Unknow resource: " + src;         		
		this.sound = resource.data;
		this.mute_vol = 0;
		this.sound.loop = true;
	},	
	set_loop: function(value)
	{		
		this.sound.loop = value;		
	},			
	play: function()
	{			
		this.sound.play();
	},
	stop: function()
	{		
		this.sound.pause();
		this.sound.currentTime = 0;				
	},
	pause: function()
	{				
		this.sound.pause();		
	},
	mute: function()
	{		
		var temp_vol = this.mute_vol;
		this.mute_vol = this.sound.volume;
		this.sound.volume = temp_vol;
	}, 
});

ui.SoundUtils = Class.extend(
{
	play: function(src)
	{
		var audioElement = 	document.createElement('audio');
		audioElement.volume = 0.5;
		audioElement.setAttribute('src', src);	
		audioElement.play();
	},
});
ui.Replicator = ui.Component.extend(
{
	init: function(manager, parent, template)
	{			
		this._super(manager, parent);		
		this.template = template;				
	},			
});