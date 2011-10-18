property curr_sound;

on btnSound2.click()
{	
	sound_2.pause();
	sound_2.currentTime = 0;
	curr_sound = sound_1;
	curr_sound.play();
}

on btnSound1.click()
{	
	sound_1.pause();
	sound_1.currentTime = 0;
	curr_sound = sound_2;
	curr_sound.play();
}

on btnPlay.click()
{
   curr_sound.play();
}

on btnStop.click()
{
    curr_sound.pause();
	curr_sound.currentTime = 0;
}

on btnPause.click()
{
    curr_sound.pause();	
}

on btn1.click()
{
    volume_bar.value -= 10;
	curr_sound.volume -= 0.1;
}

on btn2.click()
{
    volume_bar.value += 10;
	curr_sound.volume += 0.1;
}

