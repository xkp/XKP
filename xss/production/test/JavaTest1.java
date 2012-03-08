import java.util.ArrayList;
import java.util.Arrays;
public class JavaTest1 {
			private Float prop_instance1_xvalue = 100.199997f;
			private Object prop_instance1_svalue;
			private Object prop_instance1_typing;
			private Integer prop_instance1_simple = 50;
			public Float prop_instance1_read_only_get() {
				return 10.000000f;
			}
			private Object prop_instance1_writable;
			public void prop_instance1_writable_set(Object value) {
				prop_instance1_writable = value;
				System.out.println(value);
			}
			public Integer prop_instance1_mixed_get() {
				return 10;
			}
			public void prop_instance1_mixed_set(Object value) {
				System.out.println("Something");
			}
		public Object mthd_instance1_test0(Integer value) {
			ArrayList xxx;
Integer a = value;
Float b = 5.000000f;
Integer c = 2;
Float d = 3.140000f;
return a * b - xxx.get(c);
		}
		public Float mthd_instance1_test1(Integer value) {
			Float b = 5.000000f;
return b / 2;
		}
		public String mthd_instance1_test2(Integer value) {
			Integer a = value;
Float b = 5.000000f;
return mthd_application_app_test(a * b);
		}
		public Object mthd_instance1_test3(Object value) {
			return value;
		}
			private Integer prop_application_lvalue;
			private String prop_application_yvalue = "string v";
			private ArrayList<String> prop_application_xvalue = new ArrayList(Arrays.asList( new Object [] {"value1", "value2", "value3"} ));
		public String mthd_application_app_test(Integer value) {
			return "string value";
		}
			private void createapplication() {
				ArrayList<Object> types = new ArrayList(Arrays.asList( new Object [] {new Object("int", "Integer"), new Object("float", "Double"), new Object("string", "String")} ));
Integer v0 = 20;
Integer v1 = instance1.px_value;
String v2 = "string";
Float v3 = 3.140000f;
Boolean v4 = true;
Object v5 = prop_instance1_xvalue;
Integer x0;
x0 = instance1.px_value;
Object x1;
x1 = somenotexistsobject.px_value;
instance1.ps_value = x0;
Integer x2;
x2 = instance1.ps_value;
prop_instance1_svalue = x0;
Object x3;
x3 = prop_instance1_svalue;
prop_application_lvalue = x0;
Integer x4;
x4 = prop_application_lvalue;
Integer ivar = 100;
String svar = String.valueOf(ivar);
Float fval = Float.valueOf(svar);
ArrayList<Integer> a0 = new ArrayList(Arrays.asList( new Object [] {1, 2} ));
ArrayList<Integer> a1 = new ArrayList(Arrays.asList( new Object [] {1, 2} ));
a1 = new ArrayList(Arrays.asList( new Object [] {3, 4, 5, 6, 7} ));
ArrayList<Integer> a2;
a2 = new ArrayList(Arrays.asList( new Object [] {1, 2, 3} ));
ArrayList<Integer> list = new ArrayList(Arrays.asList( new Object [] {1, 2, 3} ));
list.add(4);
list.remove(2);
list = new ArrayList(Arrays.asList( new Object [] {5, 6, 7, 8} ));
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
rmthd = mthd_application_app_test(10);
String thd = mthd_application_app_test(2);
Object mthd;
mthd = mthd_instance1_test0(10);
			}
	public static void main(String[] args) {
				application.create();
				}
}
