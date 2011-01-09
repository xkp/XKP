on pushButton_1.click()
{
	pushButton_2.caption = "Clicked";
	
	// this assign is not translated correctly
	// cuz not asume the property owner
	caption = "Other Clicked";

	pushButton_2.width += pushButton_1.width;
	application.reset(600);
}

property target
{
	pushButton_2.caption = "target acquired " + value;
	
	for(int i = 0; i < 20; i++)
	{
		pushButton_2.width = pushButton_2.width + 10;
		if (pushButton_2.width > 200)
			pushButton_2.width = 100;
	}
}

method reset(string s)
{
	target = s;
	pushButton_1.caption = "...";
	pushButton_2.caption = "..." + target;
}

on pushButton_2.click()
{
	pushButton_1.caption = "Old value: " + application.target;
	application.target = 300;
}
