package com.xkp.codexcess.model;

import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EObject;
import java.util.List;

/**
 * @model
 */
public interface XSSProject extends EObject
{
	/**
	* @model
	*/
	String getName();

	/**
	 * Sets the value of the '{@link com.xkp.codexcess.model.XSSProject#getName <em>Name</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Name</em>' attribute.
	 * @see #getName()
	 * @generated
	 */
	void setName(String value);

	/**
	* @model containment="true"
	*/
	EList<XSSIdiom> getXssidioms();

	/**
	* @model containment="true"
	*/
	EList<XSSClass> getXssclasses();

	/**
	* @model
	*/
	XSSInstance getApplication();

	/**
	 * Sets the value of the '{@link com.xkp.codexcess.model.XSSProject#getApplication <em>Application</em>}' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Application</em>' reference.
	 * @see #getApplication()
	 * @generated
	 */
	void setApplication(XSSInstance value);
}
