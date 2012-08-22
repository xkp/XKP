method compress()
{
    var uploaded_file = post.files[0].path;
    shell()
    {
        gzip @uploaded_file
    }

    var filename = post.files[0].name;
    var url = "compressed/" + filename + ".gz";
    fs.rename(uploaded_file + ".gz", 'files/' + url);

    return {success: true, url : url};
}
