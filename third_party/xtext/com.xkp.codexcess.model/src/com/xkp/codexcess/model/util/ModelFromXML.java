package com.xkp.codexcess.model.util;

import java.io.File;
import java.io.IOException;
import java.util.Iterator;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.eclipse.emf.common.util.EList;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

import com.xkp.codexcess.model.XSSClass;
import com.xkp.codexcess.model.XSSIProperty;
import com.xkp.codexcess.model.XSSIdiom;
import com.xkp.codexcess.model.XSSInstance;
import com.xkp.codexcess.model.XSSProject;
import com.xkp.codexcess.model.XSSProperty;
import com.xkp.codexcess.model.impl.ModelFactoryImpl;

public class ModelFromXML {
	
	private String projectPath;
	private String projectFile;
	private String projectFileName;
	private String projectName;
	private String projectTarget;
	
	private ModelFactoryImpl factory;
	private XSSProject xssProject;
	
	public ModelFromXML(String projectPath, String projectFile) {
		
		factory = new ModelFactoryImpl();
		
		this.projectPath = projectPath;
		this.projectFile = projectFile;
		
		projectFileName = projectPath + "/" + projectFile;
		
		readXMLProject(projectFileName);
	}
	
	public void readXMLProject(String src) {
		
		if (src == null || src.equals(""))
			src = projectFileName;
		
		File fXMLFile = new File(src);

		DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
		DocumentBuilder dBuilder;
		Document xmlDoc = null;
		
		try {
			dBuilder = dbFactory.newDocumentBuilder();
			xmlDoc = dBuilder.parse(fXMLFile);
		} catch (ParserConfigurationException e) {
			e.printStackTrace();
		} catch (SAXException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		xmlDoc.getDocumentElement().normalize();
		
		Element projectNode = xmlDoc.getDocumentElement();
		
		xssProject = factory.createXSSProject();
		xssProject.setName(projectName);
		
		this.projectTarget = projectNode.getAttribute("target");
		
		NodeList nodeListIdioms = xmlDoc.getElementsByTagName("idiom");
		for (int i = 0; i < nodeListIdioms.getLength(); i++) {
			
			Node nodeIdiom = nodeListIdioms.item(i);
			if (nodeIdiom.getNodeType() == Node.ELEMENT_NODE) {
				
				Element eIdiom = (Element) nodeIdiom;
				String sourceIdiom = eIdiom.getAttribute("src");
				
				readXMLIdiom(projectPath + "/" + sourceIdiom);
			}
		}
		
		readXMLApplication(projectPath + "/" + projectTarget);
	}
	
	public void readXMLApplication(String src) {
		
		File fXMLFile = new File(src);

		DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
		DocumentBuilder dBuilder;
		Document xmlDoc = null;
		
		try {
			dBuilder = dbFactory.newDocumentBuilder();
			xmlDoc = dBuilder.parse(fXMLFile);
		} catch (ParserConfigurationException e) {
			e.printStackTrace();
		} catch (SAXException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		xmlDoc.getDocumentElement().normalize();
		
		Element applicationElement = xmlDoc.getDocumentElement();
		
		XSSInstance xssApplication = readInstance(applicationElement, null);
		
		String applicationID = applicationElement.getAttribute("id");
		String applicationName = applicationElement.getAttribute("name");

		// tricky to id or name attribute of application instance
		xssApplication.setId(applicationID.equals("") ? applicationName : applicationID);
		
		xssProject.setApplication(xssApplication);
	}
	
	private XSSInstance readInstance(Element ele, XSSInstance instance) {

		String instanceType = ele.getNodeName();
		String instanceID = ele.getAttribute("id");
		
		XSSInstance xssInstance = factory.createXSSInstance();
		xssInstance.setId(instanceID);
		xssInstance.setXssclass(getXSSClassByID(xssProject, instanceType));
		xssInstance.setParent(instance);
		
		// read attributes and create properties
		NamedNodeMap nodeAttributes = ele.getAttributes();
		for (int j = 0; j < nodeAttributes.getLength(); j++) {
			
			Node nodeProperty = nodeAttributes.item(j);
			if (nodeProperty.getNodeType() == Node.ATTRIBUTE_NODE) {
				String propName = nodeProperty.getNodeName();
				String propValue = nodeProperty.getNodeValue();
				
				XSSIProperty instProp = factory.createXSSIProperty();
				instProp.setId(propName);
				instProp.setValue(propValue);
				
				xssInstance.getProperties().add(instProp);
			}
		}
		
		// iterate over all childs recursively
		NodeList nodeListInstances = ele.getChildNodes();
		for (int i = 0; i < nodeListInstances.getLength(); i++) {
			
			Node nodeInstance = nodeListInstances.item(i);
			if (nodeInstance.getNodeType() == Node.ELEMENT_NODE) {
				Element eInstance = (Element) nodeInstance;
				
				XSSInstance child = readInstance(eInstance, xssInstance);
				xssInstance.getChilds().add(child);
			}
		}
		
		return xssInstance;
	}
	
	public void readXMLIdiom(String src) {
		
		File fXMLFile = new File(src);

		DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
		DocumentBuilder dBuilder;
		Document xmlDoc = null;
		
		try {
			dBuilder = dbFactory.newDocumentBuilder();
			xmlDoc = dBuilder.parse(fXMLFile);
		} catch (ParserConfigurationException e) {
			e.printStackTrace();
		} catch (SAXException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		xmlDoc.getDocumentElement().normalize();
		
		Element idiomElement = xmlDoc.getDocumentElement();
		
		String idiomID = idiomElement.getAttribute("id");
		
		XSSIdiom xssIdiom = factory.createXSSIdiom();
		xssIdiom.setId(idiomID);
		xssProject.getXssidioms().add(xssIdiom);
		
		// iterate over all idiom class
		NodeList nodeListClasses = idiomElement.getElementsByTagName("class");
		for (int i = 0; i < nodeListClasses.getLength(); i++) {
			
			Node nodeClass = nodeListClasses.item(i);
			if (nodeClass.getNodeType() == Node.ELEMENT_NODE) {
				Element eClass = (Element) nodeClass;
				
				String classID = eClass.getAttribute("id");
				String classSuper = eClass.getAttribute("super");
				
				XSSClass xssClass = factory.createXSSClass();
				xssClass.setId(classID);
				xssClass.setSuper(getXSSClassByID(xssIdiom, classSuper));
				
				xssIdiom.getXssclasses().add(xssClass);
				
				loadProperties(eClass, xssClass);
				loadEvents(eClass, xssClass);
				loadMethods(eClass, xssClass);
			}
		}
		
		// iterate over all idiom enums
		// TODO: customize class to enum with properties
		NodeList nodeListEnums = idiomElement.getElementsByTagName("enum");
		for (int i = 0; i < nodeListEnums.getLength(); i++) {
			
			Node nodeEnum = nodeListEnums.item(i);
			if (nodeEnum.getNodeType() == Node.ELEMENT_NODE) {
				Element eEnum = (Element) nodeEnum;
				
				String classID = eEnum.getAttribute("id");
				String classSuper = eEnum.getAttribute("super");
				
				XSSClass xssEnum = factory.createXSSClass();
				xssEnum.setId(classID);
				xssEnum.setSuper(getXSSClassByID(xssIdiom, classSuper));
				
				xssIdiom.getXssclasses().add(xssEnum);
				
				loadProperties(eEnum, xssEnum);
			}
		}
	}

	private void loadProperties(Element ele, XSSClass xssClass) {

		NodeList nodeListProperties = ele.getElementsByTagName("property");
		for (int i = 0; i < nodeListProperties.getLength(); i++) {
			
			Node nodeProperty = nodeListProperties.item(i);
			if (nodeProperty.getNodeType() == Node.ELEMENT_NODE) {
				Element eProp = (Element) nodeProperty;
				
				String propertyID = eProp.getAttribute("id");
				String propertyType = eProp.getAttribute("type");
				
				XSSProperty xssProperty = factory.createXSSProperty();
				xssProperty.setId(propertyID);
				xssProperty.setType(propertyType);
				
				xssClass.getXssproperties().add(xssProperty);
			}
		}
	}
	
	private void loadEvents(Element ele, XSSClass xssClass) {
		// TODO:
	}
	
	private void loadMethods(Element ele, XSSClass xssClass) {
		// TODO:
	}
	
	public void setProjectPath(String projectPath) {
		this.projectPath = projectPath;
		projectFileName = projectPath + "/" + projectFile;
	}
	
	public void setProjectFile(String projectFile) {
		this.projectFile = projectFile;
		projectFileName = projectPath + "/" + projectFile;
	}

	public XSSProject getProject() {
		return xssProject;
	}
	
	public EList<XSSIdiom> getIdioms() {
		return xssProject.getXssidioms();
	}
	
	public XSSInstance getApplication() {
		return xssProject.getApplication();
	}
	
	public static XSSClass getXSSClassByID(XSSProject project, String id) {
		if (project == null || id == null || id.length() == 0)
			return null;
		
		Iterator<XSSIdiom> itrIdioms = project.getXssidioms().iterator();
		while (itrIdioms.hasNext()) {
			XSSIdiom xssIdiom = (XSSIdiom) itrIdioms.next();
			
			Iterator<XSSClass> itrClass = xssIdiom.getXssclasses().iterator();
			while (itrClass.hasNext()) {
				XSSClass xssClass = (XSSClass) itrClass.next();
				if (xssClass.getId().equals(id)) {
					return xssClass;
				}
			}
		}
		
		return null;
	}
	
	public static XSSClass getXSSClassByID(XSSIdiom idiom, String id) {
		if (idiom == null || id == null || id.length() == 0)
			return null;
		
		Iterator<XSSClass> itr = idiom.getXssclasses().iterator();
		while (itr.hasNext()) {
			XSSClass xssClass = (XSSClass) itr.next();
			if (xssClass.getId().equals(id)) {
				return xssClass;
			}
		}
		
		return null;
	}
}
