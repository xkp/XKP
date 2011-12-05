package principal;

import java.util.ArrayList;



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

    public ArrayList sequences= new ArrayList();
    
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
		    Sequence seq=   (Sequence)this.sequences.get(i);
                        seq.update(t);
		}
    }

    private Timer timer;

    class UpdateTask extends TimerTask 
    {
        public UpdateTask(Manager owner)
        {
            owner_ = ownmer;
        }

        public void run() 
        {
            owner_.update(owner_.frequence);
        }

        private Manager owner_;
    }
  }
    public void start(double freq)
    {
        timer = new Timer();
        timer.schedule(new UpdateTask(), 0, freq * 1000); //subsequent rate
    }
}
