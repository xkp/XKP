
property sameName : string = "My owner is application.";

instance btn
{
	property sameName : string = "My owner is button 'btn'";
	
	on click()
	{
		EPlacement pl = EPlacement.left;

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
		
		//swap
		var swp = application.sameName;
		application.sameName = sameName;
		sameName = swp;
	}
}
