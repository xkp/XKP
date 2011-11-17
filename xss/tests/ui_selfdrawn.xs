instance sd1
{
	property ctx;
	property x = 50;
	property y = 50;
	property size = 20;
	property degrees = 30;
	property degreesList = [0,1,2,3,4,5,6,7,8,9,10,
							11,12,13,14,15,16,17,18,19,20,
							21,22,23,24,25,26,27,28,29];
	property i = 0;

	method reset()
    {
		ctx.clearRect(150,250,100,100); // clear canvas    
		var left = degreesList.slice(0, 1);
		var right = degreesList.slice(1, degreesList.length);
		degreesList = right.concat(left);  
    }
    
    method draw_all()
	{
        var c;
		var s;
		var e;
		var d = 0;

		if (i == 0) {
			reset();
		}		
		ctx.save();
		d = degreesList[i];
		c = Math.floor(255/degrees*i);
		ctx.strokeStyle = 'rgb(' + c + ', ' + c + ', ' + c + ')';
		ctx.lineWidth = size;
		ctx.beginPath();
		s = Math.floor(360/degrees*(d));
		e = Math.floor(360/degrees*(d+1)) - 1;
		ctx.arc(x, y, size, (Math.PI/180)*s, (Math.PI/180)*e, false);
		ctx.stroke();
		ctx.restore();
		i++;
		if (i >= degrees) {
			i = 0;
		}
	}
	
	on paint(context)
	{
		ctx = context;
		window.canvasTimer = window.setInterval(draw_all, 100);
	}
}
