
class ms6 : div
{
    on btn1.click()
    {
        if (btn2.alpha > 0)
            btn2.alpha -= 0.1;
        else
            btn2.alpha = 1;
    }

    on btn2.click()
    {
        if (anim1.running)
            anim1.stop();
        else    
            anim1.start();
    }
}