package com.xkp.codexcess.model;

import org.eclipse.emf.ecore.EObject;

/**
 * @model
 */
public interface XSSIProperty extends EObject
{
	/**
	* @model
	*/
	String getId();

	/**
	 * Sets the value of the '{@link com.xkp.codexcess.model.XSSIProperty#getId <em>Id</em>}' attribute.
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
	Object getValue();

	/**
	 * Sets the value of the '{@link com.xkp.codexcess.model.XSSIProperty#getValue <em>Value</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Value</em>' attribute.
	 * @see #getValue()
	 * @generated
	 */
	void setValue(Object value);
}
