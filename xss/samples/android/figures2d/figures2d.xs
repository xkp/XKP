on p1.click()
{
	for(int i = 0; i < p1.points_count; i++)
	{
		int new_x;
		int new_y;
		if(i % 2 == 0)
		{
			new_x = p1.get_point_x(i) + 5;
			new_y = p1.get_point_y(i) + 5;
		}
		else
		{
			new_x = p1.get_point_x(i) - 5;
			new_y = p1.get_point_y(i) - 5;
		}
		
		p1.setPointbyIndex(i, new_x, new_y);
	}
}

on r1.click()
{
	r1.stroke = "brown";
}

on c1.click()
{
	c1.line_width++;
}
