var jsmanip = {};

jsmanip.Preset = Class.extend(
{
	init: function()
	{
		this.filters = [];
		this.apply_ = true;
	},	

    deserialize: function(data)
    {
	    for(var i = 0; i < data.filters.length; i++)
	    {
            var filter = data.filters[i];
            var filter_name = filter.filter_name.name;

		    var curr_filter = new jsmanip.Constructors[filter_name](this);
            this.filters.push(curr_filter);
	    }
    },

	manipulate(image)
    {
	    for(var i = 0; i < this.filters.length; i++)
	    {
		    curr_filter = this.parent.filters[i];
			curr_filter.filter_name.filter(image, curr_filter.options);							
	    }
    },

    apply: function(image)
	{
		this.image = image;
		this.filters[0].set_image(image, image);
		return this.image;
	},
});

jsmanip.Filter = Class.extend(
{	
	init: function(parent)
	{		
		this.apply_ = true;		
		this.parent = parent;	
		this.render_on_apply = false;
	},
	set_render_on_apply: function(value)
	{
		this.render_on_apply = value;
		this.apply();
	},
	set_image: function(orig_img, img)
	{
		this.orig_image = orig_img;
		this.image = img;	
		this.apply();
	},
	set_apply: function(apply_)
	{
		this.apply_ = apply_;
		this.apply();
	},	
	apply: function(image)
	{
		if(image)
		{
			this.orig_image = image;
			this.image = image;
		}
		if(this.image)
		{				 	
			for (var i = 0; i < this.image.data.length; ++i)
				this.image.data[i] = this.orig_image.data[i];
			var curr_filter;			
			for(var i = 0; i < this.parent.filters.length; i++)
			{
				curr_filter = this.parent.filters[i];
				if(curr_filter.apply_)
				{						
					curr_filter.filter_name.filter(this.image, this.options);							
				}			
			}
			if(this.render_on_apply)
				put_image(this.parent, this.image);
		}	
		return this.image;
	},
});

jsmanip.Brightness = jsmanip.Filter.extend(
{
	init: function(amount_, parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.brightness;		
		this.options = {amount: amount_};			
	},		
	set_amount: function(value)
	{	
		this.options.amount = value;
		this.apply();
	},	
});	

jsmanip.Blur = jsmanip.Filter.extend(
{
	init: function(amount_, parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.blur;		
		this.filter_string = "blur";		
		this.options = {amount: amount_};			
	},		
	set_amount: function(value)
	{	
		this.options.amount = value;
		this.apply();
	},	
});	

jsmanip.Bump = jsmanip.Filter.extend(
{
	init: function(parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.bump;				
		this.filter_string = "bump";		
	},	
});

jsmanip.Circlesmear = jsmanip.Filter.extend(
{
	init: function(size_, density_, mix_, parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.circlesmear;		
		this.filter_string = "circlesmear";		
		this.options = {size: size_, density: density_, mix: mix_};			
	},		
	set_size: function(value)
	{	
		this.options.size = value;
		this.apply();
	},	
	set_density: function(value)
	{	
		this.options.density = value;
		this.apply();
	},	
	set_mix: function(value)
	{	
		this.options.mix = value;
		this.apply();
	},	
});

jsmanip.Contrast = jsmanip.Filter.extend(
{
	init: function(amount_, parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.contrast;		
		this.filter_string = "contrast";		
		this.options = {amount: amount_};			
	},		
	set_amount: function(value)
	{	
		this.options.amount = value;
		this.apply();
	},	
});	

jsmanip.Crosssmear = jsmanip.Filter.extend(
{
	init: function(size_, density_, mix_, parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.crosssmear;		
		this.filter_string = "crosssmear";		
		this.options = {size: size_, density: density_, mix: mix_};			
	},		
	set_size: function(value)
	{	
		this.options.size = value;
		this.apply();
	},	
	set_density: function(value)
	{	
		this.options.density = value;
		this.apply();
	},	
	set_mix: function(value)
	{	
		this.options.mix = value;
		this.apply();
	},	
});

jsmanip.Diffusion = jsmanip.Filter.extend(
{
	init: function(scale_, parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.diffusion;		
		this.filter_string = "diffusion";		
		this.options = {scale: scale_};			
	},		
	set_scale: function(value)
	{	
		this.options.scale = value;
		this.apply();
	},	
});	

