on btn1.click()
{
	caption = "Other Clicked";
	btn2.caption = "Clicked";
}

property target
{
	btn2.caption = "target acquired " + value;
	
	for(int i = 0; i < 20; i++)
	{
		btn2.width = btn2.width + 10;
		if (btn2.width > 200)
			btn2.width = 100;
	}
}

method reset(string s)
{
	target = s;
	btn1.caption = "...";
	btn2.caption = "...";
}

on btn2.click()
{
	btn1.caption = "Old value: " + application.target;
	application.target = 300;
}