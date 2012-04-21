<xss:parameter id="appName"/>
<xss:parameter id="base_namespace"/>

package <xss:e value="base_namespace"/>.libs.JBox2d;

import org.jbox2d.common.Vec2;
import org.jbox2d.dynamics.World;
import org.jbox2d.dynamics.joints.PulleyJoint;
import org.jbox2d.dynamics.joints.PulleyJointDef;

public class XKPPulleyJoint {
	
	private World 				mRefWorld;
	
	private PulleyJoint			mPulleyJoint;
	private PulleyJointDef 		mPulleyJointD;
	
	public XKPPulleyJoint(World world) {
		mRefWorld = world;
		mPulleyJointD = new PulleyJointDef();
	}
	
	public void createJoint() {
		mPulleyJoint = (PulleyJoint) mRefWorld.createJoint(mPulleyJointD);
	}
	
	public void setBodyA(XKPPhysicBody bodyA) {
		mPulleyJointD.bodyA = bodyA.getBody();
	}
	
	public void setBodyB(XKPPhysicBody bodyB) {
		mPulleyJointD.bodyB = bodyB.getBody();
	}

	public void setGroundAnchorA(Vec2 anchor) {
		mPulleyJointD.groundAnchorA.set(anchor);
	}
	
	public Vec2 getGroundAnchorA() {
		return mPulleyJointD.groundAnchorA;
	}
	
	public void setGroundAnchorB(Vec2 anchor) {
		mPulleyJointD.groundAnchorB.set(anchor);
	}
	
	public Vec2 getGroundAnchorB() {
		return mPulleyJointD.groundAnchorB;
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
		return mPulleyJointD.ratio;
	}
}