jsmanip.Dither = jsmanip.Filter.extend(
{
	init: function(levels_, color_, parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.dither;		
		this.filter_string = "dither";		
		this.options = {levels: levels_, color: color_};			
	},		
	set_levels: function(value)
	{	
		this.options.levels = value;
		this.apply();
	},	
	set_color: function(value)
	{	
		this.options.color = value;
		this.apply();
	},	
});	

jsmanip.Edge = jsmanip.Filter.extend(
{
	init: function(parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.edge;				
		this.filter_string = "edge";		
	},	
});

jsmanip.Emboss = jsmanip.Filter.extend(
{
	init: function(height_, angle_, elevation_, parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.emboss;		
		this.filter_string = "emboss";		
		this.options = {height: height_, angle: angle_, elevation: elevation_};			
	},		
	set_height: function(value)
	{	
		this.options.height = value;
		this.apply();
	},	
	set_angle: function(value)
	{	
		this.options.angle = value;
		this.apply();
	},	
	set_elevation: function(value)
	{	
		this.options.elevation = value;
		this.apply();
	},	
});	

jsmanip.Exposure = jsmanip.Filter.extend(
{
	init: function(exposure_, parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.exposure;		
		this.filter_string = "exposure";		
		this.options = {exposure: exposure_};			
	},		
	set_exposure: function(value)
	{	
		this.options.exposure = value;
		this.apply();
	},	
});	

jsmanip.Gain = jsmanip.Filter.extend(
{
	init: function(gain_, bias_, parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.gain;		
		this.filter_string = "gain";		
		this.options = {gain: gain_, bias: bias_};			
	},		
	set_gain: function(value)
	{	
		this.options.gain = value;
		this.apply();
	},	
	set_bias: function(value)
	{	
		this.options.bias = value;
		this.apply();
	},
});	

jsmanip.Gamma = jsmanip.Filter.extend(
{
	init: function(amount_, parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.gamma;		
		this.filter_string = "gamma";		
		this.options = {amount: amount_};			
	},		
	set_amount: function(value)
	{	
		this.options.amount = value;
		this.apply();
	},	
});	

jsmanip.Greyscale = jsmanip.Filter.extend(
{
	init: function(parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.grayscale;				
		this.filter_string = "grayscale";		
	},	
});

jsmanip.Hue = jsmanip.Filter.extend(
{
	init: function(amount_, parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.hue;		
		this.filter_string = "hue";		
		this.options = {amount: amount_};			
	},		
	set_amount: function(value)
	{	
		this.options.amount = value;
		this.apply();
	},	
});	

jsmanip.Invert = jsmanip.Filter.extend(
{
	init: function(parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.invert;				
		this.filter_string = "invert";		
	},	
});

jsmanip.Kaleidoscope = jsmanip.Filter.extend(
{
	init: function(angle_, rotation_, sides_, center_x_, center_y_, parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.kaleidoscope;		
		this.filter_string = "kaleidoscope";		
		this.options = {angle: angle_, rotation: rotation_, sides: sides_, centerX: center_x_, centerY: center_y_};			
	},		
	set_angle: function(value)
	{	
		this.options.angle = value;
		this.apply();
	},	
	set_rotation: function(value)
	{	
		this.options.rotation = value;
		this.apply();
	},	
	set_sides: function(value)
	{	
		this.options.sides = value;
		this.apply();
	},	
	set_center_x: function(value)
	{	
		this.options.centerX = value;
		this.apply();
	},	
	set_center_y: function(value)
	{	
		this.options.centerY = value;
		this.apply();
	},	
});	

jsmanip.Lensdistortion = jsmanip.Filter.extend(
{
	init: function(refraction_, radius_, center_x_, center_y_, parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.lensdistortion;		
		this.filter_string = "lensdistortion";		
		this.options = {refraction: refraction_, radius: radius_, centerX: center_x_, centerY: center_y_};			
	},		
	set_refraction: function(value)
	{	
		this.options.refraction = value;
		this.apply();
	},	
	set_radius: function(value)
	{	
		this.options.radius = value;
		this.apply();
	},	
	set_center_x: function(value)
	{	
		this.options.centerX = value;
		this.apply();
	},	
	set_center_y: function(value)
	{	
		this.options.centerY = value;
		this.apply();
	},	
});	


