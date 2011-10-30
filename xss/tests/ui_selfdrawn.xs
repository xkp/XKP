instance sd1
{
	method reset()
    {
    }
    
    on paint(context)
	{
        reset();
    	
		context.fillStyle = 'cyan';
        context.fillRect(-100, -100, 200, 200);
		context.fillStyle = 'green';
        context.beginPath();
        context.lineWidth = 3;
        context.moveTo(-50, -50);
        context.lineTo(50, 50);
        context.stroke();
	}
}
