
property click_count = 0;

on btn1.click()
{
    anim1.start();
}

on btn2.click()
{
    btn2.x = 40;
}

on btn3.click()
{
    click_count++;
    if (click_count == 4)
    {
        click_count = 0;
        anim3.start();
    }
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

instance anim3
{
    method stop_anim3()
    {
        lbl3.caption = "Button 3 Stopped";
        stop();
    }
}

/*
instance anim4
{
    property clicks = 0;

    method update_button()
    {
        clicks++;
        btn3.caption = clicks as string;
    }

    on update(ev)
    {
    }
}
*/