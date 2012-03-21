instance btnTouchme
{
	property MAX_DELTHA 	: double = 80.0;
	property FIRST_QUATER 	: double = Math.PI / 4;
	property SECOND_QUATER 	: double = 3 * Math.PI / 4;
	property THIRD_QUATER	: double = 5 * Math.PI / 4;
	property FOURTH_QUATER 	: double = 7 * Math.PI / 4;
	
	property mStartX 		: float;
	property mStartY 		: float;

	on touch(MotionEvent ev)
	{
		EActionMotionEvent action = ev.getAction();
		
		EPlacement pl = EPlacement.center;
		
		Display display = appManager.getDefaultDisplay();
		int dispWidth  = display.getWidth();
		int dispHeight = display.getHeight();
		
		float currentX = ev.getX();
		float currentY = dispHeight - ev.getY();

		//TODO: when implement switch structure control?
		if(action == EActionMotionEvent.down)
		{
			mStartX = currentX;
			mStartY = currentY;
		}
		else
		if(action == EActionMotionEvent.up)
		{
			float delthaX = currentX - mStartX;
			float delthaY = currentY - mStartY;
			
			double angle = Math.atan2(delthaY, delthaX);
			if(angle < 0) angle += 2 * Math.PI;
			
			//ERROR: op_dot is not replaced for op_dot_call correctly inside others expressions
			//float max_deltha = Math.max(Math.abs(delthaX), Math.abs(delthaY));
			
			float abs_delthaX = Math.abs(delthaX);
			float abs_delthaY = Math.abs(delthaY);
			float max_deltha = Math.max(abs_delthaX, abs_delthaY);
			
			if(max_deltha < MAX_DELTHA / 10) 
			{
				btnTouchme.placement = pl;
				return true;
			}
			else
			if(max_deltha < MAX_DELTHA)
			{
				return true;
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
		
		return true;
	}
}
