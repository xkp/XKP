package xkp.android.lib;

import java.util.ArrayList;
import java.util.Timer;
import java.util.TimerTask;



/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author Adrian
 */

public  class Manager 
{
    private static Manager _instance;
    
 
    public double frequence = 1/30.0;
    
    private Manager() 
    {  
    }
 
    public static synchronized Manager getInstance() 
    {
        if (null == _instance) 
        {
            _instance = new Manager();
        }

        return _instance;
    }

    public ArrayList<Sequence> sequences= new ArrayList<Sequence>();
    
    public void add(Sequence sequence)
    {
        if(sequences.contains(sequence))
            return; //fail safe

        this.sequences.add(sequence);
    }

    public void remove(Sequence sequence)
    {
        sequences.remove(sequence);
    }

    public void update(double t)
    {
        for(int i = 0; i < this.sequences.size(); i++)
		{
		    Sequence seq = (Sequence)this.sequences.get(i);
            seq.update(t);
		}
    }
    
    public IInterpolator interpolator(String typename)
    {
    	if (typename.equals("Integer"))
    		return new NumericInterpolator<Integer>();	
    	if (typename.equals("Float"))
    		return new NumericInterpolator<Float>();	
    	if (typename.equals("Double"))
    		return new NumericInterpolator<Double>();
    	
    	return null;
    }

    private Timer timer;

    class UpdateTask extends TimerTask 
    {
        public UpdateTask(Manager owner)
        {
            owner_ = owner;
        }

        public void run() 
        {
            owner_.update(owner_.frequence*1000);
        }

        private Manager owner_;
    }

    public void start(double freq)
    {
    	frequence = freq;
        timer = new Timer();
        timer.schedule(new UpdateTask(this), 0, (long)(freq * 1000)); 
    }
}
