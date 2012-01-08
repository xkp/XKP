on btn1.click()
{
    anim1.start();
}

on btn2.click()
{
    btn2.x = 40;
}

on anim1.start()
{
    lbl1.caption = "Button 1 Animating...";
}

on anim2.start()
{
    lbl2.caption = "Button 2 Chasing...";
}

on anim1.stop()
{
    lbl1.caption = "Button 1 Stopped";
    anim2.start();
}

on anim2.stop()
{
    lbl2.caption = "Button 2 Stopped";
}