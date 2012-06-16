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
public class Caller implements Handler
{
	public Caller(double time)
	{
		this.time_ = time;    
   	}
   	public void setCaller(Runnable caller)
	{
   		this.caller_ = caller;
	}
   	public boolean update(double t, double pt)
    {
    	if (pt > this.time_)
            return true;
    	if (t < this.time_)
            return false;
        this.caller_.run();        
        return true;
    }
	private double   time_;
	private Runnable caller_;
}
