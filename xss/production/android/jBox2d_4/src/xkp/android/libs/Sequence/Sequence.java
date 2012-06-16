package xkp.android.libs.Sequence;
import java.util.ArrayList;
import java.util.Timer;
/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
/**
 *
 * @author Adrian
 */
public class Sequence 
{
    public EventHolder events = new EventHolder();
	public Sequence()
    {
    }
    public void setParent(Sequence seq)
    {
        this.parent_ = seq;
    }
    public boolean getLoop()
    {
        return this.loop_;
    }
    public void setLoop(boolean value)
    {
        this.loop_ = value; 
    }
    public void start()
    {
        if (this.running_)
            return;
        if (this.parent_ == null)
            Manager.getInstance().add(this);  
        this.running_ = true;
        this.time_    = 0;
        this.events.dispatch("start", null);
    }
    public void stop()
    {
        this.running_ = false;
        this.time_ 	  = 0;
        if (this.parent_ == null)
            Manager.getInstance().remove(this);
        this.events.dispatch("stop", null);
    }
    public void update(double t)
    {
    	if (!this.running_)
    		return;	
    	this.events.dispatch("update", new SequenceUpdateEvent(this.time_ + t, t));
    	double pt = this.time_;
        this.time_ += t;
        boolean finished = true;
        for(Handler h : this.handlers_)
		{
        	boolean hf = h.update(this.time_, pt);
        	finished = finished && hf; 
		}
        if (finished)
        {
        	if (this.loop_)
        		this.time_ = 0;
        	else
        		this.stop();
        }
    }
    public void addHandler(Handler hand)
    {
    	if(this.handlers_.contains(hand))
        	return; //fail safe
    	this.handlers_.add(hand);
    }
    private Sequence           parent_   = null;
    private ArrayList<Handler> handlers_ = new ArrayList<Handler>();
    private boolean            running_  = false;
    private double 	           time_	 = 0;
    private boolean            loop_     = false;  
}
