// 
// Java tests console
// This source is generated with xss:xkp
//
import java.util.ArrayList;
import java.util.Arrays;
public class JavaTest {
				private static Class_instance1 instance1;
	private static class Class_application {
				private Object lvalue 	; 
				private String yvalue  = "string v" 	; 
				private ArrayList<String> xvalue  = new ArrayList(Arrays.asList( new Object [] {"value1", "value2", "value3"} )) 	; 
		public String app_test(Integer value) {
			return "string value";
		}
			private void create() {
				Object o = new Object(2);
o.new_property = "value";
o.other_new_property = 3.1400001;
ArrayList<Object> types = new ArrayList(Arrays.asList( new Object [] {new Object("int", "Integer"), new Object("float", "Double"), new Object("string", "String")} ));
Integer v0 = 20;
Integer v1 = instance1.px_value;
String v2 = "string";
Double v3 = 3.1400001;
Boolean v4 = true;
Integer x0;
x0 = instance1.px_value;
Object x1;
x1 = somenotexistsobject.px_value;
instance1.ps_value = x0;
Object x2;
x2 = instance1.ps_value;
instance1.svalue = x0;
Object x3;
x3 = instance1.svalue;
lvalue = x0;
Object x4;
x4 = lvalue;
Integer ivar = 100;
String svar = String.valueOf(ivar);
Double fval = Double.valueOf(svar);
ArrayList<Integer> a0 = new ArrayList(Arrays.asList( new Object [] {1, 2} ));
a0.setValue(j);
ArrayList<Integer> a1 = new ArrayList(Arrays.asList( new Object [] {1, 2} ));
a1 = new ArrayList(Arrays.asList( new Object [] {3, 4, 5, 6, 7} ));
ArrayList<Integer> a2;
a2 = new ArrayList(Arrays.asList( new Object [] {1, 2, 3} ));
ArrayList<Integer> list = new ArrayList(Arrays.asList( new Object [] {1, 2, 3} ));
list.add(4);
list.remove(2);
list = new ArrayList(Arrays.asList( new Object [] {5, 6, 7, 8} ));
list.set(0, 2 * 3);
Integer itx = list.get(2);
Integer size = list.size();
for(Integer e : list)
{
	System.out.println(e);
}
Integer s0 = 20;
s0 = 5;
s0 = list.get(2);
Integer s1 = 0;
s1 = 1;
s1 += s0;
s1 -= 2;
Integer s2 = s1 + s0;
Integer s3 = s1 * s2;
if (s1 != 0)
{
	Integer s4 = s3 / s1;
}
Integer l1 = 2;
Integer l2 = 3;
Integer l3 = 3;
if (l1 == l2 && l2 == l3)
{
	System.out.println("equilateral");
}
else
{
	if (l1 != l2 && l2 != l3 && l3 != l1)
	{
		System.out.println("scalene");
	}
	else
	{
		System.out.println("isosceles");
	}
}
String rmthd;
rmthd = app_test(10);
String thd = application.app_test(2);
Double mthd;
mthd = instance1.test0(10);
mthd = instance1.test0(10, "a", -2);
mthd = instance1.test0(2 * 5, "b" + "a", 1);
			}
	}
	private static class Class_instance1 {
				private Double xvalue  = 100.199997 	; 
				private Object svalue 	; 
		public Double test0(Integer value) {
			Integer a = value;
Double b = 5;
return a * b;
		}
		public Double test1(Integer value) {
			Double b = 5;
return b / 2;
		}
		public String test2(Integer value) {
			Integer a = value;
Double b = 5;
return application.app_test(a * b);
		}
		public Object test3(Object value) {
			return value;
		}
		public void foo(Object obj) {
			Boolean has_property = obj has "processed";
if (!has_property)
{
	obj.processed = true;
}
		}
	}
	public static void main(String[] args) {
		application.create();
	}
}
