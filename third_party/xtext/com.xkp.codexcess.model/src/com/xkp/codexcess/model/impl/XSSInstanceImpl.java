/**
 */
package com.xkp.codexcess.model.impl;

import com.xkp.codexcess.model.ModelPackage;
import com.xkp.codexcess.model.XSSClass;
import com.xkp.codexcess.model.XSSIProperty;
import com.xkp.codexcess.model.XSSInstance;

import java.util.Collection;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.InternalEList;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>XSS Instance</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link com.xkp.codexcess.model.impl.XSSInstanceImpl#getId <em>Id</em>}</li>
 *   <li>{@link com.xkp.codexcess.model.impl.XSSInstanceImpl#getXssclass <em>Xssclass</em>}</li>
 *   <li>{@link com.xkp.codexcess.model.impl.XSSInstanceImpl#getChilds <em>Childs</em>}</li>
 *   <li>{@link com.xkp.codexcess.model.impl.XSSInstanceImpl#getProperties <em>Properties</em>}</li>
 *   <li>{@link com.xkp.codexcess.model.impl.XSSInstanceImpl#getParent <em>Parent</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class XSSInstanceImpl extends EObjectImpl implements XSSInstance {
	/**
	 * The default value of the '{@link #getId() <em>Id</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getId()
	 * @generated
	 * @ordered
	 */
	protected static final String ID_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getId() <em>Id</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getId()
	 * @generated
	 * @ordered
	 */
	protected String id = ID_EDEFAULT;

	/**
	 * The cached value of the '{@link #getXssclass() <em>Xssclass</em>}' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getXssclass()
	 * @generated
	 * @ordered
	 */
	protected XSSClass xssclass;

	/**
	 * The cached value of the '{@link #getChilds() <em>Childs</em>}' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getChilds()
	 * @generated
	 * @ordered
	 */
	protected EList<XSSInstance> childs;

	/**
	 * The cached value of the '{@link #getProperties() <em>Properties</em>}' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getProperties()
	 * @generated
	 * @ordered
	 */
	protected EList<XSSIProperty> properties;

	/**
	 * The cached value of the '{@link #getParent() <em>Parent</em>}' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getParent()
	 * @generated
	 * @ordered
	 */
	protected XSSInstance parent;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected XSSInstanceImpl() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	protected EClass eStaticClass() {
		return ModelPackage.Literals.XSS_INSTANCE;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public String getId() {
		return id;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setId(String newId) {
		String oldId = id;
		id = newId;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, ModelPackage.XSS_INSTANCE__ID, oldId, id));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public XSSClass getXssclass() {
		if (xssclass != null && xssclass.eIsProxy()) {
			InternalEObject oldXssclass = (InternalEObject)xssclass;
			xssclass = (XSSClass)eResolveProxy(oldXssclass);
			if (xssclass != oldXssclass) {
				if (eNotificationRequired())
					eNotify(new ENotificationImpl(this, Notification.RESOLVE, ModelPackage.XSS_INSTANCE__XSSCLASS, oldXssclass, xssclass));
			}
		}
		return xssclass;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public XSSClass basicGetXssclass() {
		return xssclass;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setXssclass(XSSClass newXssclass) {
		XSSClass oldXssclass = xssclass;
		xssclass = newXssclass;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, ModelPackage.XSS_INSTANCE__XSSCLASS, oldXssclass, xssclass));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public EList<XSSInstance> getChilds() {
		if (childs == null) {
			childs = new EObjectContainmentEList<XSSInstance>(XSSInstance.class, this, ModelPackage.XSS_INSTANCE__CHILDS);
		}
		return childs;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public EList<XSSIProperty> getProperties() {
		if (properties == null) {
			properties = new EObjectContainmentEList<XSSIProperty>(XSSIProperty.class, this, ModelPackage.XSS_INSTANCE__PROPERTIES);
		}
		return properties;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public XSSInstance getParent() {
		if (parent != null && parent.eIsProxy()) {
			InternalEObject oldParent = (InternalEObject)parent;
			parent = (XSSInstance)eResolveProxy(oldParent);
			if (parent != oldParent) {
				if (eNotificationRequired())
					eNotify(new ENotificationImpl(this, Notification.RESOLVE, ModelPackage.XSS_INSTANCE__PARENT, oldParent, parent));
			}
		}
		return parent;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public XSSInstance basicGetParent() {
		return parent;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setParent(XSSInstance newParent) {
		XSSInstance oldParent = parent;
		parent = newParent;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, ModelPackage.XSS_INSTANCE__PARENT, oldParent, parent));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public NotificationChain eInverseRemove(InternalEObject otherEnd, int featureID, NotificationChain msgs) {
		switch (featureID) {
			case ModelPackage.XSS_INSTANCE__CHILDS:
				return ((InternalEList<?>)getChilds()).basicRemove(otherEnd, msgs);
			case ModelPackage.XSS_INSTANCE__PROPERTIES:
				return ((InternalEList<?>)getProperties()).basicRemove(otherEnd, msgs);
		}
		return super.eInverseRemove(otherEnd, featureID, msgs);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public Object eGet(int featureID, boolean resolve, boolean coreType) {
		switch (featureID) {
			case ModelPackage.XSS_INSTANCE__ID:
				return getId();
			case ModelPackage.XSS_INSTANCE__XSSCLASS:
				if (resolve) return getXssclass();
				return basicGetXssclass();
			case ModelPackage.XSS_INSTANCE__CHILDS:
				return getChilds();
			case ModelPackage.XSS_INSTANCE__PROPERTIES:
				return getProperties();
			case ModelPackage.XSS_INSTANCE__PARENT:
				if (resolve) return getParent();
				return basicGetParent();
		}
		return super.eGet(featureID, resolve, coreType);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@SuppressWarnings("unchecked")
	@Override
	public void eSet(int featureID, Object newValue) {
		switch (featureID) {
			case ModelPackage.XSS_INSTANCE__ID:
				setId((String)newValue);
				return;
			case ModelPackage.XSS_INSTANCE__XSSCLASS:
				setXssclass((XSSClass)newValue);
				return;
			case ModelPackage.XSS_INSTANCE__CHILDS:
				getChilds().clear();
				getChilds().addAll((Collection<? extends XSSInstance>)newValue);
				return;
			case ModelPackage.XSS_INSTANCE__PROPERTIES:
				getProperties().clear();
				getProperties().addAll((Collection<? extends XSSIProperty>)newValue);
				return;
			case ModelPackage.XSS_INSTANCE__PARENT:
				setParent((XSSInstance)newValue);
				return;
		}
		super.eSet(featureID, newValue);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public void eUnset(int featureID) {
		switch (featureID) {
			case ModelPackage.XSS_INSTANCE__ID:
				setId(ID_EDEFAULT);
				return;
			case ModelPackage.XSS_INSTANCE__XSSCLASS:
				setXssclass((XSSClass)null);
				return;
			case ModelPackage.XSS_INSTANCE__CHILDS:
				getChilds().clear();
				return;
			case ModelPackage.XSS_INSTANCE__PROPERTIES:
				getProperties().clear();
				return;
			case ModelPackage.XSS_INSTANCE__PARENT:
				setParent((XSSInstance)null);
				return;
		}
		super.eUnset(featureID);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public boolean eIsSet(int featureID) {
		switch (featureID) {
			case ModelPackage.XSS_INSTANCE__ID:
				return ID_EDEFAULT == null ? id != null : !ID_EDEFAULT.equals(id);
			case ModelPackage.XSS_INSTANCE__XSSCLASS:
				return xssclass != null;
			case ModelPackage.XSS_INSTANCE__CHILDS:
				return childs != null && !childs.isEmpty();
			case ModelPackage.XSS_INSTANCE__PROPERTIES:
				return properties != null && !properties.isEmpty();
			case ModelPackage.XSS_INSTANCE__PARENT:
				return parent != null;
		}
		return super.eIsSet(featureID);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public String toString() {
		if (eIsProxy()) return super.toString();

		StringBuffer result = new StringBuffer(super.toString());
		result.append(" (id: ");
		result.append(id);
		result.append(')');
		return result.toString();
	}

} //XSSInstanceImpl
