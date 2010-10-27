
#include <node.h>

using namespace xkp;

//base_node
base_node::base_node():
  parent_(null)
  {
  }
  
void base_node::traverse(traverser* t)
  {
    std::vector<Node>::iterator it = nodes_.begin();
    std::vector<Node>::iterator nd = nodes_.end();
    
    for(; it != nd; it++)
      {
        (*it)->traverse( t );
      }
  }
  
void base_node::visit(node_visitor* v)
  {
    std::vector<Node>::iterator it = nodes_.begin();
    std::vector<Node>::iterator nd = nodes_.end();
    
    for(; it != nd; it++)
      {
        v->visit( *it );
      }
  }
  
INode* base_node::parent()
  {
    return parent_;
  }
  
void base_node::insert(Node n)
  {
    nodes_.push_back( n );
  }

