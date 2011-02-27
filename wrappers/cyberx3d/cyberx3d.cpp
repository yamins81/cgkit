/*
  Main wrapper file.
 */

#include <boost/python.hpp>
#include <iostream>
#include <sstream>
#include <string>

#include "common_exceptions.h"
#include "vec3.h"
#include "vec4.h"
#include "mat3.h"
#include "mat4.h"
#include "worldobject.h"
#include "glpointlight.h"
#include "glspotlight.h"
#include "gldistantlight.h"
#include "boxgeom.h"
#include "trimeshgeom.h"

#include <cybergarage/x3d/CyberX3D.h>

//#include <trimeshgeom.h>
//#include <slot.h>
//#include "common_funcs.h"

using namespace boost::python;

//////////////////////////////////////////////////////////////////////

class X3DReader
{
  private:
  // Globals dictionary for Python calls
  PyObject* globals;
  // Current transformation
  support3d::mat4d transform;
  // Current name
  std::string name;
  // Current material
  object material;
  // Current parent WorldObject
  object parent;

  // Debug flag (true=output messages)
  bool debug;

  public:
  X3DReader(bool adebug=false) 
    : globals(0), transform(1.0), name("object"), material(), parent(),
      debug(adebug)
  {
    PyObject* m = PyImport_AddModule("cgkit.x3dimport");
    if (m==NULL)
      throw support3d::EKeyError("Module cgkit.x3dimport not available");
    globals = PyModule_GetDict(m);    
  }

  /**
    Import a X3D or VRML file.
   */
  void read(const char* filename)
  {
    if (debug) std::cerr<<"X3DReader::read(\""<<filename<<"\")"<<std::endl;
    if (debug) std::cerr<<"  Allocating scene graph..."<<std::flush;
    CyberX3D::SceneGraph* scngraph = new CyberX3D::SceneGraph();
    if (scngraph==0)
    {
      std::cout<<"No memory for the scene graph available"<<std::endl;
      return;
    }

    if (debug) std::cerr<<"OK\n  Loading file..."<<std::flush;
    if (scngraph->load(filename))
    {
      if (debug) std::cerr<<"OK\n  Processing scene graph"<<std::endl;
      CyberX3D::Node* node;
      for(node=scngraph->getNodes(); node!=0; node=node->next())
      {
	if (debug) std::cerr<<"  Node '"<<node->getName()<<"'"<<std::endl;
	processNode(node);
      }
    }
    else
    {
      std::cout<<"VRML/X3D error:"<<std::endl;
      std::cout<<scngraph->getParserErrorMessage()<<" (line ";
      std::cout<<scngraph->getParserErrorLineNumber()<<"): ";
      std::cout<<scngraph->getParserErrorToken()<<std::endl;
    }

    if (debug) std::cerr<<"  Deleting scene graph"<<std::endl;
    delete scngraph;
    if (debug) std::cerr<<"X3DReader::read() - end"<<std::endl;
  }

  /**
     Process a scene node.
   */
  void processNode(CyberX3D::Node* node)
  {
    if (node==0)
      return;

    int type = node->getType();
    // WorldInfo? (-> ignore)
    if (type==CyberX3D::WORLDINFO_NODE)
      { }
    // NavigationInfo? (-> ignore)
    else if (type==CyberX3D::NAVIGATIONINFO_NODE)
      { }
    // Group?
    else if (type==CyberX3D::GROUP_NODE)
      onGroup((CyberX3D::GroupNode*)node);
    // Transform?
    else if (type==CyberX3D::TRANSFORM_NODE)
      onTransform((CyberX3D::TransformNode*)node);
    // Shape?
    else if (type==CyberX3D::SHAPE_NODE)
      onShape((CyberX3D::ShapeNode*)node);
    // Viewpoint?
    else if (type==CyberX3D::VIEWPOINT_NODE)
      onViewPoint((CyberX3D::ViewpointNode*)node);
    // Box?
    else if (type==CyberX3D::BOX_NODE)
      onBox((CyberX3D::BoxNode*)node);
    // Sphere?
    else if (type==CyberX3D::SPHERE_NODE)
      onSphere((CyberX3D::SphereNode*)node);
    // Cylinder?
    else if (type==CyberX3D::CYLINDER_NODE)
      onCylinder((CyberX3D::CylinderNode*)node);
    // Cone?
    else if (type==CyberX3D::CONE_NODE)
      onCone((CyberX3D::ConeNode*)node);
    // Indexed face set?
    else if (type==CyberX3D::INDEXEDFACESET_NODE)
      onIndexedFaceSet((CyberX3D::IndexedFaceSetNode*)node);
    // Point light?
    else if (type==CyberX3D::POINTLIGHT_NODE)
      onPointLight((CyberX3D::PointLightNode*)node);
    // Spot light?
    else if (type==CyberX3D::SPOTLIGHT_NODE)
      onSpotLight((CyberX3D::SpotLightNode*)node);
    // Directional light?
    else if (type==CyberX3D::DIRECTIONALLIGHT_NODE)
      onDirectionalLight((CyberX3D::DirectionalLightNode*)node);
    else
    {
      //     std::cout<<"Name: '"<<node->getName()<<"' - Type: ";
      //     std::cout<<node->getTypeString()<<" ("<<node->getType()<<")"<<std::endl;
    }

    // Iterate over the children...
    //    visitNode(node->getChildNodes());
    // Go to the next sibling...
    //    visitNode(node->next());
  }

