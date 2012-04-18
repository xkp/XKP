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
	
	public XKPPrismaticJoint(World world) {
		mRefWorld = world;
		mPrismJointD = new PrismaticJointDef();
	}
	
	public void createJoint() {
		mPrismJoint = (PrismaticJoint) mRefWorld.createJoint(mPrismJointD);
	}
	
	public void setBodyA(XKPPhysicBody bodyA) {
		mPrismJointD.bodyA = bodyA.getBody();
	}
	
	public void setBodyB(XKPPhysicBody bodyB) {
		mPrismJointD.bodyB = bodyB.getBody();
	}
	
	public void setAxis(Vec2 axis) {
		mPrismJointD.localAxis1.set(axis);
	}
	
	public Vec2 getAxis() {
		return mPrismJointD.localAxis1;
	}
	
	public void setLowerTranslation(float lowerTranslation) {
		mPrismJointD.lowerTranslation = lowerTranslation;
	}
	
	public float getLowerTranslation() {
		return mPrismJointD.lowerTranslation;
	}
	
	public void setUpperTranslation(float upperTranslation) {
		mPrismJointD.upperTranslation = upperTranslation;
	}
	
	public float getUpperTranslation() {
		return mPrismJointD.upperTranslation;
	}
	
	public void setMaxMotorForce(float maxMotorForce) {
		mPrismJointD.maxMotorForce = maxMotorForce;
	}
	
	public float getMaxMotorForce() {
		return mPrismJointD.maxMotorForce;
	}
	
	public void setMotorSpeed(float motorSpeed) {
		mPrismJointD.motorSpeed = motorSpeed;
	}
	
	public float getMotorSpeed() {
		return mPrismJointD.motorSpeed;
	}
	
	public void setLimit(boolean limit) {
		mPrismJointD.enableLimit = limit;
	}
	
	public boolean getLimit() {
		return mPrismJointD.enableLimit;
	}
	
	public void setMotor(boolean motor) {
		mPrismJointD.enableMotor = motor;
	}
	
	public boolean getMotor() {
		return mPrismJointD.enableMotor;
	}
	
	public void setAnchorA(Vec2 anchorA) {
		mPrismJointD.localAnchorA.set(anchorA);
	}
	
	public Vec2 getAnchorA() {
		return mPrismJointD.localAnchorA;
	}
	
	public void setAnchorB(Vec2 anchorB) {
		mPrismJointD.localAnchorB.set(anchorB);
	}
	
	public Vec2 getAnchorB() {
		return mPrismJointD.localAnchorB;
	}
}
