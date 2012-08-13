
on myPage.render()
{
    rnumber.inner_html = Math.random();

    var numbers = [Math.random(), Math.random(), Math.random()];
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

method myService()
{
    return [Math.random(), Math.random(), Math.random()];
}