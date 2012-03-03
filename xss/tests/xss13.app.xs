
int p1;
property p2;
var p3 = 5;
private string p4;

method foo()
{
    int a = 0;
    int b = 2;
    int c = -1;

    switch()
    {
        case a > b:
        case b < c:
        {
            a += 10;
        }

        default:
        {
            a -= 10;
        }
    }

    switch(a)
    {
        case 1:
        case 2:
        {
            a += 10;
            break;
        }

        case 3:
        {
            a -= 10;
        }
    }
}

function bar(var xx)
{
    var yy = new object(25);
    object zz = object(25);

    try
    {
        some.dangerous();
    }
    catch(int id)
    {
        console.log("error: " + id);
    }
    catch
    {
        console.log("other error");
    }
    finally
    {
        console.log("finally!");
    }

    if (yy.z > 4)
        throw yy;
}

void foobar(string barfoo)
{
}