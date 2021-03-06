<xss:parameter id="base_namespace"/>

package <xss:e value="base_namespace"/>.libs.Widget;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

public class XKPObject {
	private Map<String, Object> properties_;
	
	public XKPObject() {
		this.properties_ = new HashMap<String, Object>();
	}
	
	public XKPObject(Object obj) {
		this.properties_ = new HashMap<String, Object>();
		this.properties_.put("__default", obj);
	}
	
	public XKPObject(XKPObject obj) {
		this.properties_ = obj.properties_;
	}
	
	public void prop(String name, Object value) {
		this.properties_.put(name, value);
	}
	
	public Object prop(String name) {
		Iterator ite = this.properties_.entrySet().iterator();
		
		while (ite.hasNext()) {
			Map.Entry e = (Map.Entry) ite.next();
			
			if (e.getKey() == name) {
				return e.getValue();
			}
		}
		
		return null;
	}
	
	public Object prop() {
		return prop("__default");
	}
	
	public Boolean hasProperty(String name) {
		return this.prop(name) != null;
	}
}
