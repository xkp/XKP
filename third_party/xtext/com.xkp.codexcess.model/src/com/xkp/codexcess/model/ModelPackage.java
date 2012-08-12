/**
 */
package com.xkp.codexcess.model;

import org.eclipse.emf.ecore.EAttribute;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EPackage;
import org.eclipse.emf.ecore.EReference;

/**
 * <!-- begin-user-doc -->
 * The <b>Package</b> for the model.
 * It contains accessors for the meta objects to represent
 * <ul>
 *   <li>each class,</li>
 *   <li>each feature of each class,</li>
 *   <li>each enum,</li>
 *   <li>and each data type</li>
 * </ul>
 * <!-- end-user-doc -->
 * @see com.xkp.codexcess.model.ModelFactory
 * @model kind="package"
 * @generated
 */
public interface ModelPackage extends EPackage {
	/**
	 * The package name.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	String eNAME = "model";

	/**
	 * The package namespace URI.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	String eNS_URI = "http:///com/xkp/codexcess/model.ecore";

	/**
	 * The package namespace name.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	String eNS_PREFIX = "com.xkp.codexcess.model";

	/**
	 * The singleton instance of the package.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	ModelPackage eINSTANCE = com.xkp.codexcess.model.impl.ModelPackageImpl.init();

	/**
	 * The meta object id for the '{@link com.xkp.codexcess.model.impl.XSSClassImpl <em>XSS Class</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see com.xkp.codexcess.model.impl.XSSClassImpl
	 * @see com.xkp.codexcess.model.impl.ModelPackageImpl#getXSSClass()
	 * @generated
	 */
	int XSS_CLASS = 0;

	/**
	 * The feature id for the '<em><b>Id</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int XSS_CLASS__ID = 0;

	/**
	 * The feature id for the '<em><b>Xssimports</b></em>' reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int XSS_CLASS__XSSIMPORTS = 1;

	/**
	 * The feature id for the '<em><b>Xssmethods</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int XSS_CLASS__XSSMETHODS = 2;

	/**
	 * The feature id for the '<em><b>Xssevents</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int XSS_CLASS__XSSEVENTS = 3;

	/**
	 * The feature id for the '<em><b>Xssproperties</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int XSS_CLASS__XSSPROPERTIES = 4;

	/**
	 * The number of structural features of the '<em>XSS Class</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int XSS_CLASS_FEATURE_COUNT = 5;

	/**
	 * The meta object id for the '{@link com.xkp.codexcess.model.impl.XSSEventImpl <em>XSS Event</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see com.xkp.codexcess.model.impl.XSSEventImpl
	 * @see com.xkp.codexcess.model.impl.ModelPackageImpl#getXSSEvent()
	 * @generated
	 */
	int XSS_EVENT = 1;

	/**
	 * The feature id for the '<em><b>Id</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int XSS_EVENT__ID = 0;

	/**
	 * The number of structural features of the '<em>XSS Event</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int XSS_EVENT_FEATURE_COUNT = 1;

	/**
	 * The meta object id for the '{@link com.xkp.codexcess.model.impl.XSSIdiomImpl <em>XSS Idiom</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see com.xkp.codexcess.model.impl.XSSIdiomImpl
	 * @see com.xkp.codexcess.model.impl.ModelPackageImpl#getXSSIdiom()
	 * @generated
	 */
	int XSS_IDIOM = 2;

	/**
	 * The feature id for the '<em><b>Id</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int XSS_IDIOM__ID = 0;

	/**
	 * The feature id for the '<em><b>Xssclasses</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int XSS_IDIOM__XSSCLASSES = 1;

	/**
	 * The number of structural features of the '<em>XSS Idiom</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int XSS_IDIOM_FEATURE_COUNT = 2;

	/**
	 * The meta object id for the '{@link com.xkp.codexcess.model.impl.XSSImportImpl <em>XSS Import</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see com.xkp.codexcess.model.impl.XSSImportImpl
	 * @see com.xkp.codexcess.model.impl.ModelPackageImpl#getXSSImport()
	 * @generated
	 */
	int XSS_IMPORT = 3;

