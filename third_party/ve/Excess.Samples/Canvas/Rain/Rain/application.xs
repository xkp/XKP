instance rain
{
    property probablity = 0.8;

    method drop()
    {
        if (Math.random() < probablity)
        {
            float x = 50 + Math.random() * (rip.width - 100);
            float y = 50 + Math.random() * (rip.height - 100);
     	    rip.disturb(x, y);			
        }
    }
}

on click(x, y)
{
    rip.disturb(x, y);    
}