  /**
     Execute Python code.
   */
  bool exec(std::string s, dict& locals)
  {
    PyObject* pyobj = PyRun_String(s.c_str(), Py_single_input, globals, locals.ptr());
    if (pyobj==NULL)
    {
      PyErr_Print();
      return false;
    }
    Py_DECREF(pyobj);
    return true;
  }

  /**
     Return the inverse world transform of world object obj.

     obj must be a Python object that either contains None or a
     WorldObject.
   */
  support3d::mat4d getInvWorldTransform(object obj)
  {
    if (obj==object())
    {
      return support3d::mat4d(1.0);
    }
    else
    {
      support3d::WorldObject& w = extract<support3d::WorldObject&>(obj)();
      try
      {
	return w.worldtransform.getValue().inverse();
      }
      catch(support3d::EZeroDivisionError&)
      {
	std::cerr<<"Warning: non invertible transformation"<<std::endl;
	return support3d::mat4d(1.0);
      }
    }
  }

  /////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////
  // Group
  //////////////////////////////////////////////////////////////////////
  void onGroup(CyberX3D::GroupNode* grp)
  {
    //    std::cout<<"GROUP ("<<grp->getName()<<")"<<std::endl;
    //    std::cout<<"  #Childs: "<<grp->getNChildNodes()<<std::endl;

    // Push global state variables...
    std::string prev_name = name;
    object prev_parent = parent;
    name = grp->getName();
    if (name=="")
      name = prev_name;

    // Create Group object
    dict locals;
    locals["mat"] = material;
    locals["parent"] = parent;
    if (!exec("g=Group(name='"+name+"', material=mat, parent=parent)", locals))
      return;

    // obj: The WorldObject
    support3d::WorldObject& obj = extract<support3d::WorldObject&>(locals["g"])();
    obj.transform.setValue(getInvWorldTransform(parent)*transform);
    
    parent = locals["g"];

    // Process the children...
    for(CyberX3D::Node* n=grp->getChildNodes(); n!=0; n=n->next())
    {
      processNode(n);
    }

    // Restore variables...
    name = prev_name;
    parent = prev_parent;
  }
  
  //////////////////////////////////////////////////////////////////////
  // Transform
  //////////////////////////////////////////////////////////////////////
  void onTransform(CyberX3D::TransformNode* t)
  {
    CyberX3D::SFMatrix x3dmat;
    float m[4][4];
    support3d::mat4d prev_transform;
    std::string prev_name;
    object prev_parent;

    // Get the transformation matrix...
    t->getSFMatrix(&x3dmat);
    x3dmat.getValue(m);
    support3d::mat4d mat(m[0][0], m[1][0], m[2][0], m[3][0],
                         m[0][1], m[1][1], m[2][1], m[3][1],
                         m[0][2], m[1][2], m[2][2], m[3][2],
                         m[0][3], m[1][3], m[2][3], m[3][3]);

    // Push global state variables...
    prev_transform = transform;
    prev_name = name;
    prev_parent = parent;
    transform *= mat;
    name = t->getName();
    if (name=="")
      name = prev_name;

    // Create a Group object if there is more than 1 child...
    if (t->getNChildNodes()>1)
    {
      dict locals;
      locals["mat"] = material;
      locals["parent"] = parent;
      if (!exec("g=Group(name='"+name+"', material=mat, parent=parent)", locals))
	return;

      // obj: The WorldObject
      support3d::WorldObject& obj = extract<support3d::WorldObject&>(locals["g"])();
      obj.transform.setValue(getInvWorldTransform(parent)*transform);

      parent = locals["g"];
    }

    // Process the children...
    for(CyberX3D::Node* n=t->getChildNodes(); n!=0; n=n->next())
    {
      processNode(n);
    }

    // Restore variables...
    transform = prev_transform;
    name = prev_name;
    parent = prev_parent;
  }

