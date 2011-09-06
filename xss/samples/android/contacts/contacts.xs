//ERROR: verify correct code_type_resolver in property like this
//       property swtTabsTexts = ["value1", "value2", "value3"];

property swtTabsTexts : array<string> = [
	"Add contact",
	"Find contact",
	"About"];

on creation()
{
}

method getSwitchText(int idx) : string
{
	if(idx < 0)
	{
		idx = swtTabsTexts.size;
	}
	else
	if(idx >= swtTabsTexts.size)
	{
		idx = idx % swtTabsTexts.size;
	}
		
	return swtTabsTexts[idx];
}

on btnLeft.click()
{
	swtStates.active_index--;
	btnLeft.caption = getSwitchText(swtStates.active_index - 1);
	
	placement x = btnLeft.placement;
	//TODO: need functionality to translate code like below that is:
	//      ((XKPLayout.LayoutParams) priv_btnLeft.getLayoutParams()).setPlacement(XKPLayout.PL_LEFT);
	btnLeft.placement = left;
}

on btnRight.click()
{
	swtStates.active_index++;
	btnLeft.caption = getSwitchText(swtStates.active_index + 1);
}

on chkVerifier.change()
{
	
}

