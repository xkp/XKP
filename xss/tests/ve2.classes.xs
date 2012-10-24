class test_class
{
    method testing_properties()
    {
        for(var i in array_property)
        {
            string si = i;
            console.log(si);
        }

        component_property.name = child_object2.name;
        child_object2.name = child_object2.child_object.name;
    }
}
