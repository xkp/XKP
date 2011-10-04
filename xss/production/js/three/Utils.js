function set_active_camera( camera )
{
	renderer.render(scene, camera);
	soundRenderer.render(scene, camera);
	active_camera = camera;
}

function default_interpolate(a, b, t)
{
    return a + (b - a)*t;
}

function get_intersects( x, y, object )
{
	var vector = new THREE.Vector3( ( x / window.innerWidth ) * 2 - 1, - ( y / window.innerHeight ) * 2 + 1, 0.5 );
	projector = new THREE.Projector();
	projector.unprojectVector( vector, active_camera );
	var ray = new THREE.Ray( active_camera.position, vector.subSelf( active_camera.position ).normalize() );
	var intersects = ray.intersectObject( object );	
	return intersects;
}

function generate_model_loader( type )
{
	if(type == RESOURCE_JSON_MODEL)
		return new THREE.JSONLoader();
	return new THREE.BinaryLoader();
}

var program_fill = function ( context ) 
{
	context.beginPath();
	context.arc( 0, 0, 1, 0, Math.PI * 2, true );
	context.closePath();
	context.fill();
}

var program_stroke = function ( context ) 
{
	context.beginPath();
	context.arc( 0, 0, 1, 0, Math.PI * 2, true );
	context.closePath();
	context.stroke();
}

var flat_shading = THREE.FlatShading;