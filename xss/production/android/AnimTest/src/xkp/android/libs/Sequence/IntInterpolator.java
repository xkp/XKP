package xkp.android.libs.Sequence;
public class IntInterpolator implements IInterpolator
{
	public Object interpolate(Object from, Object to, double t) 
	{
		Double v1 = (Double)from;
		Double v2 = (Double)to;
		return (Integer)((int)(v1 + (v2 - v1)*t));
	}
}
