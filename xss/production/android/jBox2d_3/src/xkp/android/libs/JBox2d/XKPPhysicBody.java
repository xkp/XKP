package xkp.android.libs.JBox2d;
import org.jbox2d.collision.AABB;
import org.jbox2d.collision.shapes.CircleShape;
import org.jbox2d.collision.shapes.PolygonShape;
import org.jbox2d.collision.shapes.Shape;
import org.jbox2d.collision.shapes.ShapeType;
import org.jbox2d.common.Transform;
import org.jbox2d.common.Vec2;
import org.jbox2d.dynamics.Body;
import org.jbox2d.dynamics.BodyDef;
import org.jbox2d.dynamics.BodyType;
import org.jbox2d.dynamics.FixtureDef;
import org.jbox2d.dynamics.World;
import xkp.android.libs.Graphics.XKPGraphics;
/**
 * A physic body definition holds all the data needed to construct a generic body.
 * All necessary to define shape, body and other properties are included here.
 * A link to host handle XKPGraphic object is added too.
 * 
 * @author Gilberto Cuba
 */
public class XKPPhysicBody {
	private World 		mRefWorld;
	private XKPGraphics	mRefView;
	private boolean mIsBodyCreated = false;
	private boolean mMouseJoint = false;
	private BodyDef 	mBodyDef;
	private FixtureDef mFixtureDef;
	private Shape		mShape;
	private Body 		mBody;
	private AABB		mAABB;
	private float		mScale = 1.0f;
	private Vec2 hostPosition;
	private float hostAngle = 0.0f;
	protected OnCollisionListener mOnCollisionListener;
	public XKPPhysicBody(World world, ShapeType shapeType, XKPGraphics view) {
		mRefView = view;
		if (view != null) {
			view.setTag(this);
		}
		mRefWorld = world;
		mBodyDef = new BodyDef();
		mFixtureDef = new FixtureDef();
		mAABB = new AABB();
		hostPosition = new Vec2();
		if (shapeType == ShapeType.CIRCLE) {
			mShape = new CircleShape();
		} else if (shapeType == ShapeType.POLYGON) {
			mShape = new PolygonShape();
		}
	}
	public XKPPhysicBody(World world, ShapeType shapeType) {
		this(world, shapeType, null);
	}
	public Body getBody() {
		return mBody;
	}
	public boolean updateHostPosition() {
		if (mRefView == null && !(mRefView instanceof XKPGraphics))
			return false;
		Transform transf = mBody.getTransform();
		float radianAngle = transf.getAngle();
		float delthaAngle = radianAngle - hostAngle;
		Vec2 delthaPosition = new Vec2();
		delthaPosition.set(mBody.getPosition());
		delthaPosition.subLocal(hostPosition);
		// update host body with new deltha rotation and position
		mRefView.setPosition(
				mRefView.getX1() + delthaPosition.x, 
				mRefView.getY1() + delthaPosition.y);
		mRefView.setAngle(
				mRefView.getAngle() + (float)Math.toDegrees(delthaAngle));
		// and then reassign host rotation and position to my physic body
		setPosition(mRefView.getX1(), mRefView.getY1());
		setAngle(mRefView.getAngle());
		return true;
	}
	public static double degree2rad(float degreeAngle) {
		return degreeAngle * (Math.PI / 180);
	}
	public static double rad2degree(float radAngle) {
		return radAngle * (180 / Math.PI);
	}
	public void setDensity(float density) {
		mFixtureDef.density = density;
		if (mIsBodyCreated)
			mBody.m_fixtureList.setDensity(density);
	}
	public float getDensity() {
		return mFixtureDef.density;
	}
	public void setFriction(float friction) {
		mFixtureDef.friction = friction;
		if (mIsBodyCreated)
			mBody.m_fixtureList.setFriction(friction);
	}
	public float getFriction() {
		return mFixtureDef.friction;
	}
	public void setRestitution(float restitution) {
		mFixtureDef.restitution = restitution;
		if (mIsBodyCreated)
			mBody.m_fixtureList.setRestitution(restitution);
	}
	public float getRestitution() {
		return mFixtureDef.restitution;
	}
	public void setSensor(boolean sensor) {
		mFixtureDef.isSensor = sensor;
		if (mIsBodyCreated)
			mBody.m_fixtureList.setSensor(sensor);
	}
	public boolean isSensor() {
		return mFixtureDef.isSensor;
	}
	public void setLinearDamping(float linearDamping) {
		mBodyDef.linearDamping = linearDamping;
		if (mIsBodyCreated)
			mBody.setLinearDamping(linearDamping);
	}
	public float getLinearDamping() {
		return mBody.getLinearDamping();
	}
	public void setLinearVelocity(Vec2 linearVelocity) {
		mBodyDef.linearVelocity = linearVelocity;
		if (mIsBodyCreated)
			mBody.setLinearVelocity(linearVelocity);
	}
	public Vec2 getLinearVelocity() {
		return mBody.getLinearVelocity();
	}
	public void setAngularDamping(float angularDamping) {
		mBodyDef.angularDamping = angularDamping;
		if (mIsBodyCreated)
			mBody.setAngularDamping(angularDamping);
	}
	public float getAngularDamping() {
		return mBody.getAngularDamping();
	}
	public void setBodyType(BodyType bodyType) {
		mBodyDef.type = bodyType;
		if (mIsBodyCreated)
			mBody.setType(bodyType);
	}
	public BodyType getBodyType() {
		return mBody.m_type;
	}
	public void setPosition(Vec2 pos) {
		if (pos == null)
			pos = mBodyDef.position;
		Transform transf = new Transform();
		transf.set(pos, 0.0f);
		mShape.computeAABB(mAABB, transf);
		mBodyDef.position = new Vec2(
				(pos.x + mAABB.getExtents().x) / mScale,
				(pos.y + mAABB.getExtents().y) / mScale);
		hostPosition.set(mBodyDef.position);
		if (mIsBodyCreated)
			mBody.setTransform(mBodyDef.position, getRadiansAngle());
	}
	public void setPosition(float x, float y) {
		setPosition(new Vec2(x, y));
	}
	public Vec2 getPosition() {
		return mBody.getPosition();
	}
	/**
	 * Set the body rotation.
	 * 
	 * @param angle
	 *            the world rotation in degrees.
	 */
	public void setAngle(float angle) {
		hostAngle = (float) Math.toRadians(angle);
		mBodyDef.angle = hostAngle;
		if (mIsBodyCreated)
			mBody.setTransform(getPosition(), hostAngle);
	}
	private float getRadiansAngle() {
		float resultAngle = mBodyDef.angle;
		if (mIsBodyCreated)
			resultAngle = mBody.getAngle();
		return resultAngle;
	}
	/**
	 * Get the body rotation.
	 * 
	 * @return the world rotation of the body in degrees.
	 */
	public float getAngle() {
		return (float) Math.toDegrees(getRadiansAngle());
	}
	public void setAllowSleep(boolean allow) {
		mBodyDef.allowSleep = allow;
		if (mIsBodyCreated)
			mBody.setSleepingAllowed(allow);
	}
	public boolean getAllowSleep() {
		return mBody.isSleepingAllowed();
	}
	public void setAwake(boolean awake) {
		mBodyDef.awake = awake;
		if (mIsBodyCreated)
			mBody.setAwake(awake);
	}
	public boolean getAwake() {
		return mBody.isAwake();
	}
	public void createBody(float radius_width, float height, int verticesCount,
			Vec2[] vertices) {
		if (mShape.getType() == ShapeType.CIRCLE) {
			mShape.m_radius = radius_width / mScale;
		} else if (mShape.getType() == ShapeType.POLYGON) {
			if (verticesCount == 0 && vertices == null) {
				((PolygonShape) mShape).setAsBox(radius_width / 2 * mScale,
						height / 2 * mScale);
				// TODO: shape.setAsEdge(...);
	        } else {
	        	((PolygonShape) mShape).set(vertices, verticesCount);
	        }
		}
		mFixtureDef.shape = mShape;
		setPosition(null);
		mBody = mRefWorld.createBody(mBodyDef);
		mIsBodyCreated = true;
		mBody.createFixture(mFixtureDef);
		mBody.setUserData(this);
	}
	public void createBody(float width, float height) {
		createBody(width, height, 0, null);
	}
	public void createBody(float radius) {
		createBody(radius, 0, 0, null);
	}
	public void setMouseJoint(boolean mouseJoint) {
		mMouseJoint = mouseJoint;
	}
	public boolean getMouseJoint() {
		return mMouseJoint;
	}
	public void setOnCollisionListener(OnCollisionListener l) {
		mOnCollisionListener = l;
	}
	public boolean performBodyCollision(XKPPhysicBody physicBody) {
		if (mOnCollisionListener != null) {
			mOnCollisionListener.onCollision(physicBody);
			return true;
		}
		return false;
	}
	public interface OnCollisionListener {
		void onCollision(XKPPhysicBody physicBody);
	}
}
