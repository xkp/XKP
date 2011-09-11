
on btn.click()
{
	placement++;
	
	if(visible == true)
	{
		visible = false;
	} 
	else 
	if(visible == false)
	{
		visible = gone;
	}
	else
	{
		visible = true;
	}
	
	enabled = !enabled;
	
	width = height * 2;
}
