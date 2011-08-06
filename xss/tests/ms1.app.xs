
property target = 0.0;

instance test_sequence
{
	method display()
	{
		console.log("test_sequence> Target: " + application.target);
	}

	method switch_sequence()
	{
		stop();
		test_sequence1.start();
	}
}

instance test_sequence1
{
	method display()
	{
		console.log("test_sequence1> Target: " + application.target);
	}

	method switch_sequence()
	{
		stop();
		test_sequence.start();
	}
}