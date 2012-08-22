{
	// simple declaration and then simple assign
	int firstvar = 1;
	firstvar = 2;
	
	firstvar += firstvar;
	++firstvar;
	
	// declaration with assign and then assign with cast
	string secondvar = "string value";
	secondvar = firstvar as string;
}