	/**
	 * The feature id for the '<em><b>Xssclass</b></em>' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int XSS_IMPORT__XSSCLASS = 0;

	/**
	 * The number of structural features of the '<em>XSS Import</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int XSS_IMPORT_FEATURE_COUNT = 1;

	/**
	 * The meta object id for the '{@link com.xkp.codexcess.model.impl.XSSInstanceImpl <em>XSS Instance</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see com.xkp.codexcess.model.impl.XSSInstanceImpl
	 * @see com.xkp.codexcess.model.impl.ModelPackageImpl#getXSSInstance()
	 * @generated
	 */
	int XSS_INSTANCE = 4;

	/**
	 * The feature id for the '<em><b>Id</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int XSS_INSTANCE__ID = 0;

	/**
	 * The feature id for the '<em><b>Xssclass</b></em>' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int XSS_INSTANCE__XSSCLASS = 1;

	/**
	 * The feature id for the '<em><b>Childs</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int XSS_INSTANCE__CHILDS = 2;

	/**
	 * The feature id for the '<em><b>Properties</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int XSS_INSTANCE__PROPERTIES = 3;

	/**
	 * The feature id for the '<em><b>Parent</b></em>' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int XSS_INSTANCE__PARENT = 4;

	/**
	 * The number of structural features of the '<em>XSS Instance</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int XSS_INSTANCE_FEATURE_COUNT = 5;

	/**
	 * The meta object id for the '{@link com.xkp.codexcess.model.impl.XSSMethodImpl <em>XSS Method</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see com.xkp.codexcess.model.impl.XSSMethodImpl
	 * @see com.xkp.codexcess.model.impl.ModelPackageImpl#getXSSMethod()
	 * @generated
	 */
	int XSS_METHOD = 5;

	/**
	 * The feature id for the '<em><b>Id</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int XSS_METHOD__ID = 0;

	/**
	 * The feature id for the '<em><b>Type</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int XSS_METHOD__TYPE = 1;

	/**
	 * The number of structural features of the '<em>XSS Method</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int XSS_METHOD_FEATURE_COUNT = 2;

	/**
	 * The meta object id for the '{@link com.xkp.codexcess.model.impl.XSSProjectImpl <em>XSS Project</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see com.xkp.codexcess.model.impl.XSSProjectImpl
	 * @see com.xkp.codexcess.model.impl.ModelPackageImpl#getXSSProject()
	 * @generated
	 */
	int XSS_PROJECT = 6;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int XSS_PROJECT__NAME = 0;

	/**
	 * The feature id for the '<em><b>Xssidioms</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int XSS_PROJECT__XSSIDIOMS = 1;

	/**
	 * The feature id for the '<em><b>Xssclasses</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int XSS_PROJECT__XSSCLASSES = 2;

	/**
	 * The feature id for the '<em><b>Application</b></em>' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int XSS_PROJECT__APPLICATION = 3;

	/**
	 * The number of structural features of the '<em>XSS Project</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int XSS_PROJECT_FEATURE_COUNT = 4;

	/**
	 * The meta object id for the '{@link com.xkp.codexcess.model.impl.XSSPropertyImpl <em>XSS Property</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see com.xkp.codexcess.model.impl.XSSPropertyImpl
	 * @see com.xkp.codexcess.model.impl.ModelPackageImpl#getXSSProperty()
	 * @generated
	 */
	int XSS_PROPERTY = 7;

	/**
	 * The feature id for the '<em><b>Id</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int XSS_PROPERTY__ID = 0;

	/**
	 * The feature id for the '<em><b>Type</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int XSS_PROPERTY__TYPE = 1;

	/**
	 * The number of structural features of the '<em>XSS Property</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int XSS_PROPERTY_FEATURE_COUNT = 2;

	/**
	 * The meta object id for the '{@link java.util.Map <em>Map</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see java.util.Map
	 * @see com.xkp.codexcess.model.impl.ModelPackageImpl#getMap()
	 * @generated
	 */
	int MAP = 8;

	/**
	 * The number of structural features of the '<em>Map</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int MAP_FEATURE_COUNT = 0;


	/**
	 * Returns the meta object for class '{@link com.xkp.codexcess.model.XSSClass <em>XSS Class</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>XSS Class</em>'.
	 * @see com.xkp.codexcess.model.XSSClass
	 * @generated
	 */
	EClass getXSSClass();

	/**
	 * Returns the meta object for the attribute '{@link com.xkp.codexcess.model.XSSClass#getId <em>Id</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Id</em>'.
	 * @see com.xkp.codexcess.model.XSSClass#getId()
	 * @see #getXSSClass()
	 * @generated
	 */
	EAttribute getXSSClass_Id();

