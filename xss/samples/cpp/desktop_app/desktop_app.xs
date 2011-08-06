on pushButton_1.click()
{
	// this make a deficient translation on second operand 
	// with placement property
	//placement = (placement + 1) % 7;
	pushButton_1.placement = (pushButton_1.placement + 1) % 7;
	
	caption = "Clicked";
	application.reset("600");
	
	pushButton_2.enabled = true;
}

on pushButton_2.click()
{
	pushButton_1.caption = "Old value: " + application.target;
	application.target += 10;
}

on pushButton_3.click()
{
	if(holder.x < 200)
		holder.x += 10;
}

on pushButton_4.click()
{
	if(holder.x > 10)
		holder.x -= 10;
}

on pushButton_5.click()
{
	edtText.text = "pushButton_5 is clicked";
	
	edtText.readonly = !edtText.readonly;
	if(edtText.readonly)
		label_1.caption = "editText is read only!";
	else
		label_1.caption = "editText is for write";
}

property xxx : int;

property target : int =
{
	return application.xxx;
}
{
	if(xxx < 200)
		xxx = value;
	else
		xxx = 60;

	pushButton_2.width = xxx;
	
	// "as" operator isn't translated
	//pushButton_2.caption = "target acquired " + xxx as string;
}

method reset(string s)
{
	target = 60;
	pushButton_1.caption = "...";
	pushButton_2.caption = "..." + target;
	edtText.clear();
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
