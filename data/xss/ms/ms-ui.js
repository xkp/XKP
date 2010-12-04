
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

	    //texture, keep references to make our life easier
	    this.images = [];
	},

	//Resource handling
	load_images: function(images, callback)
	{
		var resources = [];
		for(var i = 0; i < images.length; i++)
		{
			var img = images[i];
			if (this.get_image(img) == null)
			{
				resources.push({ type: RESOURCE_IMAGE, url: img});
			}
		}

		if (resources.length > 0)
		{
			var this_ = this;
			var stream_client =
			{
				resource_loaded: function(res, data)
				{
					this_.images.push({url: res.asset, texture: data});
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

	get_image: function(image)
	{
		for(var i = 0; i < this.images.length; i++)
		{
			var img = this.images[i];
			if (img.url == image)
				return img.texture;
		}

		return null;
	},

	//Mouse managemet
	drag: function()
	{
	    this.dragging = true;
	},

	mouse_move: function(x, y)
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
	    	if ('mouse_out' in this.mouse_over)
	            this.mouse_over.mouse_out(this.mouse_over);

	        if ('mouse_in' in over)
	        {
	            over.mouse_in( over );
	        }

	        this.mouse_over = over;
	    }

	    if ('mouse_move' in over)
	        over.mouse_move(x, y, over);

	    this.lastx = x;
	    this.lasty = y;

	    return this.mouse_over != this.root;
	},

	mouse_down: function(x, y)
	{
	    if ('mouse_down' in this.mouse_over)
	    {
	        this.mouse_over.mouse_down(x, y);
	    }

	    return this.mouse_over != this.root;
	},

	mouse_up: function(x, y)
	{
	    if (this.dragging)
	    {
	        this.dragging = false;
	        if ('drag_end' in this.mouse_over)
	            this.mouse_over.drag_end(x, y);
	        return true;
	    }

	    if ('mouse_up' in this.mouse_over)
	    {
	        this.mouse_over.mouse_up(x, y);
	    }
	    else if ('click' in this.mouse_over)
	    {
	        this.mouse_over.click();
	    }

	    return this.mouse_over != this.root;
	},

	update: function(elapsed)
	{
		var anims = this.animations;
		for(var i = 0; i < anims.length; i++)
		{
			var anim = anims[i];
			if (anim.active)
				anim.update(elapsed);
		}
	},

    draw: function(context)
    {
        //td: dirt
        this.root.draw(context, 0, 0);
    }
});

ms.ui.Component = Class.create(
{
	initialize: function(manager, parent)
	{
		this.manager    = manager;
		this.components = [];
		this.parent 	= parent;

		this.x = 0; 
        this.y = 0; 
        this.w = 0;
        this.h = 0;

		if (parent)
			parent.addComponent(this);
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

	client: function()
	{
		this.rect(0, 0, this.parent.w, this.parent.h);
	},

	center: function(w, h)
	{
		this.rect((this.parent.w - w)/2, (this.parent.h - h)/2, w, h);
	},

	bottom: function(cmp, h)
	{
		this.rect(cmp.x, cmp.y + cmp.h, cmp.w, h);
	},

	top: function(cmp, h)
	{
		this.rect(cmp.x, cmp.y - h, cmp.w, h);
	},

	hide: function()
	{
		this.visible_ = false;
	},

	show: function()
	{
		this.visible_ = true;
	},

	isVisible: function()
	{
		return this.visible_;
	},

	positioned: function() {},
	resized: 	function() {},
    visible_:   true,

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
	    return (x >= this.x && x <= (this.x + this.w) &&
	            y >= this.y && y <= (this.y + this.h));
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
                cmp.draw(context, x, y);
            }
        }
    },

	//factory
	newComponent: function()
	{
		var result = new ms.ui.Component(this.manager, this);
		result.mouse_thru = true;
		return result;
	},

	newImage: function(image)
	{
		var img = this.manager.get_image(image);
		if (!img)
			throw "Image " + image + " not loaded";

		return new ms.ui.Image(this.manager, this, img);
	},

	newProgressBar: function(images)
	{
		if (!images)
		{
			images = ['/assets/ui/progress.empty.png', '/assets/ui/progress.full.png'];
		}

		return new ms.ui.ProgressBar(this.manager, this, images[0], images[1]);
	},

	newLabel: function(font)
	{
		return new ms.ui.Label(this.manager, this, font);
	},

    newButton: function(normal, over)
    {
		var result = this.newImage(normal);

		var normal_texture = this.manager.get_image(normal);
		var over_texture   = this.manager.get_image(over);

	    result.mouse_in = function()
	    {
	        this.sampler.texture = over_texture;
	    }

	    result.mouse_out = function()
	    {
	        this.sampler.texture = normal_texture;
	    }

	    return result;
    },

    newStateButton: function(up, down)
    {
		var result = this.newImage(up);

		var up_texture   = this.manager.get_image(up);
		var down_texture = this.manager.get_image(down);

		result.down  = false;
	    result.click = function()
	    {
	    	this.down 			 = !this.down;
	    	this.sampler.texture = this.down? down_texture : up_texture;

	    	if (this.change)
	    		this.change(this.down);
	    }

	    result.state = function(down)
	    {
	    	this.down 			 = down;
	    	this.sampler.texture = this.down? down_texture : up_texture;
	    }

	    return result;
    },

    newSwitch: function()
    {
		return new ms.ui.Switch(this.manager, this);
    }
});

ms.ui.Image = Class.create(ms.ui.Component,
{
	initialize: function($super, manager, parent, texture)
	{
		$super(manager, parent);
	    this.texture = texture;
	},

	resized: function()
	{
	},

	color: function(r, g, b, a)
	{
	},

	image: function(texture)
	{
	    this.texture = texture;
	},

    draw: function($super, context, x, y)
    {
        if (this.texture)
        {
            context.drawImage(this.texture, this.x + x, this.y + y);
        }

        $super(context);
    }
});

ms.ui.ProgressBar = Class.create(ms.ui.Component,
{
	initialize: function($super, manager, parent, empty, full)
	{
		$super(manager, parent);
		this.left    = this.newImage(full);
		this.right   = this.newImage(empty);
		this.percent = 0;
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

	resized: function()
	{
	    var ww = this.w*(this.percent/100);
	    this.left.rect(0, 0, ww, this.h);
	    this.right.rect(ww, 0, this.w - ww, this.h);
	}
});

ms.ui.Label = Class.create(ms.ui.Component,
{
	initialize: function($super, manager, parent, font)
	{
		$super(manager, parent);
	    this.font = font;
	    this.value = "";
	},

	text: function(text)
	{
		this.value = text;
		this.resized();
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
        context.font = this.font;
        context.fillText(this.value, this.x + x, this.y + y);

        $super(context);
    },
});

ms.ui.Switch = Class.create(ms.ui.Component,
{
	active_: 0,

	componentAdded: function(cmp)
	{
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
	}
});
