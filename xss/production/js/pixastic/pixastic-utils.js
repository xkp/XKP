function obtain_image(obj, parent)
{
	var resource;
	if(parent instanceof ms.ui.Component)
	{
		resource = streamer.get_resource(parent.texture.id);		
	}else
	if(parent instanceof THREE.Mesh)
	{
		resource = streamer.get_resource(parent.materials[0].map.image.id);				
	}else
	if(parent instanceof THREE.Material)
	{
		resource = streamer.get_resource(parent.map.image.id);				
	}else
	{
		resource = parent;			
	}	
	var img_data = resource.data.getContext('2d').getImageData(0, 0, resource.data.width, resource.data.height);
	var tmp_data = resource.data.getContext('2d').createImageData(resource.data.width, resource.data.height);
    for (var i = 0; i < img_data.data.length; ++i)
		tmp_data.data[i] = img_data.data[i];  
	img_data.id = resource.id;	
	tmp_data.id = resource.id;	
	obj.set_image(img_data, tmp_data);	
}

function get_image_data(parent)
{
	var resource;
	if(parent instanceof ms.ui.Component)
	{
		resource = streamer.get_resource(parent.texture.id);		
	}else
	if(parent instanceof THREE.Mesh)
	{
		resource = streamer.get_resource(parent.materials[0].map.image.id);				
	}else
	if(parent instanceof THREE.Material)
	{
		resource = streamer.get_resource(parent.map.image.id);				
	}else
	{
		resource = parent;			
	}	
	var img_data = resource.data.getContext('2d').getImageData(0, 0, resource.data.width, resource.data.height);
	img_data.id = resource.id;	
	return img_data;
}

function put_image(parent, img)
{
	var img__ = document.createElement("canvas");
	img__.width = img.width;
	img__.height = img.height;
	img__.id = img.id;
	img__.getContext('2d').putImageData(img, 0, 0);
	var parent__ = parent;	
	if(parent__ instanceof ms.ui.Component)
	{
		parent__.texture = img__;
		parent__.invalidate();
	}else
	if(parent__ instanceof THREE.Mesh)
	{	
		parent__.materials[0].map.image = img__;		
	}else
	if(parent__ instanceof THREE.Material)
	{
		parent__.map.image = img__;			
	}else
	{
		parent__.data.getContext('2d').putImageData(img, 0, 0);
	}
}