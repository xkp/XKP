int CalculateFibonacci(int n)
{
    if (n == 0) return 0;
    if (n == 1) return 1;
    return CalculateFibonacci(n - 1) + CalculateFibonacci(n - 2);
}

method main()
{
	for(int i = 0; i < 100; i++)
	{
		var fibonacci = CalculateFibonacci(i);
		console.log("f(" + i + ") = " + fibonacci);
		int x;
	}
}