var current_image;
var image_data;

method get_image_data()
{
    var target = document.getElementById("image_container");
    var context = target.getContext("2d");
    return context.getImageData(0, 0, target.width, target.height);
}

method set_image_data(data)
{
    var target = document.getElementById("image_container");
    var context = target.getContext("2d");
    context.putImageData(data, 0, 0);
}

method set_image(id)
{
    current_image = id;

    var image = document.getElementById(id);
    var target = document.getElementById("image_container");
    var context = target.getContext("2d");
    context.drawImage(image, 0, 0, target.width, target.height);
    image_data = get_image_data();
}

on toolbox.change()
{
    if (current_image)
    {
        set_image(current_image);
        apply_filter();
    }
}

method apply_filter()
{
    set_image_data(image_data);     //restore original
    var imgdata = get_image_data(); //create a copy
    
    //get the filters from the current accordion page
    if (toolbox.active > 0)
    {
        var filter = transformer.filters[toolbox.active + 1];
        filter.applyToImage(imgdata);
    }
    else
    {
        //brightness and constrast are in the same page
        transformer.b_filter.applyToImage(imgdata);
        transformer.c_filter.applyToImage(imgdata);
    }
    
    set_image_data(imgdata);
}

on init()
{
    set_image("img2");

    native()
    {
        //some jquery, to lighten the mood
        $("#img1").click(function() { application.set_image("img1"); });
        $("#img2").click(function() { application.set_image("img2"); });
        $("#img3").click(function() { application.set_image("img3"); });
        $("#img4").click(function() { application.set_image("img4"); });
    }
}

//apply preset, in another thread
on preset_apply.click()
{
    var to_transform = image_data;
    preset_apply.enabled = false;
    preset_apply.caption = "Applying...";

    asynch(mypreset = transformer, image = to_transform)
    {
        mypreset.apply(image);

        //this will run on the dom thread
        synch(result = image)
        {
            set_image_data(result);
            preset_apply.enabled = true;
            preset_apply.caption = "Apply Stack";
        }
    }
}

//controls
on brightness.change()
{
    transformer.b_filter.amount = brightness.value/100;
    apply_filter();
}

on constrast.change()
{
    transformer.c_filter.amount = constrast.value/100;
    apply_filter();
}

on sepia.change()
{
    transformer.s_filter.amount = sepia.value;
    apply_filter();
}

on sepia_enabled.change()
{
    transformer.s_filter.enabled = sepia_enabled.checked;
}

on noise.change()
{
    transformer.n_filter.amount = noise.value;
    apply_filter();
}

on density.change()
{
    transformer.n_filter.density = density.value/100;
    apply_filter();

}

on mono.change()
{
    transformer.n_filter.monochrome = mono.checked;
    apply_filter();
}

on range.change()
{
    transformer.o_filter.range = range.value;
    apply_filter();
}