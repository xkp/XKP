property counter = 0;

on btn1.click()
{
	label1.caption = "Clicked " + application.counter++;
}

on btn2.change()
{
	pbar.value += 10;

	if (switch1.active < switch1.count - 1)
		switch1.active++;
	else
		switch1.active = 0;
}

