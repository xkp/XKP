"pixastic".namespace();

pixastic.Preset = Class.create(
{
	initialize: function()
	{
		this.filters = [];
		this.apply_ = true;
	},	
	apply: function(image)
	{
		this.image = image;
		this.filters[0].set_image(image, image);
		return this.image;
	},
});

pixastic.Filter = Class.create(
{	
	initialize: function(parent)
	{		
		this.apply_ = true;		
		this.parent = parent;		
		this.parent.put_on_apply = false;
	},
	set_image: function(img, tmp_img)
	{
		this.image = img;	
		this.tmp_img = tmp_img;
		this.apply();
	},
	set_apply: function(apply_)
	{
		this.apply_ = apply_;
		this.apply();
	},
	rect: function(left, top, width, height)
	{
		this.options.rect = {"left": left, "top": top, "width": width, "height": height};
		this.apply();
	},
	apply: function(image)
	{
		if(image)
		{
			this.set_image(image, image);
		}
		if(this.image)
		{		
			for (var i = 0; i < this.image.data.length; ++i)
				this.tmp_img.data[i] = this.image.data[i];  			
			var curr_filter;			
			for(var i = 0; i < this.parent.filters.length; i++)
			{
				curr_filter = this.parent.filters[i];
				if(curr_filter.apply_)
				{
					if(curr_filter.use_canvas)
					{
						//THIS MUST BE DONE ON MAIN THREAD (USE CANVAS)
						var temp_canvas = document.createElement("canvas");
						temp_canvas.width = this.tmp_img.width;
						temp_canvas.height = this.tmp_img.height;					
						temp_canvas.getContext('2d').putImageData(this.tmp_img, 0, 0);
						temp_canvas = Pixastic.process(
						temp_canvas,
						curr_filter.filter_name,	// filter name
						curr_filter.options		// options object
						);
						this.tmp_img = temp_canvas.getContext('2d').getImageData(0, 0, this.tmp_img.width, this.tmp_img.height);
					}
					else
					{				
						//THIS MUST BE DONE ON INDEPENDENT THREADS (DON'T USE CANVAS)
						this.tmp_img = Pixastic.process(
						this.tmp_img,
						curr_filter.filter_name,	// filter name
						curr_filter.options		// options object
						);														
					}
				}			
			}
			if(this.parent.put_on_apply)
				put_image(this.parent, this.tmp_img);
		}	
		return this.image;
	},
});

pixastic.Brightness = Class.create(pixastic.Filter,
{
	initialize: function($super, brightness, contrast, rect, parent)
	{
		$super(parent);		
		this.filter_name = "brightness";
		this.use_canvas = false;
		this.options = {"brightness": brightness, "contrast": contrast, "rect": rect};			
	},		
	set_brightness: function(value)
	{	
		this.options.brightness = value;
		this.apply();
	},
	set_contrast: function(value)
	{	
		this.options.contrast = value;
		this.apply();
	},	
});	

pixastic.HSL = Class.create(pixastic.Filter,
{
	initialize: function($super, hue, saturation, lightness, rect, parent)
	{
		$super(parent);	
		this.filter_name = "hsl";
		this.use_canvas = false;
		this.options = {"hue": hue, "saturation": saturation, "lightness": lightness, "rect": rect};		
	},	
	set_hue: function(value)
	{	
		this.options.hue = value;
		this.apply();
	},
	set_saturation: function(value)
	{	
		this.options.saturation = value;
		this.apply();
	},
	set_lightness: function(value)
	{	
		this.options.lightness = value;
		this.apply();
	},	
});	

pixastic.Edges = Class.create(pixastic.Filter,
{
	initialize: function($super, mono, invert, rect, parent)
	{
		$super(parent);	
		this.filter_name = "edges";
		this.use_canvas = false;
		this.options = {"mono": mono, "invert": invert, "rect": rect};			
	},		
	set_mono: function(value)
	{	
		this.options.mono = value;
		this.apply();
	},	
	set_invert: function(value)
	{	
		this.options.invert = value;
		this.apply();
	},		
});	

pixastic.Pointillize = Class.create(pixastic.Filter,
{
	initialize: function($super, radius, density, noise, transparent, rect, parent)
	{
		$super(parent);	
		this.filter_name = "pointillize";
		this.use_canvas = true;
		this.options = {"radius": radius, "density": density, "noise": noise, "transparent": transparent, "rect": rect};		
	},		
	set_radius: function(value)
	{	
		this.options.radius = value;
		this.apply();
	},	
	set_density: function(value)
	{	
		this.options.density = value;
		this.apply();
	},	
	set_noise: function(value)
	{	
		this.options.noise = value;
		this.apply();
	},
	set_transparent: function(value)
	{	
		this.options.transparent = value;
		this.apply();
	},		
});	

