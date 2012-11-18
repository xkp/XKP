<xss:parameter id="ns"/>
/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package <xss:e value="ns"/>;



/**
 *
 * @author Adrian
 */
public class SequenceUpdateEvent
{
	public double time;
	public double delta;
    
	public SequenceUpdateEvent(double t, double d)
    {
    	time  = t;
    	delta = d;
    }
}
