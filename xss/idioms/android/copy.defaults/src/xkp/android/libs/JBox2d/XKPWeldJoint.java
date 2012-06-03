<xss:parameter id="base_namespace"/>

package <xss:e value="base_namespace"/>.libs.JBox2d;

import org.jbox2d.dynamics.World;
import org.jbox2d.dynamics.joints.WeldJoint;
import org.jbox2d.dynamics.joints.WeldJointDef;

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
	
	public void setBodyA(XKPPhysicBody bodyA) {
		mWeldJointD.bodyA = bodyA.getBody();
		if(mIsJointCreated)
			mWeldJoint.m_bodyA = bodyA.getBody();
	}
	
	public void setBodyB(XKPPhysicBody bodyB) {
		mWeldJointD.bodyB = bodyB.getBody();
		if(mIsJointCreated)
			mWeldJoint.m_bodyB = bodyB.getBody(); 
	}

	public void setRefAngle(float angle) {
		mWeldJointD.referenceAngle = angle;
	}
	
	public float getRefAngle() {
		return mWeldJointD.referenceAngle;
	}
}
