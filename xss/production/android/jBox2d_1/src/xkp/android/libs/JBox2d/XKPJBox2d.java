package xkp.android.libs.JBox2d;
import org.jbox2d.callbacks.DebugDraw;
import org.jbox2d.collision.Manifold;
import org.jbox2d.common.Vec2;
import org.jbox2d.dynamics.Body;
import org.jbox2d.dynamics.World;
import org.jbox2d.dynamics.contacts.Contact;
public class XKPJBox2d {
	private final int TARGET_FPS = 60;
	private final float TIME_STEP = (float) (1.0f / TARGET_FPS);
	private final int VELOCITY_ITERATIONS = 8;
	private final int POSITION_ITERATIONS = 3;
	private XKPDebugDraw 	mXKPDebugDraw;
	private World 			mWorld;
	private PhysicContactListener mCollisionListener = new PhysicContactListener();
	public XKPJBox2d() {
	}
	public void createWorld(Vec2 gravityVector, XKPDebugDraw ddraw) {
		Vec2 gravity = new Vec2(0.0f, 10.0f);
		if (gravityVector != null)
			gravity.set(gravityVector);
		boolean doSleep = true;
		mWorld = new World(gravity, doSleep);
		mXKPDebugDraw = ddraw;
		if (mXKPDebugDraw != null) {
			mWorld.setDebugDraw((DebugDraw) mXKPDebugDraw);
			mXKPDebugDraw.appendFlags(DebugDraw.e_shapeBit
					| DebugDraw.e_jointBit);
		}
		mWorld.setContactListener(mCollisionListener);
	}
	public void createWorld(Vec2 gravityVector) {
		this.createWorld(gravityVector, null);
	}
	public void createWorld() {
		this.createWorld(null, null);
	}
	public World getWorld() {
		return mWorld;
	}
	public void update() {
		mWorld.step(TIME_STEP, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
		if (mXKPDebugDraw != null) {
			mWorld.drawDebugData();
			mXKPDebugDraw.getDDView().render();
		}
		Body body = mWorld.getBodyList();
		while (body != null) {
			if (body.isActive() && body.getUserData() != null) {
				XKPPhysicBody physic = (XKPPhysicBody) body.getUserData();
				boolean result = physic.updateRefPosition();
			}
			body = body.getNext();
		}
		mWorld.clearForces();
	}
	private class PhysicContactListener implements ContactListener {
		@Override
		public void beginContact(Contact contact) {
			if(contact.isTouching()) {
				XKPPhysicBody pBodyA = (XKPPhysicBody) contact.getFixtureA().getBody().getUserData();
				XKPPhysicBody pBodyB = (XKPPhysicBody) contact.getFixtureB().getBody().getUserData();
				if(pBodyA != null) pBodyA.performBodyCollision(pBodyB);
				if(pBodyB != null) pBodyB.performBodyCollision(pBodyA);
			}
		}
		@Override
		public void endContact(Contact arg0) {
		}
		@Override
		public void postSolve(Contact arg0, ContactImpulse arg1) {
		}
		@Override
		public void preSolve(Contact arg0, Manifold arg1) {
		}	
	}
}