  //////////////////////////////////////////////////////////////////////
  // Shape
  //////////////////////////////////////////////////////////////////////
  void onShape(CyberX3D::ShapeNode* shp)
  {
    //    std::cout<<"SHAPE ("<<shp->getName()<<")"<<std::endl;
    
    // Process the Appearance node...
    material = object();
    CyberX3D::AppearanceNode* app = shp->getAppearanceNodes();
    if (app!=0)
    {
      CyberX3D::MaterialNode* mat = app->getMaterialNodes();
      CyberX3D::ImageTextureNode* imgtex = app->getImageTextureNodes();
      CyberX3D::TextureTransformNode* textransform = app->getTextureTransformNodes();
      std::ostringstream args;
      std::ostringstream transformstr;
      std::string texcmd = "";

      // Get the texture transformation and store the matrix command 
      // in "transformstr"
      if (textransform!=0)
      {
	CyberX3D::SFMatrix x3dmat;
	float m[4][4];
	textransform->getSFMatrix(&x3dmat);
	x3dmat.getValue(m);
	transformstr<<"mat4("<<m[0][0]<<","<<m[1][0]<<","<<m[2][0]<<","<<m[3][0];
	transformstr<<","<<m[0][1]<<","<<m[1][1]<<","<<m[2][1]<<","<<m[3][1];
	transformstr<<","<<m[0][2]<<","<<m[1][2]<<","<<m[2][2]<<","<<m[3][2];
	transformstr<<","<<m[0][3]<<","<<m[1][3]<<","<<m[2][3]<<","<<m[3][3];
	transformstr<<")";
      }
      else
      {
	transformstr<<"mat4(1)";
      }

      // Create the texture map creation call...
      if (imgtex!=0)
      {
	std::string texname = "";
	if (imgtex->getNUrls()>0)
	{
	  texname = imgtex->getUrl(0);
	}
	texcmd = "GLTexture(imagename='"+texname+"', transform="+transformstr.str()+")";
      }

      // Create the argument string for the material...
      if (mat!=0)
      {
	float col[3];
	float ambientI;

	if (std::string(mat->getName())!="")
	  args<<"name='"<<mat->getName()<<"', ";

	ambientI = mat->getAmbientIntensity();
	mat->getDiffuseColor(col);
	args<<"ambient=("<<ambientI*col[0]<<", "<<ambientI*col[1]<<", "<<ambientI*col[2]<<", 1), ";
	args<<"diffuse=("<<col[0]<<", "<<col[1]<<", "<<col[2]<<", 1), ";

	mat->getSpecularColor(col);
	args<<"specular=("<<col[0]<<", "<<col[1]<<", "<<col[2]<<", 1), ";
	args<<"shininess="<<mat->getShininess()<<", ";

	mat->getEmissiveColor(col);
	args<<"emission=("<<col[0]<<", "<<col[1]<<", "<<col[2]<<", 1), ";
      }

      // Append the texture map if there was one...
      if (texcmd!="")
      {
	args<<"texture="<<texcmd;
      }

      // Create the material...
      if (mat!=0 || imgtex!=0)
      {
	std::string matcall = "m = GLMaterial("+args.str()+")";
	//	std::cout<<matcall<<std::endl;
	dict locals;
	if (!exec(matcall, locals))
	  return;

	material = locals["m"];
      }
    }

    // Process the geometry node...
    processNode(shp->getGeometry3DNode());
  }

