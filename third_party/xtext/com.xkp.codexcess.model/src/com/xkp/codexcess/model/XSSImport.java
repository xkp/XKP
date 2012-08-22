package com.xkp.codexcess.model;

import org.eclipse.emf.ecore.EObject;

/**
 * @model
 */
public interface XSSImport extends EObject
{
	/**
	* @model opposite="xssimports"
	*/
	XSSClass getXssclass();

	/**
	 * Sets the value of the '{@link com.xkp.codexcess.model.XSSImport#getXssclass <em>Xssclass</em>}' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Xssclass</em>' reference.
	 * @see #getXssclass()
	 * @generated
	 */
	void setXssclass(XSSClass value);
}
