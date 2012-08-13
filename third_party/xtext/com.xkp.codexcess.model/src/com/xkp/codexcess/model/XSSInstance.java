package com.xkp.codexcess.model;

import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EObject;
import java.util.List;

/**
 * @model
 */
public interface XSSInstance extends EObject
{
	/**
	* @model
	*/
	String getId();

	/**
	 * Sets the value of the '{@link com.xkp.codexcess.model.XSSInstance#getId <em>Id</em>}' attribute.
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
	XSSClass getXssclass();

	/**
	 * Sets the value of the '{@link com.xkp.codexcess.model.XSSInstance#getXssclass <em>Xssclass</em>}' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Xssclass</em>' reference.
	 * @see #getXssclass()
	 * @generated
	 */
	void setXssclass(XSSClass value);

	/**
	* @model containment="true"
	*/
	EList<XSSInstance> getChilds();

	/**
	* @model containment="true"
	*/
	EList<XSSIProperty> getProperties();

	/**
	* @model
	*/
	XSSInstance getParent();

	/**
	 * Sets the value of the '{@link com.xkp.codexcess.model.XSSInstance#getParent <em>Parent</em>}' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Parent</em>' reference.
	 * @see #getParent()
	 * @generated
	 */
	void setParent(XSSInstance value);
}