pixastic.Blend = Class.create(pixastic.Filter,
{
	initialize: function($super, amount, mode, image, rect, parent)
	{
		$super(parent);	
		this.filter_name = "blend";
		this.use_canvas = true;
		this.options = {"amount": amount, "mode": mode, "image": streamer.get_resource(image).data, "rect": rect};				
	},	
	set_amount: function(value)
	{	
		this.options.amount = value;
		this.apply();
	},
	set_mode: function(value)
	{	
		this.options.mode = value;
		this.apply();
	},	
	set_image: function(value)
	{	
		if(value instanceof HTMLCanvasElement || value instanceof HTMLImageElement)
			this.options.image = value;
		else
			this.options.image = streamer.get_resource(image).data;	
		this.apply();
	},	
});	

pixastic.FlipV = Class.create(pixastic.Filter,
{
	initialize: function($super, rect, parent)
	{			
		$super(parent);	
		this.filter_name = "flipv";
		this.use_canvas = true;
		this.options = {"rect": rect};			
	},		
});

pixastic.Invert = Class.create(pixastic.Filter,
{
	initialize: function($super, rect, parent)
	{			
		$super(parent);	
		this.use_canvas = false;
		this.filter_name = "invert";
		this.options = {"rect": rect};		
		this.apply();		
	},
});

pixastic.Noise = Class.create(pixastic.Filter,
{	
	initialize: function($super, amount, strength, mono, rect, parent)
	{
		$super(parent);	
		this.filter_name = "noise";
		this.use_canvas = false;
		this.options = {"amount": amount, "strength": strength, "mono": mono, "rect": rect};		
	},		
	set_amount: function(value)
	{	
		this.options.amount = value;
		this.apply();
	},
	set_strength: function(value)
	{	
		this.options.strength = value;
		this.apply();
	},	
	set_mono: function(value)
	{	
		this.options.mono = value;
		this.apply();
	},
});	

pixastic.Sepia = Class.create(pixastic.Filter,
{
	initialize: function($super, rect, parent)
	{			
		$super(parent);	
		this.filter_name = "sepia";
		this.use_canvas = false;
		this.options = {"rect": rect};					
	},
});

pixastic.ColorAdjust = Class.create(pixastic.Filter,
{
	initialize: function($super, red, green, blue, rect, parent)
	{
		$super(parent);	
		this.filter_name = "coloradjust";
		this.use_canvas = false;
		this.options = {"red": red, "green": green, "blue": blue, "rect": rect};				
	},		
	set_red: function(value)
	{
		this.options.red = value;
		this.apply();
	},
	set_green: function(value)
	{
		this.options.green = value;
		this.apply();
	},	
	set_blue: function(value)
	{
		this.options.blue = value;
		this.apply();
	},
});

pixastic.Blur = Class.create(pixastic.Filter,
{
	initialize: function($super, rect, parent)
	{	
		$super(parent);	
		this.filter_name = "blur";
		this.use_canvas = false;
		this.options = {"rect": rect};			
	},
});

pixastic.Edges2 = Class.create(pixastic.Filter,
{
	initialize: function($super, rect, parent)
	{			
		$super(parent);	
		this.filter_name = "edges2";
		this.use_canvas = false;
		this.options = {"rect": rect};		
	},
});

pixastic.Glow = Class.create(pixastic.Filter,
{	
	initialize: function($super, amount, radius, rect, parent)
	{
		$super(parent);	
		this.filter_name = "glow";
		this.use_canvas = true;
		this.options = {"amount": amount, "radius": radius, "rect": rect};			
	},
	set_amount: function(value)
	{	
		this.options.amount = value;
		this.apply();
	},
	set_radius: function(value)
	{	
		this.options.radius = value;
		this.apply();
	},
});	

pixastic.Laplace = Class.create(pixastic.Filter,
{	
	initialize: function($super, edge_strength, grey_level, invert, rect, parent)
	{
		$super(parent);	
		this.filter_name = "laplace";
		this.use_canvas = false;
		this.options = {"edgeStrength": edge_strength, "greyLevel": grey_level, "invert": invert, "rect": rect};		
	},	
	set_edge_strength: function(value)
	{	
		this.options.edgeStrength = value;
		this.apply();
	},
	set_grey_level: function(value)
	{	
		this.options.greyLevel = value;
		this.apply();
	},
	set_invert: function(value)
	{	
		this.options.invert = value;
		this.apply();
	},	
});

