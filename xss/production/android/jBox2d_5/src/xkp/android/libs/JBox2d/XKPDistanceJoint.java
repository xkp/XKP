package xkp.android.libs.JBox2d;
import org.jbox2d.common.Vec2;
import org.jbox2d.dynamics.World;
import org.jbox2d.dynamics.joints.DistanceJoint;
import org.jbox2d.dynamics.joints.DistanceJointDef;
public class XKPDistanceJoint {
	private World 				mRefWorld;
	private DistanceJoint		mDistJoint;
	private DistanceJointDef 	mDistJointD;
	private boolean mIsJointCreated = false;
	public XKPDistanceJoint(World world) {
		mRefWorld = world;
		mDistJointD = new DistanceJointDef();
	}
	public void createJoint() {
		mDistJoint = (DistanceJoint) mRefWorld.createJoint(mDistJointD);
		mIsJointCreated = true;
	}
	public void setBodyA(XKPPhysicBody bodyA) {
		mDistJointD.bodyA = bodyA.getBody();
		if(mIsJointCreated)
			mDistJoint.m_bodyA = bodyA.getBody();
	}
	public void setBodyB(XKPPhysicBody bodyB) {
		mDistJointD.bodyB = bodyB.getBody();
		if(mIsJointCreated)
			mDistJoint.m_bodyB = bodyB.getBody();
	}
	public void setDampingRatio(float dampingRatio) {
		mDistJointD.dampingRatio = dampingRatio;
		if(mIsJointCreated)
			mDistJoint.setDampingRatio(dampingRatio);
	}
	public float getDampingRatio() {
		return mDistJoint.getDampingRatio();
	}
	public void setFrecuencyHz(float frequencyHz) {
		mDistJointD.frequencyHz = frequencyHz;
		if(mIsJointCreated)
			mDistJoint.setFrequency(frequencyHz);
	}
	public float getFrequencyHz() {
		return mDistJoint.getFrequency();
	}
	public void setLength(float length) {
		mDistJointD.length = length;
		if(mIsJointCreated)
			mDistJoint.setLength(length);
	}
	public float getLength() {
		return mDistJoint.getLength();
	}
	public void setAnchorA(Vec2 anchorA) {
		mDistJointD.localAnchorA.set(anchorA);
		if(mIsJointCreated)
			mDistJoint.m_localAnchor1.set(anchorA);
	}
	public Vec2 getAnchorA() {
		return mDistJoint.m_localAnchor1;
	}
	public void setAnchorB(Vec2 anchorB) {
		mDistJointD.localAnchorB.set(anchorB);
		if(mIsJointCreated)
			mDistJoint.m_localAnchor2.set(anchorB);
	}
	public Vec2 getAnchorB() {
		return mDistJoint.m_localAnchor2;
	}
}
