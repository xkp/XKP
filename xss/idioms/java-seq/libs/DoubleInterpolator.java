<xss:parameter id="ns"/>

package <xss:e value="ns"/>;

public class DoubleInterpolator implements IInterpolator
{

	public Object interpolate(Object from, Object to, double t) 
	{
		Double v1 = (Double)from;
		Double v2 = (Double)to;
		return v1 + (v2 - v1)*t;
	}
}