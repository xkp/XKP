package com.xkp.codexcess.model;

import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EObject;
import java.util.List;

/**
 * @model
 */
public interface XSSClass extends EObject
{
	/**
	* @model
	*/
	String getId();

	/**
	 * Sets the value of the '{@link com.xkp.codexcess.model.XSSClass#getId <em>Id</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Id</em>' attribute.
	 * @see #getId()
	 * @generated
	 */
	void setId(String value);

	/**
	* @model
	*/
	XSSClass getSuper();

	/**
	 * Sets the value of the '{@link com.xkp.codexcess.model.XSSClass#getSuper <em>Super</em>}' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Super</em>' reference.
	 * @see #getSuper()
	 * @generated
	 */
	void setSuper(XSSClass value);

	/**
	* @model opposite="xssclass"
	*/
	EList<XSSImport> getXssimports();

	/**
	* @model containment="true"
	*/
	EList<XSSMethod> getXssmethods();

	/**
	* @model containment="true"
	*/
	EList<XSSEvent> getXssevents();

	/**
	* @model containment="true"
	*/
	EList<XSSProperty> getXssproperties();
}
