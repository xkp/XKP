/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package xkp.android.lib;



/**
 *
 * @author Adrian
 */
public class Caller extends Handler   {
    public double execTime=-1;
    public IExecutable fn;
    public Caller(IExecutable function){
        
        fn=function;
    }
    public void setTarget(Object obj){
        this.target=obj;
    }
    @Override
	public void update(double t, double ta){
       if(!this.OnUpdate.isEmpty())
         for(int i=0;i<this.OnUpdate.size();i++){
            OnUpdate w=(OnUpdate)this.OnUpdate.get(i);
             w.execute();

         }
      if(this.execTime>=0){
        if(this.execTime==t)
         fn.execute();
       }
       else
         fn.execute();

   }
    public void setExecTime(double t){
        execTime=t;
    }
}
