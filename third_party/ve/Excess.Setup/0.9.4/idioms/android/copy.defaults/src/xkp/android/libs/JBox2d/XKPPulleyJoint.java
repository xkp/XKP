<xss:parameter id="base_namespace"/>

package <xss:e value="base_namespace"/>.libs.JBox2d;

import org.jbox2d.common.Vec2;
import org.jbox2d.dynamics.World;
import org.jbox2d.dynamics.joints.PulleyJoint;
import org.jbox2d.dynamics.joints.PulleyJointDef;

import android.view.View;

public class XKPPulleyJoint {
	
	private World 				mRefWorld;
	
	private PulleyJoint			mPulleyJoint;
	private PulleyJointDef 		mPulleyJointD;
	private boolean mIsJointCreated = false;
	
	public XKPPulleyJoint(World world) {
		mRefWorld = world;
		mPulleyJointD = new PulleyJointDef();
	}
	
	public void createJoint() {
		mPulleyJoint = (PulleyJoint) mRefWorld.createJoint(mPulleyJointD);
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
		
		mPulleyJointD.bodyA = physicBodyA.getBody();
		if (mIsJointCreated)
			mPulleyJoint.m_bodyA = physicBodyA.getBody();
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
		
		mPulleyJointD.bodyB = physicBodyB.getBody();
		if (mIsJointCreated)
			mPulleyJoint.m_bodyB = physicBodyB.getBody();
	}

	public void setGroundAnchorA(Vec2 anchor) {
		mPulleyJointD.groundAnchorA.set(anchor);
		if(mIsJointCreated)
			mPulleyJoint.getGroundAnchorA().set(anchor);
	}
	
	public Vec2 getGroundAnchorA() {
		Vec2 result = mPulleyJointD.groundAnchorA;
		if(mIsJointCreated)
			result = mPulleyJoint.getGroundAnchorA();
		
		if(result == null)
			result = new Vec2();
		
		return result;
	}
	
	public void setGroundAnchorB(Vec2 anchor) {
		mPulleyJointD.groundAnchorB.set(anchor);
		if(mIsJointCreated)
			mPulleyJoint.getGroundAnchorB().set(anchor);
	}
	
	public Vec2 getGroundAnchorB() {
		Vec2 result = mPulleyJointD.groundAnchorB;
		if(mIsJointCreated)
			result = mPulleyJoint.getGroundAnchorB();
		
		if(result == null)
			result = new Vec2();
		
		return result;
	}
	
	public void setLengthA(float length) {
		mPulleyJointD.lengthA = length;
	}
	
	public float getLengthA() {
		return mPulleyJointD.lengthA;
	}
	
	public void setLengthB(float length) {
		mPulleyJointD.lengthB = length;
	}
	
	public float getLengthB() {
		return mPulleyJointD.lengthB;
	}
	
	public void setMaxLengthA(float maxLength) {
		mPulleyJointD.maxLengthA = maxLength;
	}
	
	public float getMaxLengthA() {
		return mPulleyJointD.maxLengthA;
	}
	
	public void setMaxLengthB(float maxLength) {
		mPulleyJointD.maxLengthB = maxLength;
	}
	
	public float getMaxLengthB() {
		return mPulleyJointD.maxLengthB;
	}
	
	public void setRatio(float ratio) {
		mPulleyJointD.ratio = ratio;
	}
	
	public float getRatio() {
		return mPulleyJoint.getRatio();
	}
}
