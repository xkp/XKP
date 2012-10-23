function XMLWriter()
{
    this.XML=[];
    this.Nodes=[];
    this.State="";
    this.FormatXML = function(Str)
    {
        if (Str)
            return Str.replace(/&/g, "&amp;").replace(/\"/g, "&quot;").replace(/</g, "&lt;").replace(/>/g, "&gt;");
        return ""
    }
    this.BeginNode = function(Name)
    {
        if (!Name) return;
        if (this.State=="beg") this.XML.push(">");
        this.State="beg";
        this.Nodes.push(Name);
        this.XML.push("<"+Name);
    }
    this.EndNode = function()
    {
        if (this.State=="beg")
        {
            this.XML.push("/>");
            this.Nodes.pop();
        }
        else if (this.Nodes.length>0)
            this.XML.push("</"+this.Nodes.pop()+">");
        this.State="";
    }
    this.Attrib = function(Name, Value)
    {
        if (this.State!="beg" || !Name) return;
        this.XML.push(" "+Name+"=\""+this.FormatXML(Value)+"\"");
    }
    this.WriteString = function(Value)
    {
        if (this.State=="beg") this.XML.push(">");
        this.XML.push(this.FormatXML(Value));
        this.State="";
    }
    this.Node = function(Name, Value)
    {
        if (!Name) return;
        if (this.State=="beg") this.XML.push(">");
        this.XML.push((Value=="" || !Value)?"<"+Name+"/>":"<"+Name+">"+this.FormatXML(Value)+"</"+Name+">");
        this.State="";
    }
    this.Close = function()
    {
        while (this.Nodes.length>0)
            this.EndNode();
        this.State="closed";
    }
    this.ToString = function(){return this.XML.join("");}
}
function genXML(data)
{
	try
	{
		var XML = new XMLWriter();
		XML.BeginNode("processing");
		if(data.scale)
		{
			XML.BeginNode("scale");
			XML.Attrib("x", data.scale.x);
			XML.Attrib("y", data.scale.y);
			XML.Attrib("z", data.scale.z);
			XML.EndNode();
		}
		if(data.translate)
		{
			XML.BeginNode("translate");
			XML.Attrib("x", data.translate.x);
			XML.Attrib("y", data.translate.y);
			XML.Attrib("z", data.translate.z);
			XML.EndNode();
		}
		if(data.rotate)
		{
			XML.BeginNode("rotate");
			XML.Attrib("x", data.rotate.x);
			XML.Attrib("y", data.rotate.y);
			XML.Attrib("z", data.rotate.z);
			XML.EndNode();
		}
		XML.EndNode();
		XML.Close();
		return XML.ToString().replace(/</g,"\n<");
	}
	catch(Err)
	{
		alert("Error: " + Err.description);
	}
	return false;
}