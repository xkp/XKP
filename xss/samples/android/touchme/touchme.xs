instance btnTouchme
{
	property MAX_DELTHA 	: double = 80.0;
	property FIRST_QUATER 	: double = Math.PI / 4;
	property SECOND_QUATER 	: double = 3 * Math.PI / 4;
	property THIRD_QUATER	: double = 5 * Math.PI / 4;
	property FOURTH_QUATER 	: double = 7 * Math.PI / 4;
	
	property mStartX 		: double;
	property mStartY 		: double;

	on touch(MotionEvent ev)
	{
		EActionMotionEvent action = ev.getAction();
		
		EPlacement pl = EPlacement.center;
		
		//Display display = getWindowManager().getDefaultDisplay();
		//int dispHeight = display.getHeight();
		
		double currentX = ev.getX();
		//double currentY = dispHeight - ev.getY();
		//double currentY = getWindowManager().getDefaultDisplay().getHeight() - ev.getY();
		double currentY = ev.getY();

		//TODO: when implement switch structure control?
		if(action == EActionMotionEvent.down)
		{
			mStartX = currentX;
			mStartY = currentY;
		}
		else
		if(action == EActionMotionEvent.up)
		{
			double delthaX = currentX - mStartX;
			double delthaY = currentY - mStartY;
			
			double angle = Math.atan2(delthaY, delthaX);
			if(angle < 0) angle += 2 * Math.PI;
			
			//ERROR: op_dot is not replaced for op_dot_call correctly inside others expressions
			//double max_deltha = Math.max(Math.abs(delthaX), Math.abs(delthaY));
			double abs_delthaX = Math.abs(delthaX);
			double abs_delthaY = Math.abs(delthaY);
			double max_deltha = Math.max(abs_delthaX, abs_delthaY);
			
			if(max_deltha < MAX_DELTHA / 10) 
			{
				btnTouchme.placement = pl;
				return;
			}
			else
			if(max_deltha < MAX_DELTHA)
			{
				return;
			}
			
			if(angle >= FIRST_QUATER && angle <= SECOND_QUATER)
			{
				pl = EPlacement.top;
			}
			else
			if(angle >= SECOND_QUATER && angle <= THIRD_QUATER)
			{
				pl = EPlacement.left;
			}
			else
			if(angle >= THIRD_QUATER && angle <= FOURTH_QUATER)
			{
				pl = EPlacement.bottom;
			}
			else
			{
				pl = EPlacement.right;
			}
			
			placement = pl;
		}
	}
}
