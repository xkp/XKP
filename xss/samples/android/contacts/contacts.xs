
property swtTabsTexts : array<string> = [
	"Add contact",
	"Find contact",
	"About"];

on creation()
{
}

method getSwitchText(int idx) : string
{
	var swtTabsSize = swtTabsTexts.size;
	if(idx < 0)
	{
		idx = swtTabsSize;
	}
	else
	if(idx >= swtTabsSize)
	{
		idx = idx % swtTabsSize;
	}
	
	return swtTabsTexts[idx];
}

on btnLeft.click()
{
	swtStates.active_index--;
	btnLeft.caption = application.getSwitchText(swtStates.active_index - 1);
	
	EPlacement x = btnLeft.placement;
	
	//TODO: need functionality to translate code like below that is:
	//      ((XKPLayout.LayoutParams) priv_btnLeft.getLayoutParams()).setPlacement(XKPLayout.PL_LEFT);
	btnLeft.placement = EPlacement.left;
}

on btnRight.click()
{
	swtStates.active_index++;
	btnLeft.caption = application.getSwitchText(swtStates.active_index + 1);
}

on chkVerifier.change()
{
	
}