  //////////////////////////////////////////////////////////////////////
  // Viewpoint
  //////////////////////////////////////////////////////////////////////
  void onViewPoint(CyberX3D::ViewpointNode* vp)
  {
    // Get the transformation matrix...
    float v[4];
    vp->getOrientation(v);
    support3d::vec3d axis(v[0], v[1], v[2]);

    // Compute the (local) position...
    support3d::mat4d L;
    vp->getPosition(v);
    L.setTranslation(support3d::vec3d(v[0], v[1], v[2]));
    support3d::mat4d m;
    m = getInvWorldTransform(parent)*transform*L;
    support3d::vec4d c;
    c = m.getColumn(3);
    support3d::vec3d pos(c.x, c.y, c.z);

    // Compute the orientation...
    support3d::mat3d r;
    r.setRotation(v[3], axis);
    support3d::vec3d target = pos-r.getColumn(2);
    //    std::cout<<"DIR:"<<r.getColumn(2)<<std::endl;

    // Create the camera object...
    std::string camname = std::string(vp->getName());
    std::ostringstream s;
    s<<"cam=TargetCamera(name='"<<camname<<"', ";
    s<<"pos=("<<pos.x<<","<<pos.y<<","<<pos.z<<"), ";
    s<<"target=("<<target.x<<","<<target.y<<","<<target.z<<"), ";
    s<<"parent=parent)";
    dict locals;
    locals["parent"] = parent;
    if (!exec(s.str(), locals))
      return;

    // obj: The WorldObject
    //    support3d::WorldObject& obj = extract<support3d::WorldObject&>(locals["cam"])();

    //    std::cout<<mat<<std::endl;

    //    std::cout<<" Pos: "<<v[0]<<" "<<v[1]<<" "<<v[2]<<std::endl;
    //    std::cout<<" FOV: "<<vp->getFieldOfView()<<std::endl;
    //    std::cout<<" Desc: "<<vp->getDescription()<<std::endl;
  }

  //////////////////////////////////////////////////////////////////////
  // Box
  //////////////////////////////////////////////////////////////////////
  void onBox(CyberX3D::BoxNode* box)
  {
    std::ostringstream s;
    s<<"b=Box(name='"<<name<<"', lx="<<box->getX()<<", ly="<<box->getY()<<", lz="<<box->getZ()<<", material=mat, parent=parent)";
    //    std::cout<<"BOX"<<std::endl;
    //    std::cout<<" "<<s.str().c_str()<<std::endl;

    dict locals;
    locals["mat"] = material;
    locals["parent"] = parent;
    if (!exec(s.str(), locals))
      return;

    // obj: The WorldObject
    support3d::WorldObject& obj = extract<support3d::WorldObject&>(locals["b"])();
    obj.transform.setValue(getInvWorldTransform(parent)*transform);
  }

  //////////////////////////////////////////////////////////////////////
  // Sphere
  //////////////////////////////////////////////////////////////////////
  void onSphere(CyberX3D::SphereNode* sph)
  {
    std::ostringstream s;
    s<<"s=Sphere(name='"<<name<<"', radius="<<sph->getRadius()<<", material=mat, parent=parent)";

    dict locals;
    locals["mat"] = material;
    locals["parent"] = parent;
    if (!exec(s.str(), locals))
      return;

    // obj: The WorldObject
    support3d::WorldObject& obj = extract<support3d::WorldObject&>(locals["s"])();
    obj.transform.setValue(getInvWorldTransform(parent)*transform);
  }

  //////////////////////////////////////////////////////////////////////
  // Cylinder
  //////////////////////////////////////////////////////////////////////
  void onCylinder(CyberX3D::CylinderNode* cyl)
  {
    std::cout<<"Warning: Cylinder nodes not yet supported"<<std::endl;
  }

