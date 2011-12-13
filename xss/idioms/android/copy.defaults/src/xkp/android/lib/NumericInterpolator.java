package xkp.android.lib;

public class NumericInterpolator<T extends Number> implements IInterpolator
{

	public Object interpolate(Object from, Object to, double t) 
	{
		T v1 = (T)from;
		T v2 = (T)to;
		return (Number)(v1.doubleValue() + (v2.doubleValue() - v1.doubleValue())*t);
	}
}