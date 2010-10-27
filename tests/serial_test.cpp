#define BOOST_TEST_MODULE xs
#include <boost/test/unit_test.hpp>

#include <schema.h>
#include <serial.h>
#include <node.h>

#include <iostream>
#include <fstream>
#include <archive/json_archive.h>
#include <archive/xml_archive.h>

using namespace xkp;

struct serializable
  {
    serializable() : ii(0), ff(0)                             {}
    serializable(int i, float f, str s) : ii(i), ff(f), ss(s) {}
  
    int   ii;
    float ff;
    str   ss;
  };
  
struct serializable_schema : object_schema<serializable>  
  {
    serializable_schema()
      {
        property_("ii", &serializable::ii);
        property_("ff", &serializable::ff);
        property_("ss", &serializable::ss);
      }
  };
  
register_complete_type(serializable, serializable_schema);

BOOST_AUTO_TEST_CASE( testing_objects )
{
  serializable obj;
  obj.ii = 53;
  obj.ff = 3.1415f;
  obj.ss = "hello";

  json_write_archive ar;
  ar.save(obj); 
  
  xml_write_archive xml_w;
  xml_w.save(obj);

  json_read_archive r_ar(ar.result());
  serializable obj_from_json = r_ar.get( type_schema<serializable>() );

  xml_read_archive xml_r(xml_w.result());
  serializable obj_from_xml = xml_r.get( type_schema<serializable>() );

  BOOST_CHECK(obj_from_json.ii == obj.ii); 
  BOOST_CHECK(obj_from_json.ff == obj.ff); 
  BOOST_CHECK(obj_from_json.ss == obj.ss); 

  BOOST_CHECK(obj_from_json.ii == obj_from_xml.ii); 
  BOOST_CHECK(obj_from_json.ff == obj_from_xml.ff); 
  BOOST_CHECK(obj_from_json.ss == obj_from_xml.ss); 
}

register_iterator(serializable);

BOOST_AUTO_TEST_CASE( testing_vectors )
{
  std::vector<serializable> vec;
  vec.push_back(serializable(10, 23,    "yo"));
  vec.push_back(serializable(20, 23.4f, "tu"));
  vec.push_back(serializable(30, 25.7f, "el"));

  json_write_archive ar;
  ar.save(vec);

  xml_write_archive xml_w;
  xml_w.save(vec);

  json_read_archive r_ar(ar.result());
  std::vector<serializable> vec_from_json = r_ar.get( type_schema< std::vector<serializable> >() );

  xml_read_archive xml_r(xml_w.result());
  std::vector<serializable> vec_from_xml = xml_r.get( type_schema< std::vector<serializable> >() );

  BOOST_CHECK(vec_from_json.size() == 3); 
  BOOST_CHECK(vec_from_json[0].ii  == 10); 
  BOOST_CHECK(vec_from_json[1].ff  == vec[1].ff); 
  BOOST_CHECK(vec_from_json[2].ss  == "el"); 
  
  BOOST_CHECK(vec_from_json.size() == vec_from_xml.size()); 
  BOOST_CHECK(vec_from_json[0].ii  == vec_from_xml[0].ii); 
  BOOST_CHECK(vec_from_json[1].ff  == vec_from_xml[1].ff); 
  BOOST_CHECK(vec_from_json[2].ss  == vec_from_xml[2].ss); 
}

class Node1 : public base_node, typed_object<Node1>
  {
    public:
      Node1() {}             
      Node1(const str& f) : field(f) {}
    public:
      str field;
  };

class Node2 : public base_node, typed_object<Node2>
  {
    public:
      Node2()      : field(0) {}             
      Node2(int f) : field(f) {}
    public:
      int field;
  };
  
struct Node1Schema : base_node_schema<Node1>
  {
    Node1Schema()
      {
        property_("field", &Node1::field);
      }
  };

struct Node2Schema : base_node_schema<Node2>
  {
    Node2Schema()
      {
        property_("field", &Node2::field);
      }
  };
  
register_complete_type(Node1, Node1Schema);
register_complete_type(Node2, Node2Schema);

BOOST_AUTO_TEST_CASE( testing_nodes )
{
  Node root(new Node1("Hello"));
  root->insert(Node( new Node1("First Child")));
  
  Node n2( new Node2(23) );
  
  root->insert(n2);
  
  n2->insert(Node( new Node1("Second Child")));
  
  type_registry types;
  types.add_type("node1", type_schema<Node1>());
  types.add_type("node2", type_schema<Node2>());
  
  json_write_archive ar(&types); 
  ar.save(root); 
  
  xml_write_archive xml_w(&types); 
  xml_w.save(root);

  json_read_archive r_ar(ar.result(), &types);
  Node loaded_node = r_ar.get( type_schema<Node>() );

  xml_read_archive xml_r(xml_w.result(), &types);
  Node node_from_xml = xml_r.get( type_schema<Node>() );

}

BOOST_AUTO_TEST_CASE( finished )
{
  //for some reason the test frameworks exits without letting me see the goods
  std::cin.get();
}