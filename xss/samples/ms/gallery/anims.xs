
class frame_anim : sequence
{
	//an array of lines to act as the frame
	property target : array 
	{
		left	= value[0]; 
		right	= value[1];  
		top		= value[2]; 
		bottom	= value[3]; 
	}

	//a rect (x, y, w, h) to where the target lines will move
	property dest;			 

	//the actual lines
	private property left    : line; 
	private property right   : line;  
	private property top	 : line;  
	private property bottom	 : line; 
}

class fade_in : sequence
{
	property target : img;
}