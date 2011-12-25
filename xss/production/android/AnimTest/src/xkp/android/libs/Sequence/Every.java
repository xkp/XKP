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
public class Every extends Handler{
  public ArrayList fn=new ArrayList();
  double freq;
  double wait;
   public Every(double frequency){
     freq=frequency;    
     wait=frequency;
 }
  public void addCaller(Caller function){
     fn.add(function);
 }
    @Override
 public void update(double t, double pt)
    {
     if(!this.OnUpdate.isEmpty()){
         for(int i=0;i<this.OnUpdate.size();i++){
            OnUpdate w=(OnUpdate)this.OnUpdate.get(i);
             w.execute();
         }
        }
        this.wait -= (t - pt);
        if (this.wait < 0)
        {
            this.wait = this.freq;
           for(int i=0;i<fn.size();i++){
            Caller c=(Caller)fn.get(i);
           c.update(t, pt);}
        }
    }
    public void setTarget(Object obj){
        this.target=obj;
    }
}
