
int p1;
property p2;
var p3 = 5;
private string p4;

method foo()
{
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
        }

        case 3:
        {
            a -= 10;
        }
    }
}

function bar(var xx)
{
}

void foobar(string barfoo)
{
}