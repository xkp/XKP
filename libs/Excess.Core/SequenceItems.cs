using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Excess.Core
{
    struct Key
    {
        public Key(double _t, object _value)
        {
            t = _t;
            value = _value;
        }

        public double t;
        public object value;
    }

    class Interpolator : SequenceHandler
    {
        public delegate void UpdateEvent(Interpolator sender, object value);

        public event UpdateEvent onUpdate;

        public Interpolator(IInterpolator interpolator)
        { 
            interpolator_ = interpolator;
        }

        public void addKey(double t, Object value)
        {
            keys_.Add(new Key(t, value));
        }

        public void setKeyValue(int index, Object value)
        {
            Key k = this.keys_[index];
            k.value = value;
        }

        public bool update(double t, double pt)
        {
            if (keys_.Count < 2)
                return true; //finished

            for (int i = 1; i < keys_.Count; i++)
            {
                Key k1 = keys_[i - 1];
                Key k2 = keys_[i];

                if (k1.t <= t && k2.t > t)
                {
                    double tt = (t - k1.t) / (k2.t - k1.t);
                    //double value = this.interpolate(k1.value, k2.value, tt);
                    object value = interpolator_.interpolate(k1.value, k2.value, tt);
                    if (onUpdate != null)
                        onUpdate(this, value);
                    
                    return false;
                }
            }

            return true; //finished
        }

        private IInterpolator interpolator_;
        public List<Key> keys_ = new List<Key>();
    }

    class Every : SequenceHandler
    {
        public delegate void UpdateEvent();

        public event UpdateEvent onUpdate;

        public Every(double frequency)
        {
            this.freq_ = frequency;
            this.wait_ = frequency;
        }

        public bool update(double t, double pt)
        {
    	    this.wait_ -= (t - pt);
            if (this.wait_ < 0)
            {
        	    this.wait_ = this.freq_;
                if (onUpdate != null)
                    onUpdate();
       	    }
        
            return false;
        }

        private double freq_;
        private double wait_;
    }

    class Caller : SequenceHandler
    {
        public delegate void UpdateEvent();

        public event UpdateEvent onUpdate;

        public Caller(double time)
        {
            time_ = time;
        }

        public bool update(double t, double pt)
        {
            if (pt > this.time_)
                return true;

            if (t < this.time_)
                return false;

            if (onUpdate != null)
                onUpdate();

            return true;
        }

        private double time_;
    }
}
