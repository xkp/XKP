
on hero.collision()
{
    game_over.visible = true;
    hero.move_around.stop();
    hero.src = "images/smile_sad_48.png";
}

on keydown(keycode)
{
	if (keycode == LEFT_ARROW)
		hero.move_around.vx = -1;

	if (keycode == RIGHT_ARROW)
		hero.move_around.vx = 1;
}

on keyup(keycode)
{
	if (keycode == LEFT_ARROW && hero.move_around.vx < 0)
		hero.move_around.vx = 0;

	if (keycode == RIGHT_ARROW && hero.move_around.vx > 0)
		hero.move_around.vx = 0;
}
