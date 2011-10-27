package test;
import Animation_Library.jar;
import java.util.*;
import java.util.*;
import java.applet.*;
import java.awt.*;
import java.beans.*;
import java.io.*;
import java.lang.*;
import java.math.*;
import java.net.*;
import java.nio.*;
import java.rmi.*;
import java.security.*;
import java.sql.*;
import java.text.*;
public class Test {	
 public Object target;	
 public double g_elapsed = -1;
 public double g_delta = 0.0;
 public Manager g_sequence_manager=new Manager();
 //var application = {}  //td ?
 Sequence test_sequence;Sequence test_sequence1;
 Interpolators default_interpolate=new Interpolators(){
             public double interpolate(Object from, Object to, double t) {
                 return (Double)from + ((Double)to - (Double)from)*t;
             }
     };
 public void update(){
                target = 0.000000;
test_sequence =
new ms.state.Sequence(
);
	test_sequence.display = function() 
	{
		console.log("test_sequence> Target: " + target);
	};
	test_sequence.switch_sequence = function() 
	{
		stop();
test_sequence1.start();
	};
                test_sequence.start();
            test_sequence.parent_sequence = null;
            Every ____h1 = new Every(0.300000012);
			Caller ____c1=new Caller(new IExecutable(){
				public void execute(){
						int ____numb1=test_sequence.methods.indexOf(display);
						IExecutable ____ie1=(IExecutable)test_sequence.methods.get(____numb1);
						____ie1.execute();
            }
		});
			____h1 .addCaller(____c1);
					//target
                   Interpolator ____i1 = new Interpolator();
				   ____i1.setTarget(application);//sin la prop
				   ____i1.setAssign(new Assign(){
								public void execute(Object t,double value){
								Application application = (Application)t;
								application.target = value;
								}
								});
				   ____i1.defaultInterpolator(default_interpolate);	
						____i1.addKey(0, 0.300000012);
						____i1.addKey(3, 0.300000012);
				test_sequence.addHandler(____i1);
					//target
                   Interpolator ____i2 = new Interpolator();
				   ____i2.setTarget(application);//sin la prop
				   ____i2.setAssign(new Assign(){
								public void execute(Object t,double value){
								Application application = (Application)t;
								application.target = value;
								}
								});
				   ____i2.defaultInterpolator(default_interpolate);	
						____i2.addKey(3, 0.300000012);
						____i2.addKey(5, 10);
				test_sequence.addHandler(____i2);
					//target
                   Interpolator ____i3 = new Interpolator();
				   ____i3.setTarget(application);//sin la prop
				   ____i3.setAssign(new Assign(){
								public void execute(Object t,double value){
								Application application = (Application)t;
								application.target = value;
								}
								});
				   ____i3.defaultInterpolator(default_interpolate);	
						____i3.addKey(5, 10);
						____i3.addKey(15, 10000);
				test_sequence.addHandler(____i3);
                test_sequence.addOnUpdate(new OnUpdate(){
				public void execute()
                {
                    if (application.target > 5000)
                    {
						int ____numb2=test_sequence.methods.indexOf(switch_sequence);
						IExecutable ____ie2=(IExecutable)test_sequence.methods.get(____numb2);
						____ie2.execute();
                    }
                }
			});
test_sequence1 =
new ms.state.Sequence(
);
	test_sequence1.display = function() 
	{
		console.log("test_sequence1> Target: " + target);
	};
	test_sequence1.switch_sequence = function() 
	{
		stop();
test_sequence.start();
	};
            test_sequence1.parent_sequence = null;
            Every ____h2 = new Every(0.300000012);
			Caller ____c2=new Caller(new IExecutable(){
				public void execute(){
						int ____numb3=test_sequence1.methods.indexOf(shrink_target);
						IExecutable ____ie3=(IExecutable)test_sequence1.methods.get(____numb3);
						____ie3.execute();
						int ____numb3=test_sequence1.methods.indexOf(display);
						IExecutable ____ie3=(IExecutable)test_sequence1.methods.get(____numb3);
						____ie3.execute();
            }
		});
			____h2 .addCaller(____c2);
                test_sequence1.addOnUpdate(new OnUpdate(){
				public void execute()
                {
                    if (application.target < 0)
                    {
						int ____numb4=test_sequence1.methods.indexOf(switch_sequence);
						IExecutable ____ie4=(IExecutable)test_sequence1.methods.get(____numb4);
						____ie4.execute();
                    }
                }
			});
 }
public void start(){
     float now   = new Date().getTime();
				if (g_elapsed < 0)
					g_delta = 0; //first update
				else
					g_delta  = now - g_elapsed;
				g_elapsed = now;
				g_sequence_manager.update(g_delta);
 }
public static void main(String args[]) {
        final Test t=new Test();
		t.update();
		Timer temp=new Timer();
        TimerTask tas=new TimerTask(){
            public void run(){
               t.start();
            }
        };
        GregorianCalendar cal=new GregorianCalendar(2001,1,1);
        temp.schedule(tas, cal.getTime(), 1000);
    }
}
}
