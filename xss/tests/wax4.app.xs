method service1()
{
    var data;

    sql()
    {
        data =  SELECT  * 
                FROM    DataTable
                WHERE   Column > 20    
    }    

    for(var d in data)
    {
        d.Column -= 20;
    }

    return d;
}
