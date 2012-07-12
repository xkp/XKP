on btnFullName.click()
{
    ajax()
    {
        lblFullName.caption = "Grabbing ";
        var loc = google_maps.geocode(address="780 NE 69st", sensor=false);    
        lblFullName.caption = loc.geometry.location.Lat + " " + loc.geometry.location.Long;
    }
}
