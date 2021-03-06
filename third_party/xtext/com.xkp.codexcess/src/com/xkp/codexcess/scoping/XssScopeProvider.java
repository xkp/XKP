/*
 * generated by Xtext
 */
package com.xkp.codexcess.scoping;

import java.util.Collection;
import java.util.LinkedList;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EReference;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.ecore.resource.ResourceSet;
import org.eclipse.xtext.resource.IContainer;
import org.eclipse.xtext.resource.IResourceDescription;
import org.eclipse.xtext.resource.IResourceDescriptions;
import org.eclipse.xtext.resource.impl.ResourceDescriptionsProvider;
import org.eclipse.xtext.scoping.IScope;
import org.eclipse.xtext.scoping.Scopes;
import org.eclipse.xtext.scoping.impl.AbstractDeclarativeScopeProvider;

import com.google.inject.Inject;
import com.google.inject.Provider;
import com.xkp.codexcess.xss.Expr;
import com.xkp.codexcess.xss.Statement;

/**
 * This class contains custom scoping description.
 * 
 * see : http://www.eclipse.org/Xtext/documentation/latest/xtext.html#scoping
 * on how and when to use it 
 *
 */
public class XssScopeProvider extends AbstractDeclarativeScopeProvider {
	
	@Inject
	private Provider<ResourceSet> rsp;
	  
	@Inject
	private ResourceDescriptionsProvider rdp;
	
	@Inject
    IContainer.Manager manager;
     
    public void listVisibleResources(Resource myResource, IResourceDescriptions index) {
    	IResourceDescription descr = index.getResourceDescription(myResource.getURI());
    	for(IContainer visibleContainer : manager.getVisibleContainers(descr, index)) {
    		for(IResourceDescription visibleResourceDesc : visibleContainer.getResourceDescriptions()) {
    			System.out.println(visibleResourceDesc.getURI());
    		}
    	}
    }
    
	@Override
	public IScope getScope(EObject context, EReference reference) {
		
		if (context instanceof Expr) {
			Expr varDecl = (Expr) context;
			Statement statement = (Statement) varDecl.eContainer();
			return getLocalVarDecl(statement, new LinkedList<Statement>());
		}
		
    	System.out.println("scope_" + reference.getEContainingClass().getName() + 
    			"_" + reference.getName() + "(" + context.eClass().getName() + ", ...)");
    	
		final ResourceSet rs = this.rsp.get();
		final IResourceDescriptions index = this.rdp.getResourceDescriptions(rs);
		
		listVisibleResources(context.eResource(), index);
		
		
		//return super.getScope(context, reference);
		return IScope.NULLSCOPE;
	}

	private IScope getLocalVarDecl(Statement statement, Collection<Statement> visited) {
		if (statement == null || visited.contains(statement))
			return IScope.NULLSCOPE;
		visited.add(statement);
		//IScope parentScope = getLocalVarDecl(statement.getStmt(), visited);
		//return Scopes.scopeFor((Iterable<? extends EObject>) statement.getVarDecl(), parentScope);
		return Scopes.scopeFor((Iterable<? extends EObject>) statement.getVarDecl(), IScope.NULLSCOPE);
	}
}
