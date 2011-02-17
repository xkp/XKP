on btn1.click()
{
	btn2.caption = "Clicked";
	
	caption = "btnn1 was clicked!";

	btn2.width += btn1.width;
	application.reset(600);
}

property target : string
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
	btn2.caption = "..." + target;
}

on btn2.click()
{
	btn1.caption = "Old value: " + application.target;
	application.target = 300;
}
