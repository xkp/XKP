
on render_initialization()
{
    out()
    {
        var child_process  = require("child_process");
    }
}

method render_command(commands, idx, callback, error_handler)
{
    var    command  = commands[idx];
    string var_name = compiler.genid("cmd");
    out()
    {
        var <xss:e v="var_name"/> = <xss:e v="command.text.render_as_expr()"/>;
        child_process.exec(<xss:e v="var_name"/>, function(error, stdout, stderr)<xss:open_brace/>
            if (error)
    }

    if (error_handler)
        out() { {<xss:e v="error_handler"/>(error); return;}}
    else
        out() { throw error;}

    if (command.variable)
        out() { <xss:e v="command.variable"/> = stdout; }            

    bool last = idx == commands.size - 1;
    if (last)
        out() {<xss:e v="callback"/>;}
    else
        render_command(commands, idx + 1, callback);

    out()
    {
        <xss:close_brace/>);
    }
}

on render_commands(array commands, string callback)
{
    render_command(commands, 0, callback);
}
