class my_image : img
{	
	property data_source
	{
		src = value;
	}
}

class menu_item : div
{
	property data_source
	{
		lbl_caption.caption = value.caption;
		icon.src = value.image;
	}
}