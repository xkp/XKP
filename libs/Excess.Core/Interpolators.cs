using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Excess.Core
{
    interface IInterpolator
    {
        object interpolate(object from, object to, double t);
    }

    public class IntInterpolator : IInterpolator
    {
        public object interpolate(object from, object to, double t)
        {
            int v1 = (int)from;
            int v2 = (int)to;
            return (int)(v1 + (v2 - v1) * t);
        }
    }

    public class FloatInterpolator : IInterpolator
    {
        public object interpolate(object from, object to, double t)
        {
            float v1 = (float)from;
            float v2 = (float)to;
            return (float)(v1 + (v2 - v1) * t);
        }
    }

    public class DoubleInterpolator : IInterpolator
    {
        public object interpolate(object from, object to, double t)
        {
            double v1 = (double)from;
            double v2 = (double)to;
            return (double)(v1 + (v2 - v1) * t);
        }
    }
}
