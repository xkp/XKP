<xss:parameter id="base_namespace"/>

package <xss:e value="base_namespace"/>.libs.JBox2d;

import org.jbox2d.dynamics.World;
import org.jbox2d.dynamics.joints.WeldJoint;
import org.jbox2d.dynamics.joints.WeldJointDef;

import android.view.View;


public class XKPWeldJoint {
	
	private World 				mRefWorld;
	
	private WeldJoint			mWeldJoint;
	private WeldJointDef 		mWeldJointD;
	private boolean mIsJointCreated = false;
	
	public XKPWeldJoint(World world) {
		mRefWorld = world;
		mWeldJointD = new WeldJointDef();
	}
	
	public void createJoint() {
		mWeldJoint = (WeldJoint) mRefWorld.createJoint(mWeldJointD);
		mIsJointCreated = true;
	}
	
	public void setBodyA(Object bodyA) {
		XKPPhysicBody physicBodyA;
		if(bodyA instanceof XKPPhysicBody) {
			physicBodyA = (XKPPhysicBody) bodyA;
		} else
		if(bodyA instanceof View) {
			View v = (View) bodyA;
			physicBodyA = (XKPPhysicBody) v.getTag();
		} else 
			return;
		
		mWeldJointD.bodyA = physicBodyA.getBody();
		if (mIsJointCreated)
			mWeldJoint.m_bodyA = physicBodyA.getBody();
	}

	public void setBodyB(Object bodyB) {
		XKPPhysicBody physicBodyB;
		if(bodyB instanceof XKPPhysicBody) {
			physicBodyB = (XKPPhysicBody) bodyB;
		} else
		if(bodyB instanceof View) {
			View v = (View) bodyB;
			physicBodyB = (XKPPhysicBody) v.getTag();
		} else 
			return;
		
		mWeldJointD.bodyB = physicBodyB.getBody();
		if (mIsJointCreated)
			mWeldJoint.m_bodyB = physicBodyB.getBody();
	}

	public void setRefAngle(float angle) {
		mWeldJointD.referenceAngle = angle;
	}
	
	public float getRefAngle() {
		return mWeldJointD.referenceAngle;
	}
}
