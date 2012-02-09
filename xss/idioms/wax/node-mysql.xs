on render_initialization()
{
    out()
    {
        var mysql  = require("mysql");
    }
}

method render_statement(sts, idx, callback)
{
    var    st       = sts[idx];
    string var_name = compiler.genid("query");
    out()
    {
        var <xss:e v="var_name"/> = <xss:e v="st.text.render_as_expr()"/>;
        this.query(<xss:e v="var_name"/>).execute(function(error, rows, cols) <xss:open_brace/>
    }

    if (st.variable)
        out() { <xss:e v="st.variable"/> = rows; }            

    bool last = idx == sts.size - 1;
    if (last)
        out() {<xss:e v="callback"/>;}
    else
        render_statement(sts, idx + 1, callback);

    out()
    {
        <xss:close_brace/>);
    }
}

on render_statements(array statements, string callback)
{
    out()
    {
        new mysql.Database({
            hostname: 'localhost', 
            user: 'user', 
            password: 'password', 
            database: 'test'
        }).connect(function(error) <xss:open_brace/>
    }

    render_statement(statements, 0, callback);

    //close connect
    out()
    {
        <xss:close_brace/>);
    }
}