  //////////////////////////////////////////////////////////////////////
  // Cone
  //////////////////////////////////////////////////////////////////////
  void onCone(CyberX3D::ConeNode* cone)
  {
    std::cout<<"Warning: Cone nodes not yet supported"<<std::endl;
  }

  
  //////////////////////////////////////////////////////////////////////
  // IndexedFaceSet
  //////////////////////////////////////////////////////////////////////
  void onIndexedFaceSet(CyberX3D::IndexedFaceSetNode* ifs)
  {
    CyberX3D::CoordinateNode* coords = ifs->getCoordinateNodes();
    int numverts, numfaces;
    int i, c;
    //    std::cout<<"INDEXEDFACESET"<<std::endl;
    if (coords==0)
    {
      // error: no vertices given
      std::cerr<<"Error: No vertices given"<<std::endl;
      return;
    }

//    std::cout<<"  Create trimesh"<<std::endl;

    // Create a TriMesh...
    dict locals;
    locals["mat"] = material;
    locals["parent"] = parent;
    if (!exec("m = TriMesh(name='"+name+"', material=mat, parent=parent)", locals))
      return;
    // obj: The mesh WorldObject
    support3d::WorldObject& obj = extract<support3d::WorldObject&>(locals["m"])();
    // tm: The TriMeshGeom
    support3d::TriMeshGeom* tm = dynamic_cast<support3d::TriMeshGeom*>(obj.getGeom().get());

    obj.transform.setValue(getInvWorldTransform(parent)*transform);

    ///////// Set vertices //////////////
//    std::cout<<"  set vertices"<<std::endl;

    numverts = coords->getNPoints();
    tm->verts.resize(numverts);
    for(i=0; i<numverts; i++)
    {
      float pnt[3];
      coords->getPoint(i, pnt);
      tm->verts.setValue(i, support3d::vec3d(pnt[0], pnt[1], pnt[2]));
    }

    ///////// Set faces //////////////
//    std::cout<<"  set faces"<<std::endl;

    // Determine the number of faces and check if every face is a
    // triangle...
    numfaces = 0;
    c = 0;
    for(i=0; i<ifs->getNCoordIndexes(); i++)
    {
      int idx = ifs->getCoordIndex(i);
      if (idx==-1)
      {
	if (c==0)
	  break;
	if (c!=3)
	  throw support3d::EValueError("Only triangle meshes are supported yet");
	numfaces++;
	c = 0;
      }
      else
      {
	c++;
      }
    }      

    // Set the faces...
    tm->faces.resize(numfaces);
    for(i=0; i<numfaces; i++)
    {
      int offset = i*4;
      int face[3];
      face[0] = ifs->getCoordIndex(offset);
      face[1] = ifs->getCoordIndex(offset+1);
      face[2] = ifs->getCoordIndex(offset+2);
      tm->faces.setValues(i, face);
    }

    ///////// Set normals ////////////
      
    _setIFSnormals(ifs, tm, numfaces);

    ///////// Set tex coords ////////////
      
    _setIFStexcoords(ifs, tm, numfaces);

    ///////// Set vertex colors ////////////
      
    _setIFScolors(ifs, tm, numfaces);

  }

  // Set normals
  void _setIFSnormals(CyberX3D::IndexedFaceSetNode* ifs, support3d::TriMeshGeom* tm, int numfaces)
  {
    int i;
    CyberX3D::NormalNode* normals = ifs->getNormalNodes();
    if (normals==0)
      return;

    //    std::cout<<"  set normals"<<std::endl;
    if (ifs->getNormalPerVertex()==false)
    {
      std::cerr<<"Normals per face not yet supported"<<std::endl;
      return;
    }

    if (ifs->getNNormalIndexes()!=ifs->getNCoordIndexes())
    {
      std::cerr<<"Error: Number of normal indices != number of coordinate indices"<<std::endl;
      return;
    }

    // Set normals...
    int numnormals = normals->getNVectors();
    tm->newVariable("N", support3d::USER, support3d::NORMAL, 1, numnormals);
    support3d::ArraySlot<support3d::vec3d>& tm_normals = dynamic_cast<support3d::ArraySlot<support3d::vec3d>&>(tm->slot("N"));

    for(i=0; i<numnormals; i++)
    {
      float n[3];
      normals->getVector(i, n);
      tm_normals.setValue(i, support3d::vec3d(n[0], n[1], n[2]).normalize());
    }
	
    // Set normal faces...

    tm->newVariable("Nfaces", support3d::UNIFORM, support3d::INT, 3);
    support3d::ArraySlot<int>& tm_nfaces = dynamic_cast<support3d::ArraySlot<int>&>(tm->slot("Nfaces"));

    for(i=0; i<numfaces; i++)
    {
      int offset = i*4;
      int nface[3];
      nface[0] = ifs->getNormalIndex(offset);
      nface[1] = ifs->getNormalIndex(offset+1);
      nface[2] = ifs->getNormalIndex(offset+2);
      tm_nfaces.setValues(i, nface);
    }
  }

