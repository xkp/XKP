
method set_image(id)
{
    var image = document.getElementById(id);
    var target = document.getElementById("image_container");
    var context = target.getContext("2d");
    context.drawImage(image, 0, 0, target.width, target.height);
}

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

method apply_filter(filter)
{
    var imgdata = get_image_data();
    filter.applyToImage(imgdata);
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
    var image_data = get_image_data();
    preset_apply.enabled = false;
    preset_apply.caption = "Applying...";

    asynch(mypreset = transformer, image = image_data)
    {
        mypreset.apply(image);

        //this will run on the dom thread
        synch(image_data = image)
        {
            set_image_data(image_data);
            preset_apply.enabled = true;
            preset_apply.caption = "Apply Stack";
        }
    }
}

//controls
on brightness.change()
{
    transformer.b_filter.amount = brightness.value/100;
    apply_filter(transformer.b_filter);
}

on constrast.change()
{
    transformer.c_filter.amount = constrast.value/100;
    apply_filter(transformer.c_filter);
}

on sepia.change()
{
    transformer.s_filter.amount = sepia.value;
    apply_filter(transformer.s_filter);
}

on noise.change()
{
    transformer.n_filter.amount = noise.value;
    apply_filter(transformer.n_filter);
}

on density.change()
{
    transformer.n_filter.density = density.value/100;
    apply_filter(transformer.n_filter);

}

on mono.change()
{
    transformer.n_filter.monochrome = mono.value;
    apply_filter(transformer.n_filter);
}

on range.change()
{
    transformer.o_filter.range = range.value;
    apply_filter(transformer.o_filter);
}