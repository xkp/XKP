<xss:parameter id="appName"/>
<xss:parameter id="base_namespace"/>

package <xss:e value="base_namespace"/>.libs.JBox2d;

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

import <xss:e value="base_namespace"/>.libs.Layout.XKPLayout;
import android.view.View;

public class XKPPhysicBody {
	
	private World 		mRefWorld;
	private XKPGraphics	mRefView;
	
	private BodyDef 	mBodyDef;
	private FixtureDef mFixtureDef;

	private Shape		mShape;
	private Body 		mBody;
	
	private AABB		mAABB;
	private float		mScale = 1.0f;
	
	protected OnCollisionListener mOnCollisionListener;

	public XKPPhysicBody(World world, ShapeType shapeType, XKPGraphics view) {
		mRefWorld = world;
		mRefView = view;
		
		mBodyDef = new BodyDef();
		mFixtureDef = new FixtureDef();
		mAABB = new AABB();
		
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
	
	public boolean updateRefPosition() {
		if (mRefView == null && !(mRefView instanceof XKPGraphics))
			return false;
		
		AABB aabb = new AABB();
		Transform transf = mBody.getTransform();
		mShape.computeAABB(aabb, transf);
		
		if (mShape.m_type == ShapeType.POLYGON) {
			mRefView.setLeft((int) (mBody.getPosition().x - mRefView.getDX() / 2));
			mRefView.setTop((int) (mBody.getPosition().y - mRefView.getDY() / 2));
		} else if (mShape.m_type == ShapeType.CIRCLE) {
			mRefView.setLeft((int) (aabb.getCenter().x));
			mRefView.setTop((int) (aabb.getCenter().y));
		}
		mRefView.setRotation(rad2degree(transf.getAngle()));
		
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
	}
	
	public float getDensity() {
		return mFixtureDef.density;
	}
	
	public void setFriction(float friction) {
		mFixtureDef.friction = friction;
	}
	
	public float getFriction() {
		return mFixtureDef.friction;
	}
	
	public void setRestitution(float restitution) {
		mFixtureDef.restitution = restitution;
	}
	
	public float getRestitution() {
		return mFixtureDef.restitution;
	}
	
	public void setBodyType(BodyType bodyType) {
		mBodyDef.type = bodyType;
	}
	
	public BodyType getBodyType() {
		return mBodyDef.type;
	}
	
	public void setPosition(Vec2 pos) {
		if (pos == null)
			pos = mBodyDef.position;

		Transform transf = new Transform();
		transf.set(pos, mBodyDef.angle);
		mShape.computeAABB(mAABB, transf);
        
		mBodyDef.position = new Vec2((pos.x + mAABB.getExtents().x) / mScale,
				(pos.y + mAABB.getExtents().y) / mScale);
	}
	
	public void setPosition(float x, float y) {
		setPosition(new Vec2(x, y));
	}
	
	public Vec2 getPosition() {
		return mBodyDef.position;
	}
	
	public void setAngle(float angle) {
		mBodyDef.angle = (float) degree2rad(angle);
	}
	
	public float getAngle() {
		return mBodyDef.angle;
	}
	
	public void setAllowSleep(boolean allow) {
		mBodyDef.allowSleep = allow;
	}
	
	public boolean getAllowSleep() {
		return mBodyDef.allowSleep;
	}
	
	public void setAwake(boolean awake) {
		mBodyDef.awake = awake;
	}
	
	public boolean getAwake() {
		return mBodyDef.awake;
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
        
        mBody.createFixture(mFixtureDef);
        mBody.setUserData(this);
	}
	
	public void createBody(float width, float height) {
		createBody(width, height, 0, null);
	}
	
	public void createBody(float radius) {
		createBody(radius, 0, 0, null);
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
