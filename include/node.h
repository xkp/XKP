
#ifndef XKP_NODE_H
#define XKP_NODE_H

#include "types.h"
#include "schema.h"
#include <vector>

namespace xkp
{
  //forwards
  class   INode;
  typedef reference<INode> Node;

  //interface
  struct traverser
    { 
      int id;
    };

  struct node_visitor
    {
      virtual void visit(Node& node) = 0;
    };

  class INode : virtual public ITypedObject
    {
      public:
        virtual void   traverse(traverser* t) = 0;
        virtual void   visit(node_visitor* v) = 0;
        virtual INode* parent()               = 0;
        virtual void   insert(Node n)         = 0;
    };
    
  struct node_schema : interface_schema<INode>
    {
      node_schema()
        {
          implements<ITypedObject>();
        }
    };
    
  register_interface_type(INode, node_schema);
  register_iterator(Node);
    
  //engine
  class base_node : public INode
    {
      public:
        typedef std::vector<Node> container;

      public:
        base_node();
      public:
        //INode
        virtual void   traverse(traverser* t);
        virtual void   visit(node_visitor* v);
        virtual INode* parent();
        virtual void   insert(Node n);
      public: 
        container nodes_;
        INode*    parent_;    
        //container* children() {return &nodes_;};
    };
    
  template <typename T>
  struct base_node_schema : object_schema<T>
    {
      base_node_schema()
        {
          implements<INode>();
          static_field("children", &T::nodes_);
        }
    };
}

#endif