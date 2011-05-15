on pushButton_1.click()
{
	// it's neccesary enum cast in c++
	var pl1 = pushButton_1.placement;
	Placement pl2 = Placement.left;
	var pl3 = Placement.right;
	Placement = Placement.top;

	pushButton_2.caption = "Clicked";
	caption = "Other Clicked";
	pushButton_2.width += pushButton_1.width;
	application.reset("600");
}

on pushButton_2.click()
{
	pushButton_1.caption = "Old value: " + application.target;
	application.target = "300";
	edtText.text = "btn2 clicked";
}

on pushButton_3.click()
{
	placement = (placement + 1) % 8;
	edtText.readonly = !edtText.readonly;
}

property xxx : int;

property target : string =
{
	return pushButton_2.caption;
}
{
	pushButton_2.caption = "target acquired " + value;

	for(int i = 0; i < 20; i++)
	{
		pushButton_2.width = pushButton_2.width + 10;
		if (pushButton_2.width > 200)
			pushButton_2.width = 100;
	}
}

property target1 : int = 
{
	return xxx;
}
{
	xxx = value;
}

method reset(string s)
{
	target = s;
	pushButton_1.caption = "...";
	pushButton_2.caption = "..." + target;
}

method returning_stuff()
{
	var i = 0;
	return i + 23;
}

// method declaring_error() : string
method declaring_error()
{
	var s = "hello";
	return s;
}

// method declaring_ok() : int
method declaring_ok(int value)
{
	return value;
}

// this is a very trick
// method declaring_trick(var value)
// {
	// return value;
// }

// method returned_real() : float
method returned_real()
{
	int pi = 3.14;
	return pi;
}
