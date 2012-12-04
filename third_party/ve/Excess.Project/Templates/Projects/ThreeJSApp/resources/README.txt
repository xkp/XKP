Add your resource files here          

The resources you want to have available at run-time have to be placed in this "resources" folder, this is assumed, so the path of every resource include is considered
to be relative to the resources folder. For example, if you created an "image" dir inside the "resources" folder and uploaded the
image "example.png" you will include it like this in your application xml file:
	
<resources>
    <image src="images/example.png"/>  		
</resources>