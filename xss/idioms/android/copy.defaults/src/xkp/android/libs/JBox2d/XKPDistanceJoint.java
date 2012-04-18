<xss:parameter id="appName"/>
<xss:parameter id="base_namespace"/>

package <xss:e value="base_namespace"/>.libs.JBox2d;

import org.jbox2d.common.Vec2;
import org.jbox2d.dynamics.World;
import org.jbox2d.dynamics.joints.DistanceJoint;
import org.jbox2d.dynamics.joints.DistanceJointDef;

public class XKPDistanceJoint {
	
	private World 				mRefWorld;
	
	private DistanceJoint		mDistJoint;
	private DistanceJointDef 	mDistJointD;
	
	public XKPDistanceJoint(World world) {
		mRefWorld = world;
		mDistJointD = new DistanceJointDef();
	}
	
	public void createJoint() {
		mDistJoint = (DistanceJoint) mRefWorld.createJoint(mDistJointD);
	}
	
	public void setBodyA(XKPPhysicBody bodyA) {
		mDistJointD.bodyA = bodyA.getBody();
	}
	
	public void setBodyB(XKPPhysicBody bodyB) {
		mDistJointD.bodyB = bodyB.getBody();
	}
	
	public void setDampingRatio(float dampingRatio) {
		mDistJointD.dampingRatio = dampingRatio;
	}
	
	public float getDampingRatio() {
		return mDistJointD.dampingRatio;
	}
	
	public void setFrecuencyHz(float frequencyHz) {
		mDistJointD.frequencyHz = frequencyHz;
	}
	
	public float getFrequencyHz() {
		return mDistJointD.frequencyHz;
	}
	
	public void setLength(float length) {
		mDistJointD.length = length;
	}
	
	public float getLength() {
		return mDistJointD.length;
	}
	
	public void setAnchorA(Vec2 anchorA) {
		mDistJointD.localAnchorA.set(anchorA);
	}
	
	public Vec2 getAnchorA() {
		return mDistJointD.localAnchorA;
	}
	
	public void setAnchorB(Vec2 anchorB) {
		mDistJointD.localAnchorB.set(anchorB);
	}
	
	public Vec2 getAnchorB() {
		return mDistJointD.localAnchorB;
	}
}
