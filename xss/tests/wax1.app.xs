
on myPage.render()
{
    var number = Math.random();
    if (number > 0.7)
        return redirect(tpl = "wax1.tpl", target = number);

    var numbers = [Math.random(), Math.random(), Math.random()];

    rnumber.inner_html = number;
    rnumbers.data = numbers;

    smarty(html_id = "smarty_div", items = numbers)
    {
        <ul>
        {foreach from=$items item=rn}
            <li>{$rn}</li>
        {/foreach}
        </ul>    
    }
}

on myPage.rnumbers.render(item)
{
    arnumber.inner_html = item;
}

on myApp.render()
{
    btnFullName.value = Math.random();
}

method myService()
{
    return [Math.random(), Math.random(), Math.random()];
}