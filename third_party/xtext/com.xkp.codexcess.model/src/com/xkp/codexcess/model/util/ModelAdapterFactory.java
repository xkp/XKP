/**
 */
package com.xkp.codexcess.model.util;

import com.xkp.codexcess.model.*;

import java.util.Map;

import org.eclipse.emf.common.notify.Adapter;
import org.eclipse.emf.common.notify.Notifier;

import org.eclipse.emf.common.notify.impl.AdapterFactoryImpl;

import org.eclipse.emf.ecore.EObject;

/**
 * <!-- begin-user-doc -->
 * The <b>Adapter Factory</b> for the model.
 * It provides an adapter <code>createXXX</code> method for each class of the model.
 * <!-- end-user-doc -->
 * @see com.xkp.codexcess.model.ModelPackage
 * @generated
 */
public class ModelAdapterFactory extends AdapterFactoryImpl {
	/**
	 * The cached model package.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected static ModelPackage modelPackage;

	/**
	 * Creates an instance of the adapter factory.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public ModelAdapterFactory() {
		if (modelPackage == null) {
			modelPackage = ModelPackage.eINSTANCE;
		}
	}

	/**
	 * Returns whether this factory is applicable for the type of the object.
	 * <!-- begin-user-doc -->
	 * This implementation returns <code>true</code> if the object is either the model's package or is an instance object of the model.
	 * <!-- end-user-doc -->
	 * @return whether this factory is applicable for the type of the object.
	 * @generated
	 */
	@Override
	public boolean isFactoryForType(Object object) {
		if (object == modelPackage) {
			return true;
		}
		if (object instanceof EObject) {
			return ((EObject)object).eClass().getEPackage() == modelPackage;
		}
		return false;
	}

	/**
	 * The switch that delegates to the <code>createXXX</code> methods.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected ModelSwitch<Adapter> modelSwitch =
		new ModelSwitch<Adapter>() {
			@Override
			public Adapter caseXSSClass(XSSClass object) {
				return createXSSClassAdapter();
			}
			@Override
			public Adapter caseXSSEvent(XSSEvent object) {
				return createXSSEventAdapter();
			}
			@Override
			public Adapter caseXSSIdiom(XSSIdiom object) {
				return createXSSIdiomAdapter();
			}
			@Override
			public Adapter caseXSSImport(XSSImport object) {
				return createXSSImportAdapter();
			}
			@Override
			public Adapter caseXSSInstance(XSSInstance object) {
				return createXSSInstanceAdapter();
			}
			@Override
			public Adapter caseXSSMethod(XSSMethod object) {
				return createXSSMethodAdapter();
			}
			@Override
			public Adapter caseXSSProject(XSSProject object) {
				return createXSSProjectAdapter();
			}
			@Override
			public Adapter caseXSSProperty(XSSProperty object) {
				return createXSSPropertyAdapter();
			}
			@Override
			public <T, T1> Adapter caseMap(Map<T, T1> object) {
				return createMapAdapter();
			}
			@Override
			public Adapter defaultCase(EObject object) {
				return createEObjectAdapter();
			}
		};

	/**
	 * Creates an adapter for the <code>target</code>.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param target the object to adapt.
	 * @return the adapter for the <code>target</code>.
	 * @generated
	 */
	@Override
	public Adapter createAdapter(Notifier target) {
		return modelSwitch.doSwitch((EObject)target);
	}


	/**
	 * Creates a new adapter for an object of class '{@link com.xkp.codexcess.model.XSSClass <em>XSS Class</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see com.xkp.codexcess.model.XSSClass
	 * @generated
	 */
	public Adapter createXSSClassAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link com.xkp.codexcess.model.XSSEvent <em>XSS Event</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see com.xkp.codexcess.model.XSSEvent
	 * @generated
	 */
	public Adapter createXSSEventAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link com.xkp.codexcess.model.XSSIdiom <em>XSS Idiom</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see com.xkp.codexcess.model.XSSIdiom
	 * @generated
	 */
	public Adapter createXSSIdiomAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link com.xkp.codexcess.model.XSSImport <em>XSS Import</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see com.xkp.codexcess.model.XSSImport
	 * @generated
	 */
	public Adapter createXSSImportAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link com.xkp.codexcess.model.XSSInstance <em>XSS Instance</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see com.xkp.codexcess.model.XSSInstance
	 * @generated
	 */
	public Adapter createXSSInstanceAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link com.xkp.codexcess.model.XSSMethod <em>XSS Method</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see com.xkp.codexcess.model.XSSMethod
	 * @generated
	 */
	public Adapter createXSSMethodAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link com.xkp.codexcess.model.XSSProject <em>XSS Project</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see com.xkp.codexcess.model.XSSProject
	 * @generated
	 */
	public Adapter createXSSProjectAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link com.xkp.codexcess.model.XSSProperty <em>XSS Property</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see com.xkp.codexcess.model.XSSProperty
	 * @generated
	 */
	public Adapter createXSSPropertyAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for an object of class '{@link java.util.Map <em>Map</em>}'.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null so that we can easily ignore cases;
	 * it's useful to ignore a case when inheritance will catch all the cases anyway.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see java.util.Map
	 * @generated
	 */
	public Adapter createMapAdapter() {
		return null;
	}

	/**
	 * Creates a new adapter for the default case.
	 * <!-- begin-user-doc -->
	 * This default implementation returns null.
	 * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @generated
	 */
	public Adapter createEObjectAdapter() {
		return null;
	}

} //ModelAdapterFactory
