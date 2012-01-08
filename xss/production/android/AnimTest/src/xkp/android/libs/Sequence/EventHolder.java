/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package xkp.android.libs.Sequence;
import java.util.ArrayList;
import java.util.HashMap;
/**
 *
 * @author Adrian
 */
public class EventHolder
{
    public interface Implementor
    {
	    public void call(Object ev);
    }
    public void addListener(String id, Implementor listener)
	{
		ArrayList<Implementor> impl = events_.get(id);
		if (impl == null)
		{
			impl = new ArrayList<Implementor>();
			events_.put(id, impl);
		}
		impl.add(listener);
	}
	public void dispatch(String id, Object ev)
	{
		ArrayList<Implementor> impl = events_.get(id);
		if (impl != null)
		{
			for(Implementor ei : impl)
			{
				ei.call(ev);
			}
		}
	}
	private HashMap<String, ArrayList<Implementor>> events_ = new HashMap<String, ArrayList<Implementor>>();
}
