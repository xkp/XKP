<xss:parameter id="appName"/>
<xss:parameter id="base_namespace"/>

package <xss:e value="base_namespace"/>.libs.JBox2d;

import org.jbox2d.collision.AABB;
import org.jbox2d.collision.shapes.CircleShape;
import org.jbox2d.collision.shapes.PolygonShape;
import org.jbox2d.collision.shapes.Shape;
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
	
	private World 	mRefWorld;
	private View 	mRefView;
	
	private Body 	mBody;
	private BodyDef mBodyDef;
	private Shape	mShape;
	
	private float	mScale = 1.0f;
	
	public XKPPhysicBody(World world, View view) {
		mRefWorld = world;
		mRefView = view;
	}
	
	public XKPPhysicBody(World world) {
		this(world, null);
	}
	
	public void updateRefPosition() {
		if(mRefView == null) return;
		
		AABB aabb = new AABB();
		Transform transf = mBody.getTransform();
		mShape.computeAABB(aabb, transf);
		
		//((XKPLayout.LayoutParams) mRefView.getLayoutParams()).setLeft((int)mBody.getPosition().x);
		//((XKPLayout.LayoutParams) mRefView.getLayoutParams()).setTop((int)mBody.getPosition().y);
		
		((XKPLayout.LayoutParams) mRefView.getLayoutParams()).setLeft((int)(aabb.getCenter().x - aabb.getExtents().x));
		((XKPLayout.LayoutParams) mRefView.getLayoutParams()).setTop((int)(aabb.getCenter().y - aabb.getExtents().y));
		
		//TODO: update angle rotation that is work on API 11
	}

	public static float sexa2rad(float angle) {
		return angle / 360 * 2 * (float) Math.PI;
	}
	
	private BodyDef createBodyDef(BodyType bodyType, Vec2 position, Vec2 size, float rotationAngle) {
		mBodyDef = new BodyDef();
		mBodyDef.type = bodyType;
		mBodyDef.position = new Vec2((position.x + size.x/2) / mScale, (position.y + size.y/2) / mScale);
		mBodyDef.angle = sexa2rad(rotationAngle);

		return mBodyDef;
	}
	
	private FixtureDef createFixtureDef(Shape shape, float density, float friction, float restitution) {
		FixtureDef fixtureDef = new FixtureDef();
        fixtureDef.shape = shape;
        fixtureDef.density = density;
        fixtureDef.friction = friction;
        fixtureDef.restitution = restitution;
        
		return fixtureDef;
	}
	
	public void createCircleBody(float x, float y, float radius, float angle, BodyType bodytype, 
			float density, float friction, float restitution) {
		
        mBody = mRefWorld.createBody(createBodyDef(bodytype, new Vec2(x, y), new Vec2(radius * 2, radius * 2), angle)); 
        
        CircleShape circle = new CircleShape();
        circle.m_radius = radius / mScale;
        mShape = circle;
        
        mBody.createFixture(createFixtureDef(circle, density, friction, restitution));
        mBody.setUserData(this);
	}
	
	public void createBoxBody(float x, float y, float width, float height, float angle, BodyType bodytype, 
			float density, float friction, float restitution) {
		
        mBody = mRefWorld.createBody(createBodyDef(bodytype, new Vec2(x, y), new Vec2(width, height), angle)); 
        
        PolygonShape box = new PolygonShape();
        box.setAsBox(width / 2 * mScale, height / 2 * mScale);
        mShape = box;
        
        mBody.createFixture(createFixtureDef(box, density, friction, restitution));
        mBody.setUserData(this);
	}
}
