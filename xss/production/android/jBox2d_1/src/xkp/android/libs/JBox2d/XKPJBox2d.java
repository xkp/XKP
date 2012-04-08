package xkp.android.libs.JBox2d;
import org.jbox2d.callbacks.DebugDraw;
import org.jbox2d.common.Vec2;
import org.jbox2d.dynamics.Body;
import org.jbox2d.dynamics.World;
public class XKPJBox2d {
    private final int 	TARGET_FPS = 40;
    private final float TIME_STEP = (float) (1.0f / TARGET_FPS);
    private final int 	VELOCITY_ITERATIONS = 6;
    private final int 	POSITION_ITERATIONS = 2;
    private XKPDebugDraw 	mXKPDebugDraw;
	private World 			mWorld;
	public XKPJBox2d() {
	}
	public void createWorld(Vec2 gravityVector, XKPDebugDraw ddraw) {
        Vec2 gravity = new Vec2(0.0f, 10.0f);
        if(gravityVector != null) gravity.set(gravityVector);
        boolean doSleep = true;
        mWorld = new World(gravity, doSleep);
        mXKPDebugDraw = ddraw;
        if(mXKPDebugDraw != null) {
        	mWorld.setDebugDraw((DebugDraw)mXKPDebugDraw);
        	mXKPDebugDraw.appendFlags(DebugDraw.e_shapeBit | DebugDraw.e_jointBit);
        }
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
		if(mXKPDebugDraw != null) {
			mWorld.drawDebugData();
			mXKPDebugDraw.getDDView().render();
		}
		Body body = mWorld.getBodyList();
		while(body != null) {
			if(body.isActive()) {
				XKPPhysicBody physic = (XKPPhysicBody) body.getUserData();
				physic.updateRefPosition();
			}
			body = body.getNext();
		}
		mWorld.clearForces();
		/*
		if(mWorld.getBodyCount() > 0) {
			Body body = mWorld.getBodyList();
			Vec2 position = body.getPosition();
			float angle = body.getAngle();
			Log.d(this.getClass().getSimpleName(), "Pos: (" + position.x + ", " + position.y + "), Angle: " + angle);
		}
		*/
	}
}
