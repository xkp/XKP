/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package xkp.android.lib;


import java.util.ArrayList;

/**
 *
 * @author Adrian
 */
public class Interpolator extends Handler
{
    public IInterpolator interpolator;
    public Assign        assign;
    public ArrayList     keys = new ArrayList();
    
    public void setTarget(Object obj)
    {
        this.target = obj;
    }

    public void addKey(double t, double value)
    {
        Key key1=new Key(t,value);
        keys.add(key1);
    }
    
    @Override
	public void update(double t,double pt)  
    {
        if (keys.isEmpty()|| this.keys.size() < 2)
            return; //fail

        for(int i = 1; i < this.keys.size(); i++)
        {
            Key k1 = (Key)this.keys.get(i-1);
            Key k2 = (Key)this.keys.get(i);

            if (k1.t <= t && k2.t > t)
            {
                double tt = (t - k1.t)/(k2.t - k1.t);
                //double value = this.interpolate(k1.value, k2.value, tt);
                Object value = this.interpolator.interpolate(k1.value, k2.value, tt);
                this.assign.execute(target, value);
                break;
            }
        }
    }

    public void setAssign(Assign a)
    {
        this.assign=a;
    }
    
    public void defaultInterpolator(IInterpolator i)
    {
        this.interpolator=i;
    }
}
