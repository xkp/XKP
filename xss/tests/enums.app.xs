
on btn.click()
{
	var pl = EPlacement.left;
	
	placement++;
	
	if(visible == EVisibility.true)
	{
		visible = EVisibility.false;
	} 
	else 
	if(visible == EVisibility.false)
	{
		visible = EVisibility.gone;
	}
	else
	{
		visible = EVisibility.true;
	}
	
	enabled = !enabled;
	
	width = height * 2;
}
