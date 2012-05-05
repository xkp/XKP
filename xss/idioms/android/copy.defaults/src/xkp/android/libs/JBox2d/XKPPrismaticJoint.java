<xss:parameter id="appName"/>
<xss:parameter id="base_namespace"/>

package <xss:e value="base_namespace"/>.libs.JBox2d;

import org.jbox2d.common.Vec2;
import org.jbox2d.dynamics.World;
import org.jbox2d.dynamics.joints.PrismaticJoint;
import org.jbox2d.dynamics.joints.PrismaticJointDef;

public class XKPPrismaticJoint {
	
	private World 				mRefWorld;
	
	private PrismaticJoint		mPrismJoint;
	private PrismaticJointDef 	mPrismJointD;
	private boolean mIsJointCreated = false;
	
	public XKPPrismaticJoint(World world) {
		mRefWorld = world;
		mPrismJointD = new PrismaticJointDef();
	}
	
	public void createJoint() {
		mPrismJoint = (PrismaticJoint) mRefWorld.createJoint(mPrismJointD);
		mIsJointCreated = true;
	}
	
	public void setBodyA(XKPPhysicBody bodyA) {
		mPrismJointD.bodyA = bodyA.getBody();
		if(mIsJointCreated)
			mPrismJoint.m_bodyA = bodyA.getBody();
	}
	
	public void setBodyB(XKPPhysicBody bodyB) {
		mPrismJointD.bodyB = bodyB.getBody();
		if(mIsJointCreated)
			mPrismJoint.m_bodyB = bodyB.getBody();
	}
	
	public void setAxis(Vec2 axis) {
		mPrismJointD.localAxis1.set(axis);
		if(mIsJointCreated)
			mPrismJoint.m_axis.set(axis);
	}
	
	public Vec2 getAxis() {
		return mPrismJoint.m_axis;
	}
	
	public void setLowerTranslation(float lowerTranslation) {
		mPrismJointD.lowerTranslation = lowerTranslation;
		if(mIsJointCreated)
			mPrismJoint.m_lowerTranslation = lowerTranslation;
	}
	
	public float getLowerTranslation() {
		return mPrismJoint.m_lowerTranslation;
	}
	
	public void setUpperTranslation(float upperTranslation) {
		mPrismJointD.upperTranslation = upperTranslation;
		if(mIsJointCreated)
			mPrismJoint.m_upperTranslation = upperTranslation;
	}
	
	public float getUpperTranslation() {
		return mPrismJoint.m_upperTranslation;
	}
	
	public void setMaxMotorForce(float maxMotorForce) {
		mPrismJointD.maxMotorForce = maxMotorForce;
		if(mIsJointCreated)
			mPrismJoint.setMaxMotorForce(maxMotorForce);
	}
	
	public float getMaxMotorForce() {
		return mPrismJoint.m_maxMotorForce;
	}
	
	public void setMotorSpeed(float motorSpeed) {
		mPrismJointD.motorSpeed = motorSpeed;
		if(mIsJointCreated)
			mPrismJoint.setMotorSpeed(motorSpeed);
	}
	
	public float getMotorSpeed() {
		return mPrismJoint.m_motorSpeed;
	}
	
	public void setLimit(boolean limit) {
		mPrismJointD.enableLimit = limit;
		if(mIsJointCreated)
			mPrismJoint.m_enableLimit = limit;
	}
	
	public boolean getLimit() {
		return mPrismJoint.m_enableLimit;
	}
	
	public void setMotor(boolean motor) {
		mPrismJointD.enableMotor = motor;
		if(mIsJointCreated)
			mPrismJoint.m_enableMotor = motor;
	}
	
	public boolean getMotor() {
		return mPrismJoint.m_enableMotor;
	}
	
	public void setAnchorA(Vec2 anchorA) {
		mPrismJointD.localAnchorA.set(anchorA);
		if(mIsJointCreated)
			mPrismJoint.m_localAnchor1.set(anchorA);
	}
	
	public Vec2 getAnchorA() {
		return mPrismJoint.m_localAnchor1;
	}
	
	public void setAnchorB(Vec2 anchorB) {
		mPrismJointD.localAnchorB.set(anchorB);
		if(mIsJointCreated)
			mPrismJoint.m_localAnchor2.set(anchorB);
	}
	
	public Vec2 getAnchorB() {
		return mPrismJoint.m_localAnchor2;
	}
}
