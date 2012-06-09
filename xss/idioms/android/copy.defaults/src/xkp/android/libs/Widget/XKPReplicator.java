<xss:parameter id="base_namespace"/>

package <xss:e value="base_namespace"/>.libs.Widget;

import java.util.ArrayList;

import android.content.Context;
import android.content.res.TypedArray;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import <xss:e value="base_namespace"/>.libs.Layout.XKPLayout;
import <xss:e value="base_namespace"/>.<xss:e value="application.appName"/>.R;

public class XKPReplicator extends XKPLayout {
	
	// start id of new components created with replicator
	static private final int 		ID_START = 0x7f0A0000;
	// consecutive components id created with replicator
	static private int 				mChildCount = ID_START;
	
	protected OnItemAssigner 		mOnItemAssigner;
	
	private int						mTemplateResource;
	private Items				 	mItems;

	public XKPReplicator(Context context) {
		super(context);
	}

	public XKPReplicator(Context context, AttributeSet attrs) {
		super(context, attrs);
		
		TypedArray a = context.obtainStyledAttributes(attrs, R.styleable.XKPReplicator);
		mTemplateResource = a.getResourceId(R.styleable.XKPReplicator_template, 0);
		a.recycle();
	}
	
	// methods to handle templates
	public int getTemplateResource() {
		return mTemplateResource;
	}
	
	public void setTemplateResource(int resource) {
		mTemplateResource = resource;
		updateTemplate();
	}
	
	public void updateTemplate() {
		LayoutInflater linflater = (LayoutInflater) this.getContext().
			getSystemService(Context.LAYOUT_INFLATER_SERVICE);
		
		// this is very inefficient, 
		// but live is hard and very long :)
		this.removeAllViews();
	
		for(int i = 0; i < mItems.size(); i++, mChildCount++) {
			View template = linflater.inflate(mTemplateResource, this);
			template.setId(mChildCount);
			assignItem(template);
		}
	}
	
	//method to handle items
	public void setItems(Items items) {
		mItems = items;
		updateTemplate();
	}
	
	public Items getItems() {
		return mItems;
	}
	
	public boolean assignItem(Object i) {
		
		if (mOnItemAssigner != null) {
			mOnItemAssigner.OnAssign(i);
			return true;
		}
		
		return false;
	}
	
	public void setOnItemAssigner(OnItemAssigner i) {
		mOnItemAssigner = i;
	}

	public interface OnItemAssigner {
		void OnAssign(Object item);
	}
	
	public Items generateItems() {
		return new Items();
	}
	
	public Items generateItems(Items items) {
		return new Items(items);
	}
	
	public static class Items {
		private ArrayList<XKPObject> mItems;
		
		public Items() {
			mItems = new ArrayList<XKPObject>();
		}
		
		public Items(Items items) {
			this.mItems = items.getItems();
		}
		
		public void setItems(Items items) {
			this.mItems = items.getItems();
		}
		
		public void setItems(ArrayList<XKPObject> items) {
			this.mItems = items;
		}
		
		public void addItem(XKPObject object) {
			this.mItems.add(object);
		}
		
		public void deleteItem(int index) {
			ArrayList<XKPObject> newList = new ArrayList<XKPObject>();
			
			for(int i = 0; i < mItems.size(); i++) {
				if(i == index)
					continue;
				
				newList.add(mItems.get(i));
			}
			
			this.mItems = newList;
		}
		
		public ArrayList<XKPObject> getItems() {
			return mItems;
		}

		public int size() {
			return this.mItems.size();
		}
	}
}
