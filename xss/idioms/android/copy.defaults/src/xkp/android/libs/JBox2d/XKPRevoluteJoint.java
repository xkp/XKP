<xss:parameter id="base_namespace"/>

package <xss:e value="base_namespace"/>.libs.JBox2d;

import org.jbox2d.common.Vec2;
import org.jbox2d.dynamics.World;
import org.jbox2d.dynamics.joints.RevoluteJoint;
import org.jbox2d.dynamics.joints.RevoluteJointDef;

import android.view.View;

public class XKPRevoluteJoint {
	
	private World 				mRefWorld;
	
	private RevoluteJoint		mRevtJoint;
	private RevoluteJointDef 	mRevtJointD;
	private boolean mIsJointCreated = false;
	
	public XKPRevoluteJoint(World world) {
		mRefWorld = world;
		mRevtJointD = new RevoluteJointDef();
	}
	
	public void createJoint() {
		mRevtJoint = (RevoluteJoint) mRefWorld.createJoint(mRevtJointD);
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
		
		mRevtJointD.bodyA = physicBodyA.getBody();
		if(mIsJointCreated)
			mRevtJoint.m_bodyA = physicBodyA.getBody();
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
		
		mRevtJointD.bodyB = physicBodyB.getBody();
		if(mIsJointCreated)
			mRevtJoint.m_bodyB = physicBodyB.getBody();
	}
	
	public void setLowerAngle(float lowerAngle) {
		mRevtJointD.lowerAngle = lowerAngle;
		if(mIsJointCreated)
			mRevtJoint.m_lowerAngle = lowerAngle;
	}
	
	public float getLowerAngle() {
		return mRevtJoint.m_lowerAngle;
	}
	
	public void setUpperAngle(float upperAngle) {
		mRevtJointD.upperAngle = upperAngle;
		if(mIsJointCreated)
			mRevtJoint.m_upperAngle = upperAngle;
	}
	
	public float getUpperAngle() {
		return mRevtJoint.m_upperAngle;
	}
	
	public void setAngle(float angle) {
		mRevtJointD.referenceAngle = angle;
		if(mIsJointCreated)
			mRevtJoint.m_referenceAngle = angle;
	}
	
	public float getAngle() {
		return mRevtJoint.m_referenceAngle;
	}
	
	public void setMaxMotorTorque(float maxMotorTorque) {
		mRevtJointD.maxMotorTorque = maxMotorTorque;
		if(mIsJointCreated)
			mRevtJoint.setMaxMotorTorque(maxMotorTorque);
	}
	
	public float getMaxMotorTorque() {
		return mRevtJoint.m_maxMotorTorque;
	}
	
	public void setMotorSpeed(float motorSpeed) {
		mRevtJointD.motorSpeed = motorSpeed;
		if(mIsJointCreated)
			mRevtJoint.setMotorSpeed(motorSpeed);
	}
	
	public float getMotorSpeed() {
		return mRevtJoint.m_motorSpeed;
	}
	
	public void setLimit(boolean limit) {
		mRevtJointD.enableLimit = limit;
		if(mIsJointCreated)
			mRevtJoint.m_enableLimit = limit;
	}
	
	public boolean getLimit() {
		return mRevtJoint.m_enableLimit;
	}
	
	public void setMotor(boolean motor) {
		mRevtJointD.enableMotor = motor;
		if(mIsJointCreated)
			mRevtJoint.m_enableMotor = motor;
	}
	
	public boolean getMotor() {
		return mRevtJoint.m_enableMotor;
	}
	
	public void setAnchorA(Vec2 anchorA) {
		mRevtJointD.localAnchorA.set(anchorA);
		if(mIsJointCreated)
			mRevtJoint.m_localAnchor1.set(anchorA);
	}
	
	public Vec2 getAnchorA() {
		return mRevtJoint.m_localAnchor1;
	}
	
	public void setAnchorB(Vec2 anchorB) {
		mRevtJointD.localAnchorB.set(anchorB);
		if(mIsJointCreated)
			mRevtJoint.m_localAnchor2.set(anchorB);
	}
	
	public Vec2 getAnchorB() {
		return mRevtJoint.m_localAnchor2;
	}
}
