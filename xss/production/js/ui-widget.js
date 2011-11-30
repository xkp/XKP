"ui.widget".namespace();

ui.widget.Manager = Class.create(
{
	initialize: function(client, streamer)
	{
		this.streamer = streamer;

	    //setup our components
	    this.root = new ui.widget.Component(this);
	    this.root.rect(0, 0, client.width, client.height);

	    //mouse handling
	    this.mouse_over 	= this.root;
	    this.dragging 		= false;
		this.mouse_pressed	= false;
	    this.lastx 			= 0;
	    this.lasty 			= 0;
				
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
	    if ('keydown' in application)
	    {
			application.keydown(keycode);
	    }	    
	},
	
	keyup: function(keycode)
	{
	    if ('keyup' in application)
	    {
	        application.keyup(keycode);
	    }	    
	},
	
	keypress: function(keycode)
	{
	    if ('keypress' in application)
	    {
	        application.keypress(keycode);
	    }	    
	},
	
	mousemove: function(x, y)
	{
		if(this.mouse_pressed)
			this.dragging = true;
	    if (this.dragging)
	    {
			if ('drag' in this.mouse_over)
				this.mouse_over.drag(x , y);
	        this.lastx = x;
	        this.lasty = y;
	        return true;
	    }

	    var over = this.root.find(x, y);
	    if (!over)
	        over = this.root;

	    if (over != this.mouse_over)
	    {
	    	if ('mouseout' in this.mouse_over)
	            this.mouse_over.mouseout(this.mouse_over);

	        if ('mousein' in over)
	        {
	            over.mousein( over );
	        }

	        this.mouse_over = over;
	    }

	    if ('mousemove' in over)
	        over.mousemove(x, y, over);

	    this.lastx = x;
	    this.lasty = y;

	    return this.mouse_over != this.root;
	},

	mousedown: function(x, y)
	{
	    if ('mousedown' in this.mouse_over)
	    {
	        this.mouse_over.mouse_down(x, y);
	    }
		
	    this.mouse_pressed = true;
	    return this.mouse_over != this.root;
	},
	
	mouseup: function(x, y)
	{
		this.mouse_pressed = false;
	    if (this.dragging)
	    {
	        this.dragging = false;
	        if ('dragend' in this.mouse_over)
	            this.mouse_over.dragend(x, y);	
			return true;
	    }

	    if ('mouseup' in this.mouse_over)
	    {
	        this.mouse_over.mouse_up(x, y);
	    }
	    else if ('click' in this.mouse_over)
	    {
	        this.mouse_over.click();
	    }
	    return this.mouse_over != this.root;
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

ui.widget.Component = Class.create(
{
	initialize: function(manager, parent)
	{
		if(manager == null) manager = g_ui_root.manager;
		if(parent == null) parent = g_ui_root;
		
		this.manager    = manager;
		this.components = [];
		this.parent 	= parent;		
		
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
            return 1.0;
        return this.opacity;
    },

    alpha: function(value)
    {
        if (value < 0)
            value = 0;

        this.opacity = value;
        this.invalidate();
    },
	
	set_rotation: function(value)
    {
        this.rotation = value;
        this.positioned();
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
        if (this.moved)
        {
            this.moved(this);
        }

        this.invalidate()
    },
	
    resized: function() 
    {
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
			
		var ev_x = x - (this.x + this.w/2) - 8;
		var ev_y = y - (this.y + this.h/2) - 8;
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

ui.widget.TextView = Class.create(ui.widget.Component,
{	
	initialize: function($super, manager, parent)
	{
		$super(manager, parent);
	},
	draw: function(context, x, y)
    {
		
    },	
});



		