  // Set tex coords
  void _setIFStexcoords(CyberX3D::IndexedFaceSetNode* ifs, support3d::TriMeshGeom* tm, int numfaces)
  {
    int i;
    CyberX3D::TextureCoordinateNode* texcoords = ifs->getTextureCoordinateNodes();
    if (texcoords==0)
      return;

    //    std::cout<<"  set tex coords"<<std::endl;

    if (ifs->getNTexCoordIndexes()!=ifs->getNCoordIndexes())
    {
      std::cerr<<"Error: Number of tex coord indices != number of coordinate indices"<<std::endl;
      return;
    }

    // Set texture coords...
    int numtexcoords = texcoords->getNPoints();
    tm->newVariable("st", support3d::USER, support3d::FLOAT, 2, numtexcoords);
    support3d::ArraySlot<double>& tm_texcoords = dynamic_cast<support3d::ArraySlot<double>&>(tm->slot("st"));

    for(i=0; i<numtexcoords; i++)
    {
      float st[2];
      double st2[2];
      texcoords->getPoint(i, st);
      st2[0] = st[0];
      st2[1] = st[1];
      tm_texcoords.setValues(i, st2);
    }
	
    // Set texture faces...

    tm->newVariable("stfaces", support3d::UNIFORM, support3d::INT, 3);
    support3d::ArraySlot<int>& tm_texfaces = dynamic_cast<support3d::ArraySlot<int>&>(tm->slot("stfaces"));

    for(i=0; i<numfaces; i++)
    {
      int offset = i*4;
      int tface[3];
      tface[0] = ifs->getTexCoordIndex(offset);
      tface[1] = ifs->getTexCoordIndex(offset+1);
      tface[2] = ifs->getTexCoordIndex(offset+2);
      tm_texfaces.setValues(i, tface);
    }
  }

  // Set vertex colors
  void _setIFScolors(CyberX3D::IndexedFaceSetNode* ifs, support3d::TriMeshGeom* tm, int numfaces)
  {
    int i;
    CyberX3D::ColorNode* colors = ifs->getColorNodes();
    if (colors==0)
      return;

    if (ifs->getColorPerVertex()==false)
    {
      std::cerr<<"Colors per face not yet supported"<<std::endl;
      return;
    }

    if (ifs->getNColorIndexes()!=ifs->getNCoordIndexes())
    {
      std::cerr<<"Error: Number of color indices != number of coordinate indices"<<std::endl;
      return;
    }

    // Set colors...
    int numcolors = colors->getNColors();
    tm->newVariable("Cs", support3d::USER, support3d::COLOR, 1, numcolors);
    support3d::ArraySlot<support3d::vec3d>& tm_colors = dynamic_cast<support3d::ArraySlot<support3d::vec3d>&>(tm->slot("Cs"));

    for(i=0; i<numcolors; i++)
    {
      float c[3];
      colors->getColor(i, c);
      tm_colors.setValue(i, support3d::vec3d(c[0], c[1], c[2]));
    }
	
    // Set color faces...

    tm->newVariable("Csfaces", support3d::UNIFORM, support3d::INT, 3);
    support3d::ArraySlot<int>& tm_csfaces = dynamic_cast<support3d::ArraySlot<int>&>(tm->slot("Csfaces"));

    for(i=0; i<numfaces; i++)
    {
      int offset = i*4;
      int cface[3];
      cface[0] = ifs->getColorIndex(offset);
      cface[1] = ifs->getColorIndex(offset+1);
      cface[2] = ifs->getColorIndex(offset+2);
      tm_csfaces.setValues(i, cface);
    }
  }


  //////////////////////////////////////////////////////////////////////
  // PointLight
  //////////////////////////////////////////////////////////////////////
  void onPointLight(CyberX3D::PointLightNode* lgt)
  {
    std::string lgtname = std::string(lgt->getName());
    std::string cmd = "lgt=GLPointLight(name='"+lgtname+"', parent=parent)";
    dict locals;
    locals["parent"] = parent;
    if (!exec(cmd, locals))
      return;

    // obj: The GLPointLight
    support3d::GLPointLight& obj = extract<support3d::GLPointLight&>(locals["lgt"])();

    // Set transformation...
    float v[3];
    lgt->getLocation(v);
    support3d::mat4d T;
    T.setTranslation(support3d::vec3d(v[0], v[1], v[2]));
    obj.transform.setValue(getInvWorldTransform(parent)*transform*T);

    // Set color
    lgt->getColor(v);
    obj.diffuse.setValue(support3d::vec3d(v[0], v[1], v[2]));

    // Set intensity
    obj.intensity.setValue(lgt->getIntensity());

    // Set attenuation factors
    lgt->getAttenuation(v);
    obj.constant_attenuation.setValue(v[0]);
    obj.linear_attenuation.setValue(v[1]);
    obj.quadratic_attenuation.setValue(v[2]);

    // Set enabled flag
    obj.enabled.setValue(lgt->isOn());
  }