jsmanip.Linesmear = jsmanip.Filter.extend(
{
	init: function(distance_, density_, angle_, mix_, parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.linesmear;		
		this.filter_string = "linesmear";		
		this.options = {distance: distance_, density: density_, angle: angle_, mix: mix_};			
	},		
	set_distance: function(value)
	{	
		this.options.distance = value;
		this.apply();
	},	
	set_density: function(value)
	{	
		this.options.density = value;
		this.apply();
	},	
	set_angle: function(value)
	{	
		this.options.angle = value;
		this.apply();
	},	
	set_mix: function(value)
	{	
		this.options.mix = value;
		this.apply();
	},	
});	

jsmanip.Maximum = jsmanip.Filter.extend(
{
	init: function(parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.maximum;				
		this.filter_string = "maximum";		
	},	
});

jsmanip.Median = jsmanip.Filter.extend(
{
	init: function(parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.median;				
		this.filter_string = "median";		
	},	
});

jsmanip.Minimum = jsmanip.Filter.extend(
{
	init: function(parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.minimum;				
		this.filter_string = "minimum";		
	},	
});

jsmanip.Noise = jsmanip.Filter.extend(
{
	init: function(amount_, density_, monochrome_, parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.noise;		
		this.filter_string = "noise";		
		this.options = {amount: amount_, density: density_, monochrome: monochrome_};			
	},		
	set_amount: function(value)
	{	
		this.options.amount = value;
		this.apply();
	},	
	set_density: function(value)
	{	
		this.options.density = value;
		this.apply();
	},	
	set_monochrome: function(value)
	{	
		this.options.monochrome = value;
		this.apply();
	},	
	set_mix: function(value)
	{	
		this.options.mix = value;
		this.apply();
	},	
});

jsmanip.Oil = jsmanip.Filter.extend(
{
	init: function(range_, parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.oil;		
		this.filter_string = "oil";		
		this.options = {range: range_};			
	},		
	set_range: function(value)
	{	
		this.options.range = value;
		this.apply();
	},	
});	

