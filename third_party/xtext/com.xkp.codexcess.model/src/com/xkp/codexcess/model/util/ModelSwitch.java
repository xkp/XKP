/**
 */
package com.xkp.codexcess.model.util;

import com.xkp.codexcess.model.*;

import java.util.Map;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EPackage;

import org.eclipse.emf.ecore.util.Switch;

/**
 * <!-- begin-user-doc -->
 * The <b>Switch</b> for the model's inheritance hierarchy.
 * It supports the call {@link #doSwitch(EObject) doSwitch(object)}
 * to invoke the <code>caseXXX</code> method for each class of the model,
 * starting with the actual class of the object
 * and proceeding up the inheritance hierarchy
 * until a non-null result is returned,
 * which is the result of the switch.
 * <!-- end-user-doc -->
 * @see com.xkp.codexcess.model.ModelPackage
 * @generated
 */
public class ModelSwitch<T2> extends Switch<T2> {
	/**
	 * The cached model package
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected static ModelPackage modelPackage;

	/**
	 * Creates an instance of the switch.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public ModelSwitch() {
		if (modelPackage == null) {
			modelPackage = ModelPackage.eINSTANCE;
		}
	}

	/**
	 * Checks whether this is a switch for the given package.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @parameter ePackage the package in question.
	 * @return whether this is a switch for the given package.
	 * @generated
	 */
	@Override
	protected boolean isSwitchFor(EPackage ePackage) {
		return ePackage == modelPackage;
	}

	/**
	 * Calls <code>caseXXX</code> for each class of the model until one returns a non null result; it yields that result.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the first non-null result returned by a <code>caseXXX</code> call.
	 * @generated
	 */
	@Override
	protected T2 doSwitch(int classifierID, EObject theEObject) {
		switch (classifierID) {
			case ModelPackage.XSS_CLASS: {
				XSSClass xssClass = (XSSClass)theEObject;
				T2 result = caseXSSClass(xssClass);
				if (result == null) result = defaultCase(theEObject);
				return result;
			}
			case ModelPackage.XSS_EVENT: {
				XSSEvent xssEvent = (XSSEvent)theEObject;
				T2 result = caseXSSEvent(xssEvent);
				if (result == null) result = defaultCase(theEObject);
				return result;
			}
			case ModelPackage.XSS_IDIOM: {
				XSSIdiom xssIdiom = (XSSIdiom)theEObject;
				T2 result = caseXSSIdiom(xssIdiom);
				if (result == null) result = defaultCase(theEObject);
				return result;
			}
			case ModelPackage.XSS_IMPORT: {
				XSSImport xssImport = (XSSImport)theEObject;
				T2 result = caseXSSImport(xssImport);
				if (result == null) result = defaultCase(theEObject);
				return result;
			}
			case ModelPackage.XSS_INSTANCE: {
				XSSInstance xssInstance = (XSSInstance)theEObject;
				T2 result = caseXSSInstance(xssInstance);
				if (result == null) result = defaultCase(theEObject);
				return result;
			}
			case ModelPackage.XSS_METHOD: {
				XSSMethod xssMethod = (XSSMethod)theEObject;
				T2 result = caseXSSMethod(xssMethod);
				if (result == null) result = defaultCase(theEObject);
				return result;
			}
			case ModelPackage.XSS_PROJECT: {
				XSSProject xssProject = (XSSProject)theEObject;
				T2 result = caseXSSProject(xssProject);
				if (result == null) result = defaultCase(theEObject);
				return result;
			}
			case ModelPackage.XSS_PROPERTY: {
				XSSProperty xssProperty = (XSSProperty)theEObject;
				T2 result = caseXSSProperty(xssProperty);
				if (result == null) result = defaultCase(theEObject);
				return result;
			}
			default: return defaultCase(theEObject);
		}
	}

	/**
	 * Returns the result of interpreting the object as an instance of '<em>XSS Class</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpreting the object as an instance of '<em>XSS Class</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public T2 caseXSSClass(XSSClass object) {
		return null;
	}

	/**
	 * Returns the result of interpreting the object as an instance of '<em>XSS Event</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpreting the object as an instance of '<em>XSS Event</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public T2 caseXSSEvent(XSSEvent object) {
		return null;
	}

	/**
	 * Returns the result of interpreting the object as an instance of '<em>XSS Idiom</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpreting the object as an instance of '<em>XSS Idiom</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public T2 caseXSSIdiom(XSSIdiom object) {
		return null;
	}

	/**
	 * Returns the result of interpreting the object as an instance of '<em>XSS Import</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpreting the object as an instance of '<em>XSS Import</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public T2 caseXSSImport(XSSImport object) {
		return null;
	}

	/**
	 * Returns the result of interpreting the object as an instance of '<em>XSS Instance</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpreting the object as an instance of '<em>XSS Instance</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public T2 caseXSSInstance(XSSInstance object) {
		return null;
	}

	/**
	 * Returns the result of interpreting the object as an instance of '<em>XSS Method</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpreting the object as an instance of '<em>XSS Method</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public T2 caseXSSMethod(XSSMethod object) {
		return null;
	}

	/**
	 * Returns the result of interpreting the object as an instance of '<em>XSS Project</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpreting the object as an instance of '<em>XSS Project</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public T2 caseXSSProject(XSSProject object) {
		return null;
	}

	/**
	 * Returns the result of interpreting the object as an instance of '<em>XSS Property</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpreting the object as an instance of '<em>XSS Property</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public T2 caseXSSProperty(XSSProperty object) {
		return null;
	}

	/**
	 * Returns the result of interpreting the object as an instance of '<em>Map</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpreting the object as an instance of '<em>Map</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
	 * @generated
	 */
	public <T, T1> T2 caseMap(Map<T, T1> object) {
		return null;
	}

	/**
	 * Returns the result of interpreting the object as an instance of '<em>EObject</em>'.
	 * <!-- begin-user-doc -->
	 * This implementation returns null;
	 * returning a non-null result will terminate the switch, but this is the last case anyway.
	 * <!-- end-user-doc -->
	 * @param object the target of the switch.
	 * @return the result of interpreting the object as an instance of '<em>EObject</em>'.
	 * @see #doSwitch(org.eclipse.emf.ecore.EObject)
	 * @generated
	 */
	@Override
	public T2 defaultCase(EObject object) {
		return null;
	}

} //ModelSwitch
