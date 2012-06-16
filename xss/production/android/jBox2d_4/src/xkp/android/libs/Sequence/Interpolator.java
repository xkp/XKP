/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package xkp.android.libs.Sequence;
import java.util.ArrayList;
/**
 *
 * @author Adrian
 */
public class Interpolator implements Handler
{
	public Interpolator()
	{
	}
    public void setAssigner(Assign assign)
    {
        this.assign_ = assign;
    }
    public void setInterpolator(IInterpolator interpolator)
    {
        this.interpolator_ = interpolator;
    }
    public void addKey(double t, Object value)
    {
        this.keys_.add(new Key(t, value));
    }
    public void setKeyValue(int index, Object value)
    {
        Key k = this.keys_.get(index);
        k.value = value;
    }
    public boolean update(double t, double pt)  
    {
        if (this.keys_.size() < 2)
            return true; //finished
        for(int i = 1; i < this.keys_.size(); i++)
        {
            Key k1 = this.keys_.get(i-1);
            Key k2 = this.keys_.get(i);
            if (k1.t <= t && k2.t > t)
            {
                double tt = (t - k1.t)/(k2.t - k1.t);
                //double value = this.interpolate(k1.value, k2.value, tt);
                Object value = this.interpolator_.interpolate(k1.value, k2.value, tt);
                this.assign_.put(value);
                return false;
            }
        }
        return true; //finished
    }
    private IInterpolator  interpolator_;
    private Assign         assign_;
    public 	ArrayList<Key> keys_ = new ArrayList<Key>();
}
