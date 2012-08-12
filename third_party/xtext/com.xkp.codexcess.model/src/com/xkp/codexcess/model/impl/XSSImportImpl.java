/**
 */
package com.xkp.codexcess.model.impl;

import com.xkp.codexcess.model.ModelPackage;
import com.xkp.codexcess.model.XSSClass;
import com.xkp.codexcess.model.XSSImport;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>XSS Import</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link com.xkp.codexcess.model.impl.XSSImportImpl#getXssclass <em>Xssclass</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class XSSImportImpl extends EObjectImpl implements XSSImport {
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
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected XSSImportImpl() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	protected EClass eStaticClass() {
		return ModelPackage.Literals.XSS_IMPORT;
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
					eNotify(new ENotificationImpl(this, Notification.RESOLVE, ModelPackage.XSS_IMPORT__XSSCLASS, oldXssclass, xssclass));
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
	public NotificationChain basicSetXssclass(XSSClass newXssclass, NotificationChain msgs) {
		XSSClass oldXssclass = xssclass;
		xssclass = newXssclass;
		if (eNotificationRequired()) {
			ENotificationImpl notification = new ENotificationImpl(this, Notification.SET, ModelPackage.XSS_IMPORT__XSSCLASS, oldXssclass, newXssclass);
			if (msgs == null) msgs = notification; else msgs.add(notification);
		}
		return msgs;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setXssclass(XSSClass newXssclass) {
		if (newXssclass != xssclass) {
			NotificationChain msgs = null;
			if (xssclass != null)
				msgs = ((InternalEObject)xssclass).eInverseRemove(this, ModelPackage.XSS_CLASS__XSSIMPORTS, XSSClass.class, msgs);
			if (newXssclass != null)
				msgs = ((InternalEObject)newXssclass).eInverseAdd(this, ModelPackage.XSS_CLASS__XSSIMPORTS, XSSClass.class, msgs);
			msgs = basicSetXssclass(newXssclass, msgs);
			if (msgs != null) msgs.dispatch();
		}
		else if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, ModelPackage.XSS_IMPORT__XSSCLASS, newXssclass, newXssclass));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public NotificationChain eInverseAdd(InternalEObject otherEnd, int featureID, NotificationChain msgs) {
		switch (featureID) {
			case ModelPackage.XSS_IMPORT__XSSCLASS:
				if (xssclass != null)
					msgs = ((InternalEObject)xssclass).eInverseRemove(this, ModelPackage.XSS_CLASS__XSSIMPORTS, XSSClass.class, msgs);
				return basicSetXssclass((XSSClass)otherEnd, msgs);
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
			case ModelPackage.XSS_IMPORT__XSSCLASS:
				return basicSetXssclass(null, msgs);
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
			case ModelPackage.XSS_IMPORT__XSSCLASS:
				if (resolve) return getXssclass();
				return basicGetXssclass();
		}
		return super.eGet(featureID, resolve, coreType);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public void eSet(int featureID, Object newValue) {
		switch (featureID) {
			case ModelPackage.XSS_IMPORT__XSSCLASS:
				setXssclass((XSSClass)newValue);
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
			case ModelPackage.XSS_IMPORT__XSSCLASS:
				setXssclass((XSSClass)null);
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
			case ModelPackage.XSS_IMPORT__XSSCLASS:
				return xssclass != null;
		}
		return super.eIsSet(featureID);
	}

} //XSSImportImpl
