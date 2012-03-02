class move_action : sequence
{
    property speed  = 130.0;
    property vx     = Math.random();
    property vy     = Math.random();
    property target : img;

    on tick(delta)
    {
        target.x += vx*speed*delta;
        target.y += vy*speed*delta;

        if (target.x < 10 || target.x > 570)
            vx *= -1;

        if (target.y < 10 || target.y > 200)
            vy *= -1;
    }
}

class bullet : img
{
    on collision(who)
    {
        if (who is floor)
        {
            this.RemoveFromWorld();
			if(sound.active)
				SoundUtils.play("sounds/boom.wav");
        }
    }
}

class bad_guy : img
{
    instance fade_out
    {
        property target : bad_guy;
    }

    method init()
    {
        move_around.target = this;
        fade_out.target = this;
    }

    on collision(who)
    {
        if (who is bad_guy)
        {
            this.RemoveFromWorld(true);
	        cannon.stop();
            fade_out.start();
        }    
    }
}

class good_guy : img
{
    method init()
    {
        move_around.target = this;
    }

    on collision()
    {
        //we dead

    }
}