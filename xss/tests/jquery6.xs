
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

//controls
on brightness.change()
{
    transformer.b_filter.brightness = brightness.value;
    apply_filter(transformer.b_filter);
}

on constrast.change()
{
    transformer.b_filter.constrast = constrast.value/100;
    apply_filter(transformer.b_filter);
}

on red.change()
{
    transformer.c_filter.red = red.value/100;
    apply_filter(transformer.c_filter);
}

on blue.change()
{
    transformer.c_filter.blue = blue.value/100;
    apply_filter(transformer.c_filter);
}

on green.change()
{
    transformer.c_filter.green = green.value/100;
    apply_filter(transformer.c_filter);
}

on hue.change()
{
    transformer.hsl_filter.hue = hue.value;
    apply_filter(transformer.hsl_filter);
}

on saturation.change()
{
    transformer.hsl_filter.saturation = saturation.value;
    apply_filter(transformer.hsl_filter);
}

on lightness.change()
{
    transformer.hsl_filter.lightness = lightness.value;
    apply_filter(transformer.hsl_filter);
}

on noise_amount.change()
{
    transformer.n_filter.amount = noise_amount.value/100;
    apply_filter(transformer.n_filter);
}

on noise_strength.change()
{
    transformer.n_filter.strength = noise_strength.value/100;
    apply_filter(transformer.n_filter);
}

on mono.change()
{
    transformer.n_filter.mono = mono.value;
    apply_filter(transformer.n_filter);
}

on blur_apply.click()
{
    apply_filter(transformer.blur_filter);
}