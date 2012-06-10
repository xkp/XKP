property time;

on update()
{
	sphere1.rotation.y -= 0.5;
	sphere1.rotation.x -= 0.5;

	tetra1.rotation.y -= 0.5;
	tetra1.rotation.x -= 0.5;
	
	plane1.rotation.x -= 0.5;
	plane1.rotation.y -= 0.5;
	
	cube1.rotation.x -= 0.5;
	cube1.rotation.y -= 0.5;
	
	icosahedron1.rotation.x -= 0.5;
	icosahedron1.rotation.y -= 0.5;
	
	cylinder1.rotation.x -= 0.5;
	cylinder1.rotation.y -= 0.5;
	
	octa1.rotation.x -= 0.5;
	octa1.rotation.y -= 0.5;
	
	torus1.rotation.x -= 0.5;
	torus1.rotation.y -= 0.5;
	
	torusknot1.rotation.x -= 0.5;
	torusknot1.rotation.y -= 0.5;
}