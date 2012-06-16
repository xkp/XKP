package null.null
public class custom_clazz extends JavaClass {
			private Integer x = 0;
			private Integer y = 0;
			private Integer width = 10;
			private Integer height = 10;
			public Integer radius_get() {
				return value;
			}
			public void radius_set(Integer value) {
				width = value * 2;
height = value * 2;
			}
		public void set_default() {
			width = 10;
height = 10;
		}
			private void java_event(Object value) {
				width = width + 10;
height = height + 10;
			}
}