	/**
	 * Returns the meta object for the reference list '{@link com.xkp.codexcess.model.XSSClass#getXssimports <em>Xssimports</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the reference list '<em>Xssimports</em>'.
	 * @see com.xkp.codexcess.model.XSSClass#getXssimports()
	 * @see #getXSSClass()
	 * @generated
	 */
	EReference getXSSClass_Xssimports();

	/**
	 * Returns the meta object for the containment reference list '{@link com.xkp.codexcess.model.XSSClass#getXssmethods <em>Xssmethods</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Xssmethods</em>'.
	 * @see com.xkp.codexcess.model.XSSClass#getXssmethods()
	 * @see #getXSSClass()
	 * @generated
	 */
	EReference getXSSClass_Xssmethods();

	/**
	 * Returns the meta object for the containment reference list '{@link com.xkp.codexcess.model.XSSClass#getXssevents <em>Xssevents</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Xssevents</em>'.
	 * @see com.xkp.codexcess.model.XSSClass#getXssevents()
	 * @see #getXSSClass()
	 * @generated
	 */
	EReference getXSSClass_Xssevents();

	/**
	 * Returns the meta object for the containment reference list '{@link com.xkp.codexcess.model.XSSClass#getXssproperties <em>Xssproperties</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Xssproperties</em>'.
	 * @see com.xkp.codexcess.model.XSSClass#getXssproperties()
	 * @see #getXSSClass()
	 * @generated
	 */
	EReference getXSSClass_Xssproperties();

	/**
	 * Returns the meta object for class '{@link com.xkp.codexcess.model.XSSEvent <em>XSS Event</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>XSS Event</em>'.
	 * @see com.xkp.codexcess.model.XSSEvent
	 * @generated
	 */
	EClass getXSSEvent();

	/**
	 * Returns the meta object for the attribute '{@link com.xkp.codexcess.model.XSSEvent#getId <em>Id</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Id</em>'.
	 * @see com.xkp.codexcess.model.XSSEvent#getId()
	 * @see #getXSSEvent()
	 * @generated
	 */
	EAttribute getXSSEvent_Id();

	/**
	 * Returns the meta object for class '{@link com.xkp.codexcess.model.XSSIdiom <em>XSS Idiom</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>XSS Idiom</em>'.
	 * @see com.xkp.codexcess.model.XSSIdiom
	 * @generated
	 */
	EClass getXSSIdiom();

	/**
	 * Returns the meta object for the attribute '{@link com.xkp.codexcess.model.XSSIdiom#getId <em>Id</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Id</em>'.
	 * @see com.xkp.codexcess.model.XSSIdiom#getId()
	 * @see #getXSSIdiom()
	 * @generated
	 */
	EAttribute getXSSIdiom_Id();

	/**
	 * Returns the meta object for the containment reference list '{@link com.xkp.codexcess.model.XSSIdiom#getXssclasses <em>Xssclasses</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Xssclasses</em>'.
	 * @see com.xkp.codexcess.model.XSSIdiom#getXssclasses()
	 * @see #getXSSIdiom()
	 * @generated
	 */
	EReference getXSSIdiom_Xssclasses();

	/**
	 * Returns the meta object for class '{@link com.xkp.codexcess.model.XSSImport <em>XSS Import</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>XSS Import</em>'.
	 * @see com.xkp.codexcess.model.XSSImport
	 * @generated
	 */
	EClass getXSSImport();

	/**
	 * Returns the meta object for the reference '{@link com.xkp.codexcess.model.XSSImport#getXssclass <em>Xssclass</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the reference '<em>Xssclass</em>'.
	 * @see com.xkp.codexcess.model.XSSImport#getXssclass()
	 * @see #getXSSImport()
	 * @generated
	 */
	EReference getXSSImport_Xssclass();

	/**
	 * Returns the meta object for class '{@link com.xkp.codexcess.model.XSSInstance <em>XSS Instance</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>XSS Instance</em>'.
	 * @see com.xkp.codexcess.model.XSSInstance
	 * @generated
	 */
	EClass getXSSInstance();

	/**
	 * Returns the meta object for the attribute '{@link com.xkp.codexcess.model.XSSInstance#getId <em>Id</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Id</em>'.
	 * @see com.xkp.codexcess.model.XSSInstance#getId()
	 * @see #getXSSInstance()
	 * @generated
	 */
	EAttribute getXSSInstance_Id();

