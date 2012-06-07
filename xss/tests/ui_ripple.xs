
instance rain
{
    property probablity = 0.8;

    void drop()
    {
        if (Math.random() < probablity)
        {
            float x = Math.random() * ripple1.width;
            float y = Math.random() * ripple1.height;
 		    ripple1.disturb(x, y);			
        }
    }
}
