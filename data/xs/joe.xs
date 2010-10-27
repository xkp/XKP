
property busy = false;

on shot.started
{
	joe.busy = true;
}

on shot.finished
{
	joe.busy = false;
}
