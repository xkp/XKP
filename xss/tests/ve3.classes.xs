class testComponent : div
{
    const increment = 10;

    on btn1.click()
    {
        btn1.width--;
        btn1.height += increment;
    }
}

class testClass
{
    property x : int;

    method xx(int z, int r) : int
    {
        return x + z + r;
    }
}
