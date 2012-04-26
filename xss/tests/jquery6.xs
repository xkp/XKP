
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

on init()
{
    set_image("img2");

    native()
    {
        $("#img1").click(function() { application.set_image("img1"); });
        $("#img2").click(function() { application.set_image("img2"); });
        $("#img3").click(function() { application.set_image("img3"); });
        $("#img4").click(function() { application.set_image("img4"); });
    }
}