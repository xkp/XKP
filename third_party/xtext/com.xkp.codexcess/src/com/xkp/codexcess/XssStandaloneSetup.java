
package com.xkp.codexcess;

/**
 * Initialization support for running Xtext languages 
 * without equinox extension registry
 */
public class XssStandaloneSetup extends XssStandaloneSetupGenerated{

	public static void doSetup() {
		new XssStandaloneSetup().createInjectorAndDoEMFRegistration();
	}
}

