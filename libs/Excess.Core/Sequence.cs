using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Excess.Core
{
    public class SequenceManager
    {
        public static void add(Sequence seq)
        {
            sequences_.Add(seq);
        }

        public static void remove(Sequence seq)
        {
            sequences_.Remove(seq);
        }

        private static List<Sequence> sequences_ = new List<Sequence>();
    }

    public class SequenceUpdateEvent
    {
        public SequenceUpdateEvent(double time, double last, double elapsed)
        { 
            time_ = time;
            last_ = last;
            elapsed_ = elapsed;
        }

        public double Time
        {
            get { return time_; }
            set { time_ = value; }
        }

        public double LastTime
        {
            get { return last_; }
            set { last_ = value; }
        }

        public double Elapsed
        {
            get { return elapsed_; }
            set { elapsed_ = value; }
        }

        private double time_;
        private double last_;
        private double elapsed_;
    }

    public interface SequenceHandler
    {
        bool update(double time, double elapsed);
    }

    public class Sequence
    {
        public delegate void SequenceEvent(Sequence sender);
        public delegate void UpdateEvent(Sequence sender, SequenceUpdateEvent ev);

        public event SequenceEvent onStart;
        public event SequenceEvent onStop;
        public event UpdateEvent   onUpdate;

        public void setParent(Sequence seq)
        {
            this.parent_ = seq;
        }

        public bool loop
        {
            get {return loop_;}
            set {loop_ = value;}
        }

        public void start()
        {
            if (running_)
                return;

            if (parent_ == null)
                SequenceManager.add(this);

            this.running_ = true;
            this.time_ = 0;

            if (onStart != null)
                onStart(this);
        }

        public void stop()
        {
            running_ = false;
            time_ = 0;
            if (parent_ == null)
                SequenceManager.remove(this);

            if (onStop != null)
                onStop(this);
        }

        public void update(double t)
        {
    	    if (!running_)
    		    return;	
    	    
            if (onUpdate != null)
    	        onUpdate(this, new SequenceUpdateEvent(time_ + t, time_, t));
        
    	    double pt = this.time_;
            this.time_ += t;
            
            bool finished = true;
            foreach(SequenceHandler h in handlers_)
		    {
        	    bool hf = h.update(this.time_, pt);
        	    finished = finished && hf; 
		    }
        
            if (finished)
            {
        	    if (loop_)
        		    time_ = 0;
        	    else
        		    stop();
            }
        }

        public void addHandler(SequenceHandler hand)
        {
            if (handlers_.Contains(hand))
                return; //fail safe

            handlers_.Add(hand);
        }

        private Sequence              parent_   = null;
        private List<SequenceHandler> handlers_ = new List<SequenceHandler>();
        private bool                  running_  = false;
        private double                time_     = 0;
        private bool                  loop_     = false;
    }
}