	/**
	 * Returns the meta object for the reference '{@link com.xkp.codexcess.model.XSSInstance#getXssclass <em>Xssclass</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the reference '<em>Xssclass</em>'.
	 * @see com.xkp.codexcess.model.XSSInstance#getXssclass()
	 * @see #getXSSInstance()
	 * @generated
	 */
	EReference getXSSInstance_Xssclass();

	/**
	 * Returns the meta object for the containment reference list '{@link com.xkp.codexcess.model.XSSInstance#getChilds <em>Childs</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Childs</em>'.
	 * @see com.xkp.codexcess.model.XSSInstance#getChilds()
	 * @see #getXSSInstance()
	 * @generated
	 */
	EReference getXSSInstance_Childs();

	/**
	 * Returns the meta object for the containment reference '{@link com.xkp.codexcess.model.XSSInstance#getProperties <em>Properties</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Properties</em>'.
	 * @see com.xkp.codexcess.model.XSSInstance#getProperties()
	 * @see #getXSSInstance()
	 * @generated
	 */
	EReference getXSSInstance_Properties();

	/**
	 * Returns the meta object for the reference '{@link com.xkp.codexcess.model.XSSInstance#getParent <em>Parent</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the reference '<em>Parent</em>'.
	 * @see com.xkp.codexcess.model.XSSInstance#getParent()
	 * @see #getXSSInstance()
	 * @generated
	 */
	EReference getXSSInstance_Parent();

	/**
	 * Returns the meta object for class '{@link com.xkp.codexcess.model.XSSMethod <em>XSS Method</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>XSS Method</em>'.
	 * @see com.xkp.codexcess.model.XSSMethod
	 * @generated
	 */
	EClass getXSSMethod();

	/**
	 * Returns the meta object for the attribute '{@link com.xkp.codexcess.model.XSSMethod#getId <em>Id</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Id</em>'.
	 * @see com.xkp.codexcess.model.XSSMethod#getId()
	 * @see #getXSSMethod()
	 * @generated
	 */
	EAttribute getXSSMethod_Id();

	/**
	 * Returns the meta object for the attribute '{@link com.xkp.codexcess.model.XSSMethod#getType <em>Type</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Type</em>'.
	 * @see com.xkp.codexcess.model.XSSMethod#getType()
	 * @see #getXSSMethod()
	 * @generated
	 */
	EAttribute getXSSMethod_Type();

	/**
	 * Returns the meta object for class '{@link com.xkp.codexcess.model.XSSProject <em>XSS Project</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>XSS Project</em>'.
	 * @see com.xkp.codexcess.model.XSSProject
	 * @generated
	 */
	EClass getXSSProject();

	/**
	 * Returns the meta object for the attribute '{@link com.xkp.codexcess.model.XSSProject#getName <em>Name</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Name</em>'.
	 * @see com.xkp.codexcess.model.XSSProject#getName()
	 * @see #getXSSProject()
	 * @generated
	 */
	EAttribute getXSSProject_Name();

	/**
	 * Returns the meta object for the containment reference list '{@link com.xkp.codexcess.model.XSSProject#getXssidioms <em>Xssidioms</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Xssidioms</em>'.
	 * @see com.xkp.codexcess.model.XSSProject#getXssidioms()
	 * @see #getXSSProject()
	 * @generated
	 */
	EReference getXSSProject_Xssidioms();

	/**
	 * Returns the meta object for the containment reference list '{@link com.xkp.codexcess.model.XSSProject#getXssclasses <em>Xssclasses</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Xssclasses</em>'.
	 * @see com.xkp.codexcess.model.XSSProject#getXssclasses()
	 * @see #getXSSProject()
	 * @generated
	 */
	EReference getXSSProject_Xssclasses();

	/**
	 * Returns the meta object for the reference '{@link com.xkp.codexcess.model.XSSProject#getApplication <em>Application</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the reference '<em>Application</em>'.
	 * @see com.xkp.codexcess.model.XSSProject#getApplication()
	 * @see #getXSSProject()
	 * @generated
	 */
	EReference getXSSProject_Application();

	/**
	 * Returns the meta object for class '{@link com.xkp.codexcess.model.XSSProperty <em>XSS Property</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>XSS Property</em>'.
	 * @see com.xkp.codexcess.model.XSSProperty
	 * @generated
	 */
	EClass getXSSProperty();

