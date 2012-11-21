<xss:parameter id="ns"/>

package <xss:e value="ns"/>;

public class IntInterpolator implements IInterpolator
{

	public Object interpolate(Object from, Object to, double t) 
	{
		Integer v1 = (Integer)from;
		Integer v2 = (Integer)to;
		return (Integer)((int)(v1 + (v2 - v1)*t));
	}
}