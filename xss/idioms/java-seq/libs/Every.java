<xss:parameter id="ns"/>
/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package <xss:e value="ns"/>;

import java.util.ArrayList;

/**
 *
 * @author Adrian
 */
public class Every implements Handler
{
	public Every(double frequency)
	{
		this.freq_ = frequency;    
		this.wait_ = frequency;
   	}
   
   	public void addCaller(Runnable caller)
	{
   		this.callers_.add(caller);
	}
    
   	public boolean update(double t, double pt)
    {
    	this.wait_ -= (t - pt);
        if (this.wait_ < 0)
        {
        	this.wait_ = this.freq_;
        	for(Runnable r : this.callers_)
        	{
        		r.run();
       		}
       	}
        
        return false;
    }

	private ArrayList<Runnable> callers_  = new ArrayList<Runnable>();
	private double 				freq_;
	private double 				wait_;
}