	/**
	 * Returns the meta object for the attribute '{@link com.xkp.codexcess.model.XSSProperty#getId <em>Id</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Id</em>'.
	 * @see com.xkp.codexcess.model.XSSProperty#getId()
	 * @see #getXSSProperty()
	 * @generated
	 */
	EAttribute getXSSProperty_Id();

	/**
	 * Returns the meta object for the attribute '{@link com.xkp.codexcess.model.XSSProperty#getType <em>Type</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Type</em>'.
	 * @see com.xkp.codexcess.model.XSSProperty#getType()
	 * @see #getXSSProperty()
	 * @generated
	 */
	EAttribute getXSSProperty_Type();

	/**
	 * Returns the meta object for class '{@link java.util.Map <em>Map</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Map</em>'.
	 * @see java.util.Map
	 * @model instanceClass="java.util.Map" typeParameters="T T1"
	 * @generated
	 */
	EClass getMap();

	/**
	 * Returns the factory that creates the instances of the model.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the factory that creates the instances of the model.
	 * @generated
	 */
	ModelFactory getModelFactory();

	/**
	 * <!-- begin-user-doc -->
	 * Defines literals for the meta objects that represent
	 * <ul>
	 *   <li>each class,</li>
	 *   <li>each feature of each class,</li>
	 *   <li>each enum,</li>
	 *   <li>and each data type</li>
	 * </ul>
	 * <!-- end-user-doc -->
	 * @generated
	 */
	interface Literals {
		/**
		 * The meta object literal for the '{@link com.xkp.codexcess.model.impl.XSSClassImpl <em>XSS Class</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see com.xkp.codexcess.model.impl.XSSClassImpl
		 * @see com.xkp.codexcess.model.impl.ModelPackageImpl#getXSSClass()
		 * @generated
		 */
		EClass XSS_CLASS = eINSTANCE.getXSSClass();

		/**
		 * The meta object literal for the '<em><b>Id</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute XSS_CLASS__ID = eINSTANCE.getXSSClass_Id();

		/**
		 * The meta object literal for the '<em><b>Xssimports</b></em>' reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference XSS_CLASS__XSSIMPORTS = eINSTANCE.getXSSClass_Xssimports();

		/**
		 * The meta object literal for the '<em><b>Xssmethods</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference XSS_CLASS__XSSMETHODS = eINSTANCE.getXSSClass_Xssmethods();

		/**
		 * The meta object literal for the '<em><b>Xssevents</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference XSS_CLASS__XSSEVENTS = eINSTANCE.getXSSClass_Xssevents();

		/**
		 * The meta object literal for the '<em><b>Xssproperties</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference XSS_CLASS__XSSPROPERTIES = eINSTANCE.getXSSClass_Xssproperties();

		/**
		 * The meta object literal for the '{@link com.xkp.codexcess.model.impl.XSSEventImpl <em>XSS Event</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see com.xkp.codexcess.model.impl.XSSEventImpl
		 * @see com.xkp.codexcess.model.impl.ModelPackageImpl#getXSSEvent()
		 * @generated
		 */
		EClass XSS_EVENT = eINSTANCE.getXSSEvent();

		/**
		 * The meta object literal for the '<em><b>Id</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute XSS_EVENT__ID = eINSTANCE.getXSSEvent_Id();

		/**
		 * The meta object literal for the '{@link com.xkp.codexcess.model.impl.XSSIdiomImpl <em>XSS Idiom</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see com.xkp.codexcess.model.impl.XSSIdiomImpl
		 * @see com.xkp.codexcess.model.impl.ModelPackageImpl#getXSSIdiom()
		 * @generated
		 */
		EClass XSS_IDIOM = eINSTANCE.getXSSIdiom();

		/**
		 * The meta object literal for the '<em><b>Id</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute XSS_IDIOM__ID = eINSTANCE.getXSSIdiom_Id();

		/**
		 * The meta object literal for the '<em><b>Xssclasses</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference XSS_IDIOM__XSSCLASSES = eINSTANCE.getXSSIdiom_Xssclasses();

		/**
		 * The meta object literal for the '{@link com.xkp.codexcess.model.impl.XSSImportImpl <em>XSS Import</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see com.xkp.codexcess.model.impl.XSSImportImpl
		 * @see com.xkp.codexcess.model.impl.ModelPackageImpl#getXSSImport()
		 * @generated
		 */
		EClass XSS_IMPORT = eINSTANCE.getXSSImport();

