"pixastic".namespace();

pixastic.Brightness = Class.create(
{
	initialize: function(brightness, contrast, id, parent)
	{
		this.brightness = brightness;
		this.contrast = contrast;
		this.id = id;
		this.parent = parent;	
		obtain_image(this, this.parent, this.id);
		this.invalidate();
	},		
	set_brightness: function(value)
	{	
		this.brightness = value;
		this.invalidate();
	},
	set_contrast: function(value)
	{	
		this.contrast = value;
		this.invalidate();
	},
	invalidate: function()
	{
		var newimg = Pixastic.process(
		this.original_image,
		"brightness",	// brightness/contrast adjustment
		{				// options object
				"brightness" : this.brightness,
				"contrast" : this.contrast,				
		}
		);
		set_image(this, this.parent, newimg);		
	},
});	

function obtain_image(obj, parent, id)
{
	if(parent instanceof ms.ui.Component)
	{
		obj.original_image = parent.texture;		
	}else
	if(parent instanceof THREE.Mesh)
	{
		obj.original_image = parent.materials[0].map.image;				
	}else
	if(parent instanceof THREE.Material)
	{
		obj.original_image = parent.map.image;				
	}else
	{
		obj.original_image = document.createElement("canvas");
		obj.original_image.width = parent.data.width;
		obj.original_image.height = parent.data.height;
		obj.original_image.getContext('2d').putImageData(parent.data.getContext('2d').getImageData(0, 0, parent.data.width, parent.data.height), 0, 0);			
	}
	if(!obj.parent.filters)
		obj.parent.filters = [];
	obj.parent.filters.push(obj);	
}

function set_image(obj, parent, img)
{
	if(parent instanceof ms.ui.Component)
	{
		obj.parent.texture = img;
		obj.parent.invalidate();
	}else
	if(parent instanceof THREE.Mesh)
	{	
		obj.parent.materials[0].map.image = img;		
	}else
	if(parent instanceof THREE.Material)
	{
		obj.parent.map.image = img;			
	}else
	{
		obj.parent.data.getContext('2d').putImageData(img.getContext('2d').getImageData(0, 0, img.width, img.height), 0, 0);
	}
}