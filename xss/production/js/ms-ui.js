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
	    this.dragging 	= false;
	    this.lastx 		= 0;
	    this.lasty 		= 0;
		
		//keyboard handling
		this.keyb_ev = this.root;

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
	
	//Mouse managemet
	drag: function()
	{
	    this.dragging = true;
	},

	mousemove: function(x, y)
	{
	    if (this.dragging)
	    {
	        this.mouse_over.drag(x - this.lastx, y - this.lasty);
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

	    return this.mouse_over != this.root;
	},
	
	mouseup: function(x, y)
	{
	    if (this.dragging)
	    {
	        this.dragging = false;
	        if ('drag_end' in this.mouse_over)
	            this.mouse_over.drag_end(x, y);
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

ms.ui.Component = Class.create(
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
        return this.opacity;
    },

    alpha: function(value)
    {
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

	positioned: function() {this.invalidate()},
	resized: 	function() {this.invalidate()},
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
        for(var i = 0; i < this.components.length; i++)
        {
            var cmp = this.components[i];
            if (cmp.isVisible())
            {
                cmp.draw(context, this.x + x, this.y + y);
            }
        }
    },	
});

ms.ui.Image = Class.create(ms.ui.Component,
{	
	initialize: function($super, image, manager, parent)
	{				
		$super(manager, parent);		
		var texture = streamer.get_resource(image).data;
		if (image && !texture && image != 'null')
			throw "Image " + image + " not loaded";		
	    this.texture = texture;		
	},

	resized: function()
	{
        this.invalidate();
	},

	fill: function(f)
	{
        this.fill_ = f;
        this.invalidate();
	},

    src: function(image)
    {
	    this.texture = streamer.get_resource(image).data;
        this.invalidate();
    },

	image: function(texture)
	{
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

	resized: function()
	{
		var ww = this.w*(this.percent/100);
	    this.left.rect(0, 0, ww, this.h);
	    this.right.rect(ww, 0, this.w - ww, this.h);
	}
});

ms.ui.Label = Class.create(ms.ui.Component,
{
	initialize: function($super, font, manager, parent)
	{
		$super(manager, parent);
	    this.font = font;
	    this.value = "";
	},

	text: function(text)
	{
		this.value = text;
		this.manager.invalidate_all(); //td: do extents!!!
	},

	resized: function()
	{
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
        context.fillStyle = 'black'; //td:
        context.fillText(this.value, -this.w/2, -this.h/2);
		
		context.restore(); 
        $super(context);
    },
});

ms.ui.Button = Class.create(ms.ui.Component,
{
	initialize: function($super, normal, over, manager, parent)
	{
		$super(manager, parent);
		this.image = new ms.ui.Image( normal, this.manager, this.parent );		
		
		var normal_texture = streamer.get_resource(normal).data;
		var over_texture   = streamer.get_resource(over).data;	
		
		this.mousein = function()
	    {
	        this.image.image(over_texture);
	    }

	    this.mouseout = function()
	    {
	        this.image.image(normal_texture);
	    }		
	},
	positioned: function()
	{
		this.image.rotation = this.rotation;
	    this.image.rect(this.x, this.y, this.w, this.h);
	},
});

ms.ui.StateButton = Class.create(ms.ui.Component,
{
	initialize: function($super, up, down, manager, parent)
	{
		$super(manager, parent);
		this.image = new ms.ui.Image(up, this.manager, this.parent);		
		
		var up_texture 		= streamer.get_resource(up).data;
		var down_texture   	= streamer.get_resource(down).data;
		
		this.down = false;		
		this.click = function()
	    {
	        this.down  = !this.down;
			this.image.image(this.down? down_texture : up_texture);
	    }	    	
	},
	positioned: function()
	{
		this.image.rotation = this.rotation;
	    this.image.rect(this.x, this.y, this.w, this.h);
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

    draw: function($super, context, x, y)
    {
		
        context.fillStyle = this.fillStyle;
        context.beginPath();
        context.lineWidth = this.lineWidth;
        context.moveTo(x + this.x1, y + this.y1);
        context.lineTo(x + this.x2, y + this.y2);
        context.stroke();

		$super(context);
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


		