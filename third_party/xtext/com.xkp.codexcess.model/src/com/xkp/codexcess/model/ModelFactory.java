/**
 */
package com.xkp.codexcess.model;

import org.eclipse.emf.ecore.EFactory;

/**
 * <!-- begin-user-doc -->
 * The <b>Factory</b> for the model.
 * It provides a create method for each non-abstract class of the model.
 * <!-- end-user-doc -->
 * @see com.xkp.codexcess.model.ModelPackage
 * @generated
 */
public interface ModelFactory extends EFactory {
	/**
	 * The singleton instance of the factory.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	ModelFactory eINSTANCE = com.xkp.codexcess.model.impl.ModelFactoryImpl.init();

	/**
	 * Returns a new object of class '<em>XSS Class</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>XSS Class</em>'.
	 * @generated
	 */
	XSSClass createXSSClass();

	/**
	 * Returns a new object of class '<em>XSS Event</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>XSS Event</em>'.
	 * @generated
	 */
	XSSEvent createXSSEvent();

	/**
	 * Returns a new object of class '<em>XSSI Property</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>XSSI Property</em>'.
	 * @generated
	 */
	XSSIProperty createXSSIProperty();

	/**
	 * Returns a new object of class '<em>XSS Idiom</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>XSS Idiom</em>'.
	 * @generated
	 */
	XSSIdiom createXSSIdiom();

	/**
	 * Returns a new object of class '<em>XSS Import</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>XSS Import</em>'.
	 * @generated
	 */
	XSSImport createXSSImport();

	/**
	 * Returns a new object of class '<em>XSS Instance</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>XSS Instance</em>'.
	 * @generated
	 */
	XSSInstance createXSSInstance();

	/**
	 * Returns a new object of class '<em>XSS Method</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>XSS Method</em>'.
	 * @generated
	 */
	XSSMethod createXSSMethod();

	/**
	 * Returns a new object of class '<em>XSS Project</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>XSS Project</em>'.
	 * @generated
	 */
	XSSProject createXSSProject();

	/**
	 * Returns a new object of class '<em>XSS Property</em>'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return a new object of class '<em>XSS Property</em>'.
	 * @generated
	 */
	XSSProperty createXSSProperty();

	/**
	 * Returns the package supported by this factory.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the package supported by this factory.
	 * @generated
	 */
	ModelPackage getModelPackage();

} //ModelFactory