pixastic.Sharpen = Class.create(pixastic.Filter,
{	
	initialize: function($super, amount, rect, parent)
	{
		$super(parent);	
		this.filter_name = "sharpen";
		this.use_canvas = false;
		this.options = {"amount": amount, "rect": rect};		
	},
	set_amount: function(value)
	{	
		this.options.amount = value;
		this.apply();
	},	
});	

pixastic.BlurFast = Class.create(pixastic.Filter,
{	
	initialize: function($super, amount, rect, parent)
	{
		$super(parent);	
		this.filter_name = "blurfast";
		this.use_canvas = true;
		this.options = {"amount": amount, "rect": rect};			
	},
	set_amount: function(value)
	{	
		this.options.amount = value;
		this.apply();
	},	
});	

pixastic.Crop = Class.create(pixastic.Filter,
{	
	initialize: function($super, rect, parent)
	{
		$super(parent);	
		this.filter_name = "crop";
		this.use_canvas = true;
		this.options = {"rect": rect};			
	},
});	

pixastic.Emboss = Class.create(pixastic.Filter,
{
	initialize: function($super, strength, grey_level, direction, blend, rect, parent)
	{
		$super(parent);	
		this.filter_name = "emboss";
		this.use_canvas = false;
		this.options = {"strength": strength, "greyLevel": grey_level, "direction": direction, "blend": blend,"rect": rect};		
	},
	set_strength: function(value)
	{	
		this.options.strength = value;
		this.apply();
	},	
	set_grey_level: function(value)
	{	
		this.options.greyLevel = value;
		this.apply();
	},	
	set_direction: function(value)
	{	
		this.options.direction = value;
		this.apply();
	},	
	set_blend: function(value)
	{	
		this.options.blend = value;
		this.apply();
	},	
});	

pixastic.Lighten = Class.create(pixastic.Filter,
{	
	initialize: function($super, amount, rect, parent)
	{
		$super(parent);	
		this.filter_name = "lighten";
		this.use_canvas = false;
		this.options = {"amount": amount, "rect": rect};		
	},
	set_amount: function(value)
	{	
		this.options.amount = value;
		this.apply();
	},	
});	

pixastic.Posterize = Class.create(pixastic.Filter,
{	
	initialize: function($super, levels, rect, parent)
	{
		$super(parent);	
		this.filter_name = "posterize";
		this.use_canvas = false;
		this.options = {"levels": levels, "rect": rect};		
	},
	set_levels: function(value)
	{	
		this.options.levels = value;
		this.apply();
	},	
});	

pixastic.Solarize = Class.create(pixastic.Filter,
{	
	initialize: function($super, rect, parent)
	{
		$super(parent);	
		this.filter_name = "solarize";
		this.use_canvas = false;
		this.options = {"rect": rect};			
	},
});	

pixastic.Desaturate = Class.create(pixastic.Filter,
{	
	initialize: function($super, average, rect, parent)
	{
		$super(parent);	
		this.filter_name = "desaturate";
		this.use_canvas = false;
		this.options = {"average": average, "rect": rect};		
	},
	set_average: function(value)
	{	
		this.options.average = value;
		this.apply();
	},	
});	

pixastic.FlipH = Class.create(pixastic.Filter,
{
	initialize: function($super, rect, parent)
	{			
		$super(parent);	
		this.filter_name = "fliph";
		this.use_canvas = true;
		this.options = {"rect": rect};			
	},		
});

pixastic.Mosaic = Class.create(pixastic.Filter,
{	
	initialize: function($super, block_size, rect, parent)
	{
		$super(parent);	
		this.filter_name = "mosaic";
		this.use_canvas = true;
		this.options = {"blockSize": block_size, "rect": rect};			
	},
	set_block_size: function(value)
	{	
		this.options.blockSize = value;
		this.apply();
	},	
});	

pixastic.RemoveNoise = Class.create(pixastic.Filter,
{
	initialize: function($super, rect, parent)
	{			
		$super(parent);	
		this.filter_name = "removenoise";
		this.use_canvas = false;
		this.options = {"rect": rect};					
	},		
});

pixastic.UnsharpMask = Class.create(pixastic.Filter,
{	
	initialize: function($super, amount, radius, threshold, rect, parent)
	{
		$super(parent);	
		this.filter_name = "unsharpmask";
		this.use_canvas = true;
		this.options = {"amount": amount, "radius": radius, "threshold": threshold, "rect": rect};			
	},
	set_amount: function(value)
	{	
		this.options.amount = value;
		this.apply();
	},
	set_radius: function(value)
	{	
		this.options.radius = value;
		this.apply();
	},	
	set_threshold: function(value)
	{	
		this.options.threshold = value;
		this.apply();
	},	
});	