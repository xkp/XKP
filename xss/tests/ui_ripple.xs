
instance rain
{
    property probablity = 0.8;

    void drop()
    {
        if (Math.random() < probablity)
        {
            float x = 50 + Math.random() * (ripple1.width - 100);
            float y = 50 + Math.random() * (ripple1.height - 100);
 		    ripple1.disturb(x, y);			
        }
    }
}
