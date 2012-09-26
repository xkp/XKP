/**
 */
package com.xkp.codexcess.model.impl;

import com.xkp.codexcess.model.ModelPackage;
import com.xkp.codexcess.model.XSSClass;
import com.xkp.codexcess.model.XSSEvent;
import com.xkp.codexcess.model.XSSImport;
import com.xkp.codexcess.model.XSSMethod;
import com.xkp.codexcess.model.XSSProperty;

import java.util.Collection;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.EObjectWithInverseResolvingEList;
import org.eclipse.emf.ecore.util.InternalEList;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>XSS Class</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link com.xkp.codexcess.model.impl.XSSClassImpl#getId <em>Id</em>}</li>
 *   <li>{@link com.xkp.codexcess.model.impl.XSSClassImpl#getSuper <em>Super</em>}</li>
 *   <li>{@link com.xkp.codexcess.model.impl.XSSClassImpl#getXssimports <em>Xssimports</em>}</li>
 *   <li>{@link com.xkp.codexcess.model.impl.XSSClassImpl#getXssmethods <em>Xssmethods</em>}</li>
 *   <li>{@link com.xkp.codexcess.model.impl.XSSClassImpl#getXssevents <em>Xssevents</em>}</li>
 *   <li>{@link com.xkp.codexcess.model.impl.XSSClassImpl#getXssproperties <em>Xssproperties</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class XSSClassImpl extends EObjectImpl implements XSSClass {
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
	 * The cached value of the '{@link #getSuper() <em>Super</em>}' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getSuper()
	 * @generated
	 * @ordered
	 */
	protected XSSClass super_;

	/**
	 * The cached value of the '{@link #getXssimports() <em>Xssimports</em>}' reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getXssimports()
	 * @generated
	 * @ordered
	 */
	protected EList<XSSImport> xssimports;

	/**
	 * The cached value of the '{@link #getXssmethods() <em>Xssmethods</em>}' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getXssmethods()
	 * @generated
	 * @ordered
	 */
	protected EList<XSSMethod> xssmethods;

	/**
	 * The cached value of the '{@link #getXssevents() <em>Xssevents</em>}' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getXssevents()
	 * @generated
	 * @ordered
	 */
	protected EList<XSSEvent> xssevents;

	/**
	 * The cached value of the '{@link #getXssproperties() <em>Xssproperties</em>}' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getXssproperties()
	 * @generated
	 * @ordered
	 */
	protected EList<XSSProperty> xssproperties;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected XSSClassImpl() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	protected EClass eStaticClass() {
		return ModelPackage.Literals.XSS_CLASS;
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
			eNotify(new ENotificationImpl(this, Notification.SET, ModelPackage.XSS_CLASS__ID, oldId, id));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public XSSClass getSuper() {
		if (super_ != null && super_.eIsProxy()) {
			InternalEObject oldSuper = (InternalEObject)super_;
			super_ = (XSSClass)eResolveProxy(oldSuper);
			if (super_ != oldSuper) {
				if (eNotificationRequired())
					eNotify(new ENotificationImpl(this, Notification.RESOLVE, ModelPackage.XSS_CLASS__SUPER, oldSuper, super_));
			}
		}
		return super_;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public XSSClass basicGetSuper() {
		return super_;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setSuper(XSSClass newSuper) {
		XSSClass oldSuper = super_;
		super_ = newSuper;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, ModelPackage.XSS_CLASS__SUPER, oldSuper, super_));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public EList<XSSImport> getXssimports() {
		if (xssimports == null) {
			xssimports = new EObjectWithInverseResolvingEList<XSSImport>(XSSImport.class, this, ModelPackage.XSS_CLASS__XSSIMPORTS, ModelPackage.XSS_IMPORT__XSSCLASS);
		}
		return xssimports;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public EList<XSSMethod> getXssmethods() {
		if (xssmethods == null) {
			xssmethods = new EObjectContainmentEList<XSSMethod>(XSSMethod.class, this, ModelPackage.XSS_CLASS__XSSMETHODS);
		}
		return xssmethods;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public EList<XSSEvent> getXssevents() {
		if (xssevents == null) {
			xssevents = new EObjectContainmentEList<XSSEvent>(XSSEvent.class, this, ModelPackage.XSS_CLASS__XSSEVENTS);
		}
		return xssevents;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public EList<XSSProperty> getXssproperties() {
		if (xssproperties == null) {
			xssproperties = new EObjectContainmentEList<XSSProperty>(XSSProperty.class, this, ModelPackage.XSS_CLASS__XSSPROPERTIES);
		}
		return xssproperties;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@SuppressWarnings("unchecked")
	@Override
	public NotificationChain eInverseAdd(InternalEObject otherEnd, int featureID, NotificationChain msgs) {
		switch (featureID) {
			case ModelPackage.XSS_CLASS__XSSIMPORTS:
				return ((InternalEList<InternalEObject>)(InternalEList<?>)getXssimports()).basicAdd(otherEnd, msgs);
		}
		return super.eInverseAdd(otherEnd, featureID, msgs);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public NotificationChain eInverseRemove(InternalEObject otherEnd, int featureID, NotificationChain msgs) {
		switch (featureID) {
			case ModelPackage.XSS_CLASS__XSSIMPORTS:
				return ((InternalEList<?>)getXssimports()).basicRemove(otherEnd, msgs);
			case ModelPackage.XSS_CLASS__XSSMETHODS:
				return ((InternalEList<?>)getXssmethods()).basicRemove(otherEnd, msgs);
			case ModelPackage.XSS_CLASS__XSSEVENTS:
				return ((InternalEList<?>)getXssevents()).basicRemove(otherEnd, msgs);
			case ModelPackage.XSS_CLASS__XSSPROPERTIES:
				return ((InternalEList<?>)getXssproperties()).basicRemove(otherEnd, msgs);
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
			case ModelPackage.XSS_CLASS__ID:
				return getId();
			case ModelPackage.XSS_CLASS__SUPER:
				if (resolve) return getSuper();
				return basicGetSuper();
			case ModelPackage.XSS_CLASS__XSSIMPORTS:
				return getXssimports();
			case ModelPackage.XSS_CLASS__XSSMETHODS:
				return getXssmethods();
			case ModelPackage.XSS_CLASS__XSSEVENTS:
				return getXssevents();
			case ModelPackage.XSS_CLASS__XSSPROPERTIES:
				return getXssproperties();
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
			case ModelPackage.XSS_CLASS__ID:
				setId((String)newValue);
				return;
			case ModelPackage.XSS_CLASS__SUPER:
				setSuper((XSSClass)newValue);
				return;
			case ModelPackage.XSS_CLASS__XSSIMPORTS:
				getXssimports().clear();
				getXssimports().addAll((Collection<? extends XSSImport>)newValue);
				return;
			case ModelPackage.XSS_CLASS__XSSMETHODS:
				getXssmethods().clear();
				getXssmethods().addAll((Collection<? extends XSSMethod>)newValue);
				return;
			case ModelPackage.XSS_CLASS__XSSEVENTS:
				getXssevents().clear();
				getXssevents().addAll((Collection<? extends XSSEvent>)newValue);
				return;
			case ModelPackage.XSS_CLASS__XSSPROPERTIES:
				getXssproperties().clear();
				getXssproperties().addAll((Collection<? extends XSSProperty>)newValue);
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
			case ModelPackage.XSS_CLASS__ID:
				setId(ID_EDEFAULT);
				return;
			case ModelPackage.XSS_CLASS__SUPER:
				setSuper((XSSClass)null);
				return;
			case ModelPackage.XSS_CLASS__XSSIMPORTS:
				getXssimports().clear();
				return;
			case ModelPackage.XSS_CLASS__XSSMETHODS:
				getXssmethods().clear();
				return;
			case ModelPackage.XSS_CLASS__XSSEVENTS:
				getXssevents().clear();
				return;
			case ModelPackage.XSS_CLASS__XSSPROPERTIES:
				getXssproperties().clear();
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
			case ModelPackage.XSS_CLASS__ID:
				return ID_EDEFAULT == null ? id != null : !ID_EDEFAULT.equals(id);
			case ModelPackage.XSS_CLASS__SUPER:
				return super_ != null;
			case ModelPackage.XSS_CLASS__XSSIMPORTS:
				return xssimports != null && !xssimports.isEmpty();
			case ModelPackage.XSS_CLASS__XSSMETHODS:
				return xssmethods != null && !xssmethods.isEmpty();
			case ModelPackage.XSS_CLASS__XSSEVENTS:
				return xssevents != null && !xssevents.isEmpty();
			case ModelPackage.XSS_CLASS__XSSPROPERTIES:
				return xssproperties != null && !xssproperties.isEmpty();
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

} //XSSClassImpl
