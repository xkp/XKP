var left     = {x: -500, y: 0, z: 0};
var right    = {x:  500, y: 0, z: 0};
var selected = imgg1;

on init()
{
    imgg1.images = ["i1",  "i2",  "i3",  "i4",  "i5",  "i6",  "i7",  "i8",  "i9"];  
    imgg2.images = ["i10", "i11", "i12", "i13", "i14", "i15", "i16", "i17", "i18"];  
    imgg3.images = ["i19", "i20", "i21", "i22", "i23", "i24", "i25", "i26", "i27"];  
}

method select(group)
{
    if (selected != group)
    {
        if (selected)
        {
            if (group.position.x > 0)
                selected.destination = right;
            else   
                selected.destination = left;

            selected.move.start();
            selected.shrink.start();
        }

        selected = group;
        selected.destination = {x: 0, y: 0, z: 0};
        selected.move.start();
        selected.grow.start();
    }
}