jsmanip.Pinch = jsmanip.Filter.extend(
{
	init: function(amount_, radius_, angle_, center_x_, center_y_, parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.pinch;		
		this.filter_string = "pinch";		
		this.options = {amount: amount_, radius: radius_, angle: angle_, centerX: center_x_, centerY: center_y_};			
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
	set_angle: function(value)
	{	
		this.options.angle = value;
		this.apply();
	},
	set_center_x: function(value)
	{	
		this.options.centerX = value;
		this.apply();
	},	
	set_center_y: function(value)
	{	
		this.options.centerY = value;
		this.apply();
	},	
});	

jsmanip.Pixelate = jsmanip.Filter.extend(
{
	init: function(size_, parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.pixelate;		
		this.filter_string = "pixelate";		
		this.options = {size: size_};			
	},		
	set_size: function(value)
	{	
		this.options.size = value;
		this.apply();
	},	
});

jsmanip.Posterize = jsmanip.Filter.extend(
{
	init: function(levels_, parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.posterize;		
		this.filter_string = "posterize";		
		this.options = {levels: levels_};			
	},		
	set_levels: function(value)
	{	
		this.options.levels = value;
		this.apply();
	},	
});

jsmanip.RGBadjust = jsmanip.Filter.extend(
{
	init: function(red_, green_, blue_, parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.rgbadjust;		
		this.filter_string = "rgbadjust";		
		this.options = {red: red_, green: green_, blue: blue_};			
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

jsmanip.Saturation = jsmanip.Filter.extend(
{
	init: function(amount_, parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.saturation;		
		this.filter_string = "saturation";		
        this.options = {amount: amount_};			
	},		
	set_amount: function(value)
	{	
		this.options.amount = value;
		this.apply();
	},	
});	

jsmanip.Sawtoothripple = jsmanip.Filter.extend(
{
	init: function(amplitude_x_, amplitude_y_, wavelength_x_, wavelength_y_,parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.sawtoothripple;		
		this.filter_string = "sawtoothripple";		
		this.options = {xAmplitude: amplitude_x_, yAmplitude: amplitude_y_, xWavelength: wavelength_x_, yWavelength: wavelength_y_};			
	},		
	set_amplitude_x: function(value)
	{	
		this.options.xAmplitude = value;
		this.apply();
	},	
	set_amplitude_y: function(value)
	{	
		this.options.yAmplitude = value;
		this.apply();
	},	
	set_wavelength_x: function(value)
	{	
		this.options.xWavelength = value;
		this.apply();
	},	
	set_wavelength_y: function(value)
	{	
		this.options.yWavelength = value;
		this.apply();
	},	
});	

jsmanip.Sepia = jsmanip.Filter.extend(
{
	init: function(amount_, parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.sepia;		
		this.filter_string = "sepia";		
		this.options = {amount: amount_};			
	},		
	set_amount: function(value)
	{	
		this.options.amount = value;
		this.apply();
	},	
});	

jsmanip.Sharpen = jsmanip.Filter.extend(
{
	init: function(parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.sharpen;				
		this.filter_string = "sharpen";		
	},	
});

jsmanip.Sineripple = jsmanip.Filter.extend(
{
	init: function(amplitude_x_, amplitude_y_, wavelength_x_, wavelength_y_,parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.sineripple;		
		this.filter_string = "sineripple";		
		this.options = {xAmplitude: amplitude_x_, yAmplitude: amplitude_y_, xWavelength: wavelength_x_, yWavelength: wavelength_y_};			
	},		
	set_amplitude_x: function(value)
	{	
		this.options.xAmplitude = value;
		this.apply();
	},	
	set_amplitude_y: function(value)
	{	
		this.options.yAmplitude = value;
		this.apply();
	},	
	set_wavelength_x: function(value)
	{	
		this.options.xWavelength = value;
		this.apply();
	},	
	set_wavelength_y: function(value)
	{	
		this.options.yWavelength = value;
		this.apply();
	},	
});

jsmanip.Solarize = jsmanip.Filter.extend(
{
	init: function(parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.solarize;				
		this.filter_string = "solarize";		
	},	
});

jsmanip.Sparkle = jsmanip.Filter.extend(
{
	init: function(rays_, size_, amount_, randomness_, center_x_, center_y_, parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.sparkle;		
		this.filter_string = "sparkle";		
		this.options = {rays: rays_, size: size_, amount: amount_, randomness: randomness_, centerX: center_x_, centerY: center_y_};			
	},		
	set_rays: function(value)
	{	
		this.options.rays = value;
		this.apply();
	},	
	set_size: function(value)
	{	
		this.options.size = value;
		this.apply();
	},	
	set_amount: function(value)
	{	
		this.options.amount = value;
		this.apply();
	},
	set_randomness: function(value)
	{	
		this.options.randomness = value;
		this.apply();
	},
	set_center_x: function(value)
	{	
		this.options.centerX = value;
		this.apply();
	},	
	set_center_y: function(value)
	{	
		this.options.centerY = value;
		this.apply();
	},	
});

jsmanip.Squaresmear = jsmanip.Filter.extend(
{
	init: function(size_, density_, mix_, parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.squaresmear;		
		this.filter_string = "squaresmear";		
		this.options = {size: size_, density: density_, mix: mix_};			
	},		
	set_size: function(value)
	{	
		this.options.size = value;
		this.apply();
	},	
	set_density: function(value)
	{	
		this.options.density = value;
		this.apply();
	},	
	set_mix: function(value)
	{	
		this.options.mix = value;
		this.apply();
	},	
});	

jsmanip.Threshold = jsmanip.Filter.extend(
{
	init: function(threshold_, parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.threshold;		
		this.filter_string = "threshold";		
		this.options = {threshold: threshold_};			
	},		
	set_threshold: function(value)
	{	
		this.options.threshold = value;
		this.apply();
	},	
});	

jsmanip.Triangleripple = jsmanip.Filter.extend(
{
	init: function(amplitude_x_, amplitude_y_, wavelength_x_, wavelength_y_,parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.triangleripple;		
		this.filter_string = "triangleripple";		
		this.options = {xAmplitude: amplitude_x_, yAmplitude: amplitude_y_, xWavelength: wavelength_x_, yWavelength: wavelength_y_};			
	},		
	set_amplitude_x: function(value)
	{	
		this.options.xAmplitude = value;
		this.apply();
	},	
	set_amplitude_y: function(value)
	{	
		this.options.yAmplitude = value;
		this.apply();
	},	
	set_wavelength_x: function(value)
	{	
		this.options.xWavelength = value;
		this.apply();
	},	
	set_wavelength_y: function(value)
	{	
		this.options.yWavelength = value;
		this.apply();
	},	
});

jsmanip.Vignette = jsmanip.Filter.extend(
{
	init: function(amount_, parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.vignette;		
		this.filter_string = "vignette";		
		this.options = {amount: amount_};			
	},		
	set_amount: function(value)
	{	
		this.options.amount = value;
		this.apply();
	},	
});

jsmanip.Twirl = jsmanip.Filter.extend(
{
	init: function(radius_, angle_, center_x_, center_y_, parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.twirl;		
		this.filter_string = "twirl";		
		this.options = {radius: radius_, angle: angle_, centerX: center_x_, centerY: center_y_};			
	},		
	set_radius: function(value)
	{	
		this.options.radius = value;
		this.apply();
	},	
	set_angle: function(value)
	{	
		this.options.angle = value;
		this.apply();
	},
	set_center_x: function(value)
	{	
		this.options.centerX = value;
		this.apply();
	},	
	set_center_y: function(value)
	{	
		this.options.centerY = value;
		this.apply();
	},	
});	

jsmanip.Waterripple = jsmanip.Filter.extend(
{
	init: function(phase_, radius_, wavelength_, amplitude_, center_x_, center_y_, parent)
	{
		this._super(parent);		
		this.filter_name = JSManipulate.waterripple;		
		this.filter_string = "waterripple";		
		this.options = {phase: phase_, radius: radius_, wavelength: wavelength_, amplitude: amplitude_, centerX: center_x_, centerY: center_y_};			
	},	
	set_phase: function(value)
	{	
		this.options.phase = value;
		this.apply();
	},	
	set_radius: function(value)
	{	
		this.options.radius = value;
		this.apply();
	},	
	set_wavelength: function(value)
	{	
		this.options.wavelength = value;
		this.apply();
	},
	set_amplitude: function(value)
	{	
		this.options.amplitude = value;
		this.apply();
	},
	set_center_x: function(value)
	{	
		this.options.centerX = value;
		this.apply();
	},	
	set_center_y: function(value)
	{	
		this.options.centerY = value;
		this.apply();
	},	
});	

jsmanip.Constructors = 
{
	blur : jsmanip.Blur,
	brightness : jsmanip.Brightness,
	bump : jsmanip.Bump,
	circlesmear : jsmanip.CircleSmear,
	contrast : jsmanip.Contrast,
	crosssmear : jsmanip.CrossSmear,
	diffusion : jsmanip.Diffusion,
	dither : jsmanip.Dither,
	edge : jsmanip.Edge,
	emboss : jsmanip.Emboss,
	exposure : jsmanip.Exposure,
	gain : jsmanip.Gain,
	gamma : jsmanip.Gamma,
	grayscale : jsmanip.Grayscale,
	hue : jsmanip.Hue,
	invert : jsmanip.Invert,
	kaleidoscope : jsmanip.Kaleidoscope,
	lensdistortion : jsmanip.LensDistortion,
	linesmear : jsmanip.LineSmear,
	maximum : jsmanip.Maximum,
	median : jsmanip.Median,
	minimum : jsmanip.Minimum,
	noise : jsmanip.Noise,
	oil : jsmanip.Oil,
	opacity : jsmanip.Opacity,
	pinch : jsmanip.Pinch,
	pixelate : jsmanip.Pixelation,
	posterize : jsmanip.Posterize,
	rgbadjust : jsmanip.RGBAdjust,
	saturation : jsmanip.Saturation,
	sawtoothripple : jsmanip.SawtoothRipple,
	sepia : jsmanip.Sepia,
	sharpen : jsmanip.Sharpen,
	sineripple : jsmanip.SineRipple,
	solarize : jsmanip.Solarize,
	sparkle : jsmanip.Sparkle,
	squaresmear : jsmanip.SquareSmear,
	threshold : jsmanip.Threshold,
	triangleripple : jsmanip.TriangleRipple,
	twirl : jsmanip.Twirl,
	vignette : jsmanip.Vignette,
	waterripple : jsmanip.WaterRipple 
};