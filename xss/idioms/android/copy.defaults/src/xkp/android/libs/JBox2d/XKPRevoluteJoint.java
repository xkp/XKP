<xss:parameter id="appName"/>
<xss:parameter id="base_namespace"/>

package <xss:e value="base_namespace"/>.libs.JBox2d;

import org.jbox2d.common.Vec2;
import org.jbox2d.dynamics.World;
import org.jbox2d.dynamics.joints.RevoluteJoint;
import org.jbox2d.dynamics.joints.RevoluteJointDef;

public class XKPRevoluteJoint {
	
	private World 				mRefWorld;
	
	private RevoluteJoint		mRevtJoint;
	private RevoluteJointDef 	mRevtJointD;
	
	public XKPRevoluteJoint(World world) {
		mRefWorld = world;
		mRevtJointD = new RevoluteJointDef();
	}
	
	public void createJoint() {
		mRevtJoint = (RevoluteJoint) mRefWorld.createJoint(mRevtJointD);
	}
	
	public void setBodyA(XKPPhysicBody bodyA) {
		mRevtJointD.bodyA = bodyA.getBody();
	}
	
	public void setBodyB(XKPPhysicBody bodyB) {
		mRevtJointD.bodyB = bodyB.getBody();
	}
	
	public void setLowerAngle(float lowerAngle) {
		mRevtJointD.lowerAngle = lowerAngle;
	}
	
	public float getLowerAngle() {
		return mRevtJointD.lowerAngle;
	}
	
	public void setUpperAngle(float upperAngle) {
		mRevtJointD.upperAngle = upperAngle;
	}
	
	public float getUpperAngle() {
		return mRevtJointD.upperAngle;
	}
	
	public void setMaxMotorTorque(float maxMotorTorque) {
		mRevtJointD.maxMotorTorque = maxMotorTorque;
	}
	
	public float getMaxMotorTorque() {
		return mRevtJointD.maxMotorTorque;
	}
	
	public void setMotorSpeed(float motorSpeed) {
		mRevtJointD.motorSpeed = motorSpeed;
	}
	
	public float getMotorSpeed() {
		return mRevtJointD.motorSpeed;
	}
	
	public void setLimit(boolean limit) {
		mRevtJointD.enableLimit = limit;
	}
	
	public boolean getLimit() {
		return mRevtJointD.enableLimit;
	}
	
	public void setMotor(boolean motor) {
		mRevtJointD.enableMotor = motor;
	}
	
	public boolean getMotor() {
		return mRevtJointD.enableMotor;
	}
	
	public void setAnchorA(Vec2 anchorA) {
		mRevtJointD.localAnchorA.set(anchorA);
	}
	
	public Vec2 getAnchorA() {
		return mRevtJointD.localAnchorA;
	}
	
	public void setAnchorB(Vec2 anchorB) {
		mRevtJointD.localAnchorB.set(anchorB);
	}
	
	public Vec2 getAnchorB() {
		return mRevtJointD.localAnchorB;
	}
}
