
on dp1.select(dateText, inst)
{
    alert(dateText);
}

on dp2.beforeShow()
{
    dp1.visible = false;
}

on dp2.close()
{
    dp1.visible = true;
}

on btn1.click()
{
    simple_dialog.show();
}

on btn2.click()
{
    native()
    {
        alert("Hello, World");
    }
}

on sld1.change()
{
    pb1.value = sld1.value;
}

on acc1.change()
{
    tc1.selected = acc1.active;
}

on tc1.show()
{
    acc1.active = tc1.selected;
}