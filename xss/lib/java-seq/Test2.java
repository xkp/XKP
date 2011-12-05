/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package principal;

import java.util.*;


/**
 *
 * @author Adrian
 */
public class Test2 {
Object target;
public number n=new number(2);

public void update(){
   Date start = new Date();

   System.out.print(" " + start.getSeconds()+" 1ra  ");
    
    this.target=n;
    Manager man=new Manager();
    Sequence seq1=new Sequence(man);
    Caller c=new Caller(new IExecutable(){
        public void execute(){
          System.out.print(" mierda ");
        }
    });

    System.out.print(n.num);
    Interpolator i=new Interpolator();
    i.setTarget(n);
    i.addKey(1, 2);
    i.addKey(4, 51);
    i.setAssign(new Assign(){
         public void execute(Object t,double value){
            /*number nu=(number)t;
            nu.num=value;*/
             n.num=value;
             t=n;
        }
    });
   /* i.defaultInterpolator(new Interpolators(){
         public double interpolate(double a,double b,double t){
            return a + (b - a)*t;
            }
    });*/
    i.defaultInterpolator(new Interpolators(){
             public double interpolate(Object from, Object to, double t) {
                 return (Double)from + ((Double)to - (Double)from)*t;
             }
     });
     IExecutable ie=new IExecutable(){
        public void execute(){
          System.out.print(" Iexcute ");
        }
    };
    seq1.addMethod(ie);
    int in=seq1.methods.indexOf(ie);
    IExecutable ie2=(IExecutable)seq1.methods.get(in);
    ie2.execute();
    seq1.start();
    Caller ca=new Caller(ie2);
    ca.setExecTime(3);
    Every ev=new Every(2);
    ev.setTarget(n);
    ev.addCaller(ca);
    seq1.addHandler(i);
    seq1.addHandler(ev);
    
   
    
    seq1.addOnStart(new IExecutable(){
        public void execute(){
          System.out.print(" OnStart ");
        }
    });
    
    seq1.addOnUpdate(new OnUpdate(){
         public void execute(){
            
          if(n.num==2){
            System.out.print(" OnUpdate1 ");
          }
        }
    });
     seq1.addOnUpdate(new OnUpdate(){
         public void execute(){
           
          if(n.num==2){
            System.out.print(" OnUpdate2 ");
          }
        }
    });
    
    man.add(seq1);
    seq1.start();    
    man.update(3000);
    number nu=(number)target;
    System.out.print(nu.num);
     //long start1 = new Date().getTime();
   
   
   
   
}
public static void main(String args[]) {
         final Test2 t=new Test2();

     Timer temp=new Timer();
        TimerTask tas=new TimerTask(){
            public void run(){
                t.update();
            }
        };
        GregorianCalendar cal=new GregorianCalendar(2001,1,1);
        temp.schedule(tas, cal.getTime(), 2000);

        
    }

}
