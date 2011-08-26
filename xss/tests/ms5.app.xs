
property clicks = 0;

on btnNext.click()
{
    if (main.active < main.count - 1)
        main.active++;
}

on btnPrev.click()
{
    if (main.active > 0)
        main.active--;
}

on btn1.click()
{
    lbl1.caption = "Got " + clicks++ + " smileys";
}

on btn2.click()
{
    pb1.value--;
}

on btn3.click()
{
    pb1.value++;
}

