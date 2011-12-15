package xkp.android.lib;

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
public class Sequence {
    public Sequence parent_sequence;
    public ArrayList  methods=new ArrayList();
    public IExecutable OnStart=null;
    public ArrayList OnUpdate=new ArrayList();
    public ArrayList handlers=new ArrayList();
    public boolean running= false;
    public double time=0;
    public Sequence parent= null;

    public Sequence()
    {
    }

    public void start()
    {
        if (running)
            return;
        
        if (parent==null)
            Manager.getInstance().add(this);  

        this.running = true;
        this.time = 0;
        
        if(OnStart!=null)
        {
           OnStart.execute();
        }
    }

    public void stop(){
        this.running=false;
        this.time = 0;
        if (this.parent==null){
            Manager.getInstance().remove(this);
        }
    }
    public void update(double t){

      if (this.running)
        {
           if(!this.OnUpdate.isEmpty()){
               for(int i = 0; i < this.handlers.size(); i++)
		{
			Handler h =(Handler) this.handlers.get(i);
			for(int j=0;j<OnUpdate.size();j++){
                        h.OnUpdate.add(this.OnUpdate.get(j));}
		}
          }
            double pt = this.time;
            this.time += t/1000.0;
            
            for(int i = 0; i < this.handlers.size(); i++)
		{
			Handler h =(Handler) this.handlers.get(i);
                        //aqui hay q poner this.time= tiempo actual
			h.update(this.time, pt);
		}
        }
    }
    public void addHandler(Handler hand){
        if(this.handlers.contains(hand))
         return; //fail safe
        handlers.add(hand);
    }
    public void addOnStart(IExecutable os){
        OnStart=os;
    }
    public void addOnUpdate(OnUpdate ou){
        OnUpdate.add(ou);           
        
    }
     public void addMethod(IExecutable ie){
        this.methods.add(ie);

    }
}
