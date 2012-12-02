
array<int> fib = [];

int CalculateFibonacci(int n)
{
    if (n == 0) return 0;
    if (n == 1) return 1;
    
	if (fib[n] >= 0)
		return fib[n];

	int result = CalculateFibonacci(n - 1) + CalculateFibonacci(n - 2);
	fib[n] = result;  
	return result; 
}


method main()
{
	//init cache
	for(int i = 0; i < 46; i++) 
	{
		fib += -1;
	}

	for(int i = 0; i < 46; i++)
	{
		int my_thing = 3;
		var fibonacci = CalculateFibonacci(i);
		console.log("f(" + i + ") = " + fibonacci);
	}

	console.read();
}