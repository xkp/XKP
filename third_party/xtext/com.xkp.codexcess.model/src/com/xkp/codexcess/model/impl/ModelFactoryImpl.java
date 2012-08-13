/**
 */
package com.xkp.codexcess.model.impl;

import com.xkp.codexcess.model.*;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EPackage;

import org.eclipse.emf.ecore.impl.EFactoryImpl;

import org.eclipse.emf.ecore.plugin.EcorePlugin;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model <b>Factory</b>.
 * <!-- end-user-doc -->
 * @generated
 */
public class ModelFactoryImpl extends EFactoryImpl implements ModelFactory {
	/**
	 * Creates the default factory implementation.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static ModelFactory init() {
		try {
			ModelFactory theModelFactory = (ModelFactory)EPackage.Registry.INSTANCE.getEFactory("http:///com/xkp/codexcess/model.ecore"); 
			if (theModelFactory != null) {
				return theModelFactory;
			}
		}
		catch (Exception exception) {
			EcorePlugin.INSTANCE.log(exception);
		}
		return new ModelFactoryImpl();
	}

	/**
	 * Creates an instance of the factory.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public ModelFactoryImpl() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public EObject create(EClass eClass) {
		switch (eClass.getClassifierID()) {
			case ModelPackage.XSS_CLASS: return createXSSClass();
			case ModelPackage.XSS_EVENT: return createXSSEvent();
			case ModelPackage.XSSI_PROPERTY: return createXSSIProperty();
			case ModelPackage.XSS_IDIOM: return createXSSIdiom();
			case ModelPackage.XSS_IMPORT: return createXSSImport();
			case ModelPackage.XSS_INSTANCE: return createXSSInstance();
			case ModelPackage.XSS_METHOD: return createXSSMethod();
			case ModelPackage.XSS_PROJECT: return createXSSProject();
			case ModelPackage.XSS_PROPERTY: return createXSSProperty();
			default:
				throw new IllegalArgumentException("The class '" + eClass.getName() + "' is not a valid classifier");
		}
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public XSSClass createXSSClass() {
		XSSClassImpl xssClass = new XSSClassImpl();
		return xssClass;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public XSSEvent createXSSEvent() {
		XSSEventImpl xssEvent = new XSSEventImpl();
		return xssEvent;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public XSSIProperty createXSSIProperty() {
		XSSIPropertyImpl xssiProperty = new XSSIPropertyImpl();
		return xssiProperty;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public XSSIdiom createXSSIdiom() {
		XSSIdiomImpl xssIdiom = new XSSIdiomImpl();
		return xssIdiom;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public XSSImport createXSSImport() {
		XSSImportImpl xssImport = new XSSImportImpl();
		return xssImport;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public XSSInstance createXSSInstance() {
		XSSInstanceImpl xssInstance = new XSSInstanceImpl();
		return xssInstance;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public XSSMethod createXSSMethod() {
		XSSMethodImpl xssMethod = new XSSMethodImpl();
		return xssMethod;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public XSSProject createXSSProject() {
		XSSProjectImpl xssProject = new XSSProjectImpl();
		return xssProject;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public XSSProperty createXSSProperty() {
		XSSPropertyImpl xssProperty = new XSSPropertyImpl();
		return xssProperty;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public ModelPackage getModelPackage() {
		return (ModelPackage)getEPackage();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @deprecated
	 * @generated
	 */
	@Deprecated
	public static ModelPackage getPackage() {
		return ModelPackage.eINSTANCE;
	}

} //ModelFactoryImpl