		/**
		 * The meta object literal for the '<em><b>Xssclass</b></em>' reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference XSS_IMPORT__XSSCLASS = eINSTANCE.getXSSImport_Xssclass();

		/**
		 * The meta object literal for the '{@link com.xkp.codexcess.model.impl.XSSInstanceImpl <em>XSS Instance</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see com.xkp.codexcess.model.impl.XSSInstanceImpl
		 * @see com.xkp.codexcess.model.impl.ModelPackageImpl#getXSSInstance()
		 * @generated
		 */
		EClass XSS_INSTANCE = eINSTANCE.getXSSInstance();

		/**
		 * The meta object literal for the '<em><b>Id</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute XSS_INSTANCE__ID = eINSTANCE.getXSSInstance_Id();

		/**
		 * The meta object literal for the '<em><b>Xssclass</b></em>' reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference XSS_INSTANCE__XSSCLASS = eINSTANCE.getXSSInstance_Xssclass();

		/**
		 * The meta object literal for the '<em><b>Childs</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference XSS_INSTANCE__CHILDS = eINSTANCE.getXSSInstance_Childs();

		/**
		 * The meta object literal for the '<em><b>Properties</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference XSS_INSTANCE__PROPERTIES = eINSTANCE.getXSSInstance_Properties();

		/**
		 * The meta object literal for the '<em><b>Parent</b></em>' reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference XSS_INSTANCE__PARENT = eINSTANCE.getXSSInstance_Parent();

		/**
		 * The meta object literal for the '{@link com.xkp.codexcess.model.impl.XSSMethodImpl <em>XSS Method</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see com.xkp.codexcess.model.impl.XSSMethodImpl
		 * @see com.xkp.codexcess.model.impl.ModelPackageImpl#getXSSMethod()
		 * @generated
		 */
		EClass XSS_METHOD = eINSTANCE.getXSSMethod();

		/**
		 * The meta object literal for the '<em><b>Id</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute XSS_METHOD__ID = eINSTANCE.getXSSMethod_Id();

		/**
		 * The meta object literal for the '<em><b>Type</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute XSS_METHOD__TYPE = eINSTANCE.getXSSMethod_Type();

		/**
		 * The meta object literal for the '{@link com.xkp.codexcess.model.impl.XSSProjectImpl <em>XSS Project</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see com.xkp.codexcess.model.impl.XSSProjectImpl
		 * @see com.xkp.codexcess.model.impl.ModelPackageImpl#getXSSProject()
		 * @generated
		 */
		EClass XSS_PROJECT = eINSTANCE.getXSSProject();

		/**
		 * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute XSS_PROJECT__NAME = eINSTANCE.getXSSProject_Name();

		/**
		 * The meta object literal for the '<em><b>Xssidioms</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference XSS_PROJECT__XSSIDIOMS = eINSTANCE.getXSSProject_Xssidioms();

		/**
		 * The meta object literal for the '<em><b>Xssclasses</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference XSS_PROJECT__XSSCLASSES = eINSTANCE.getXSSProject_Xssclasses();

		/**
		 * The meta object literal for the '<em><b>Application</b></em>' reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference XSS_PROJECT__APPLICATION = eINSTANCE.getXSSProject_Application();

		/**
		 * The meta object literal for the '{@link com.xkp.codexcess.model.impl.XSSPropertyImpl <em>XSS Property</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see com.xkp.codexcess.model.impl.XSSPropertyImpl
		 * @see com.xkp.codexcess.model.impl.ModelPackageImpl#getXSSProperty()
		 * @generated
		 */
		EClass XSS_PROPERTY = eINSTANCE.getXSSProperty();

		/**
		 * The meta object literal for the '<em><b>Id</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute XSS_PROPERTY__ID = eINSTANCE.getXSSProperty_Id();

		/**
		 * The meta object literal for the '<em><b>Type</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute XSS_PROPERTY__TYPE = eINSTANCE.getXSSProperty_Type();

		/**
		 * The meta object literal for the '{@link java.util.Map <em>Map</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see java.util.Map
		 * @see com.xkp.codexcess.model.impl.ModelPackageImpl#getMap()
		 * @generated
		 */
		EClass MAP = eINSTANCE.getMap();

	}

} //ModelPackage
