on init()
{
	v.play();
}

on btnVolUp.click()
{	
	v.volume += 0.1;
}

on btnVolDown.click()
{	
	v.volume -= 0.1;
}

on btnMute.click()
{	
	v.mute();
}

on btnPlay.click()
{	
	v.play();
}

on btnPause.click()
{	
	v.pause();
}

on btnStop.click()
{	
	v.stop();
}

on btnRotLeft.click()
{	
	v.rotation -= 10;
}

on btnRotRight.click()
{	
	v.rotation += 10;
}
