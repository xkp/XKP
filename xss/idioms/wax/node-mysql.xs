on render_initialization()
{
    out()
    {
        var mysql  = require("mysql");
    }
}

method render_statement(sts, idx, callback, error_handler)
{
    var    st       = sts[idx];
    string var_name = compiler.genid("query");
    out()
    {
        var <xss:e v="var_name"/> = <xss:e v="st.text.render_as_expr()"/>;
        this.query(<xss:e v="var_name"/>).execute(function(error, rows, cols) <xss:open_brace/>
            if (error)
    }

    if (error_handler)
        out() { {<xss:e v="error_handler"/>(error); return;}}
    else
        out() { throw error;}

    if (st.variable)
    {
        if (st.resolve_to_object)
        {
            out()
            {
                if (rows.lenght == 0)
                    <xss:e v="st.variable"/> = null;
                else if (rows.lenght == 1)
                    <xss:e v="st.variable"/> = rows[0];
                else
                    throw {error: "Expecting a single value returned by query"};
            }
        }
        else if (st.resolve_to_value)
        {
            out()
            {
                if (rows.lenght == 0)
                    <xss:e v="st.variable"/> = null;
                else if (rows.lenght == 1)
                {
                    var __found = false;
                    var __obj = rows[0];
                    for(var __prop in __obj)
                    {
                        if (!__found)
                        {
                            __found = true;
                            <xss:e v="st.variable"/> = __obj[__prop];
                            break;
                        }
                    }

                    if (!__found)
                        <xss:e v="st.variable"/> = null;
                }
                else
                    throw {error: "Expecting a single value returned by query"};
            }
        }    
        else
        {
            out() { <xss:e v="st.variable"/> = rows; }            
        }
    }

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

on render_statements(array statements, string callback, string error_handler)
{
    out()
    {
        new mysql.Database({
            hostname: 'localhost', 
            user: 'user', 
            password: 'password', 
            database: 'test'
        }).connect(function(error) <xss:open_brace/>
            if (error)
    }

    if (error_handler)
        out() { {<xss:e v="error_handler"/>(error); return;}}
    else
        out() { throw error;}

    render_statement(statements, 0, callback, error_handler);

    //close connect
    out()
    {
        <xss:close_brace/>);
    }
}
