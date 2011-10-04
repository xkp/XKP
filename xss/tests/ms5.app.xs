
property clicks = 0;

on btnNext.click()
{
    if (main.active < main.count - 1)
        main.active++;
	btnNext.rotation += 0.2;
}

on btnPrev.click()
{
    if (main.active > 0)
        main.active--;	
}

on btn1.click()
{
    lbl1.caption = "Got " + clicks++ + " smileys";
	btn1.rotation += 0.2;
}

on btn2.click()
{
    pb1.value--;
}

on btn3.click()
{
    pb1.value++;
}

