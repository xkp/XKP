/**
 */
package com.xkp.codexcess.model.impl;

import com.xkp.codexcess.model.ModelPackage;
import com.xkp.codexcess.model.XSSClass;
import com.xkp.codexcess.model.XSSIdiom;
import com.xkp.codexcess.model.XSSInstance;
import com.xkp.codexcess.model.XSSProject;

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
 * An implementation of the model object '<em><b>XSS Project</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link com.xkp.codexcess.model.impl.XSSProjectImpl#getName <em>Name</em>}</li>
 *   <li>{@link com.xkp.codexcess.model.impl.XSSProjectImpl#getXssidioms <em>Xssidioms</em>}</li>
 *   <li>{@link com.xkp.codexcess.model.impl.XSSProjectImpl#getXssclasses <em>Xssclasses</em>}</li>
 *   <li>{@link com.xkp.codexcess.model.impl.XSSProjectImpl#getApplication <em>Application</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class XSSProjectImpl extends EObjectImpl implements XSSProject {
	/**
	 * The default value of the '{@link #getName() <em>Name</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getName()
	 * @generated
	 * @ordered
	 */
	protected static final String NAME_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getName() <em>Name</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getName()
	 * @generated
	 * @ordered
	 */
	protected String name = NAME_EDEFAULT;

	/**
	 * The cached value of the '{@link #getXssidioms() <em>Xssidioms</em>}' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getXssidioms()
	 * @generated
	 * @ordered
	 */
	protected EList<XSSIdiom> xssidioms;

	/**
	 * The cached value of the '{@link #getXssclasses() <em>Xssclasses</em>}' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getXssclasses()
	 * @generated
	 * @ordered
	 */
	protected EList<XSSClass> xssclasses;

	/**
	 * The cached value of the '{@link #getApplication() <em>Application</em>}' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getApplication()
	 * @generated
	 * @ordered
	 */
	protected XSSInstance application;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected XSSProjectImpl() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	protected EClass eStaticClass() {
		return ModelPackage.Literals.XSS_PROJECT;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public String getName() {
		return name;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setName(String newName) {
		String oldName = name;
		name = newName;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, ModelPackage.XSS_PROJECT__NAME, oldName, name));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public EList<XSSIdiom> getXssidioms() {
		if (xssidioms == null) {
			xssidioms = new EObjectContainmentEList<XSSIdiom>(XSSIdiom.class, this, ModelPackage.XSS_PROJECT__XSSIDIOMS);
		}
		return xssidioms;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public EList<XSSClass> getXssclasses() {
		if (xssclasses == null) {
			xssclasses = new EObjectContainmentEList<XSSClass>(XSSClass.class, this, ModelPackage.XSS_PROJECT__XSSCLASSES);
		}
		return xssclasses;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public XSSInstance getApplication() {
		if (application != null && application.eIsProxy()) {
			InternalEObject oldApplication = (InternalEObject)application;
			application = (XSSInstance)eResolveProxy(oldApplication);
			if (application != oldApplication) {
				if (eNotificationRequired())
					eNotify(new ENotificationImpl(this, Notification.RESOLVE, ModelPackage.XSS_PROJECT__APPLICATION, oldApplication, application));
			}
		}
		return application;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public XSSInstance basicGetApplication() {
		return application;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setApplication(XSSInstance newApplication) {
		XSSInstance oldApplication = application;
		application = newApplication;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, ModelPackage.XSS_PROJECT__APPLICATION, oldApplication, application));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public NotificationChain eInverseRemove(InternalEObject otherEnd, int featureID, NotificationChain msgs) {
		switch (featureID) {
			case ModelPackage.XSS_PROJECT__XSSIDIOMS:
				return ((InternalEList<?>)getXssidioms()).basicRemove(otherEnd, msgs);
			case ModelPackage.XSS_PROJECT__XSSCLASSES:
				return ((InternalEList<?>)getXssclasses()).basicRemove(otherEnd, msgs);
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
			case ModelPackage.XSS_PROJECT__NAME:
				return getName();
			case ModelPackage.XSS_PROJECT__XSSIDIOMS:
				return getXssidioms();
			case ModelPackage.XSS_PROJECT__XSSCLASSES:
				return getXssclasses();
			case ModelPackage.XSS_PROJECT__APPLICATION:
				if (resolve) return getApplication();
				return basicGetApplication();
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
			case ModelPackage.XSS_PROJECT__NAME:
				setName((String)newValue);
				return;
			case ModelPackage.XSS_PROJECT__XSSIDIOMS:
				getXssidioms().clear();
				getXssidioms().addAll((Collection<? extends XSSIdiom>)newValue);
				return;
			case ModelPackage.XSS_PROJECT__XSSCLASSES:
				getXssclasses().clear();
				getXssclasses().addAll((Collection<? extends XSSClass>)newValue);
				return;
			case ModelPackage.XSS_PROJECT__APPLICATION:
				setApplication((XSSInstance)newValue);
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
			case ModelPackage.XSS_PROJECT__NAME:
				setName(NAME_EDEFAULT);
				return;
			case ModelPackage.XSS_PROJECT__XSSIDIOMS:
				getXssidioms().clear();
				return;
			case ModelPackage.XSS_PROJECT__XSSCLASSES:
				getXssclasses().clear();
				return;
			case ModelPackage.XSS_PROJECT__APPLICATION:
				setApplication((XSSInstance)null);
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
			case ModelPackage.XSS_PROJECT__NAME:
				return NAME_EDEFAULT == null ? name != null : !NAME_EDEFAULT.equals(name);
			case ModelPackage.XSS_PROJECT__XSSIDIOMS:
				return xssidioms != null && !xssidioms.isEmpty();
			case ModelPackage.XSS_PROJECT__XSSCLASSES:
				return xssclasses != null && !xssclasses.isEmpty();
			case ModelPackage.XSS_PROJECT__APPLICATION:
				return application != null;
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
		result.append(" (name: ");
		result.append(name);
		result.append(')');
		return result.toString();
	}

} //XSSProjectImpl