  //////////////////////////////////////////////////////////////////////
  // SpotLight
  //////////////////////////////////////////////////////////////////////
  void onSpotLight(CyberX3D::SpotLightNode* lgt)
  {
    std::string lgtname = std::string(lgt->getName());
    std::string cmd = "lgt=GLFreeSpotLight(name='"+lgtname+"', parent=parent)";
    dict locals;
    locals["parent"] = parent;
    if (!exec(cmd, locals))
      return;

    // obj: The GLSpotLight
    support3d::GLSpotLight& obj = extract<support3d::GLSpotLight&>(locals["lgt"])();

    // Set transformation...
    float v[3];
    lgt->getLocation(v);
    support3d::vec3d pos(v[0], v[1], v[2]);
    lgt->getDirection(v);
    support3d::vec3d dir(v[0], v[1], v[2]);
    support3d::mat4d L;
    L.setLookAt(pos, pos+dir, support3d::vec3d(0,1,0));
    obj.transform.setValue(getInvWorldTransform(parent)*transform*L);

    // Set color
    lgt->getColor(v);
    obj.diffuse.setValue(support3d::vec3d(v[0], v[1], v[2]));

    // Set intensity
    obj.intensity.setValue(lgt->getIntensity());

    // Set cutOffAngle
    obj.cutoff.setValue(lgt->getCutOffAngle()*180.0/3.1415926535897931);

    // Set attenuation factors
    lgt->getAttenuation(v);
    obj.constant_attenuation.setValue(v[0]);
    obj.linear_attenuation.setValue(v[1]);
    obj.quadratic_attenuation.setValue(v[2]);

    // Set enabled flag
    obj.enabled.setValue(lgt->isOn());
  }

  //////////////////////////////////////////////////////////////////////
  // DirectionalLight
  //////////////////////////////////////////////////////////////////////
  void onDirectionalLight(CyberX3D::DirectionalLightNode* lgt)
  {
    std::string lgtname = std::string(lgt->getName());
    std::string cmd = "lgt=GLFreeDistantLight(name='"+lgtname+"', parent=parent)";
    dict locals;
    locals["parent"] = parent;
    if (!exec(cmd, locals))
      return;

    // obj: The GLDistantLight
    support3d::GLDistantLight& obj = extract<support3d::GLDistantLight&>(locals["lgt"])();

    // Set transformation...
    float v[3];
    lgt->getDirection(v);
    support3d::vec3d dir(v[0], v[1], v[2]);
    support3d::mat4d L;
    L.setLookAt(support3d::vec3d(0), dir, support3d::vec3d(0,1,0));
    obj.transform.setValue(getInvWorldTransform(parent)*transform*L);

    // Set color
    lgt->getColor(v);
    obj.diffuse.setValue(support3d::vec3d(v[0], v[1], v[2]));

    // Set intensity
    obj.intensity.setValue(lgt->getIntensity());

    // Set enabled flag
    obj.enabled.setValue(lgt->isOn());
  }

};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void show(CyberX3D::Node* node, int depth)
{
  if (node==0)
    return;
  for(int i=0; i<depth; i++)
  {
    std::cout<<" ";
  }
  std::cout<<"Name: '"<<node->getName()<<"' - Type: ";
  std::cout<<node->getTypeString()<<" ("<<node->getType()<<")"<<std::endl;
  show(node->getChildNodes(), depth+2);
  show(node->next(), depth);
}


void cyber(char* name)
{
  CyberX3D::SceneGraph* scngraph = new CyberX3D::SceneGraph();
  if (scngraph==0)
  {
    std::cout<<"Kein Speicher für SceneGraph"<<std::endl;
    return;
  }

  std::cout<<"Filename: "<<name<<std::endl;

  if (scngraph->load(name))
  {
    show(scngraph->getNodes(), 0);
  }
  else
  {
    std::cout<<"Fehler beim Laden!"<<std::endl;
    std::cout<<scngraph->getParserErrorMessage()<<" (line ";
    std::cout<<scngraph->getParserErrorLineNumber()<<"): ";
    std::cout<<scngraph->getParserErrorToken()<<std::endl;
  }

  delete scngraph;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


void cyberx3d()
{
  def("cyber", cyber);

  // X3DReader
  class_<X3DReader>("X3DReader", init<optional<bool> >())
    .def("read", &X3DReader::read)
  ;
}
