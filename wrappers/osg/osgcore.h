/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is the Python Computer Graphics Kit.
 *
 * The Initial Developer of the Original Code is Matthias Baas.
 * Portions created by the Initial Developer are Copyright (C) 2004
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Ole Ciliox <ole@ira.uka.de>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

/*======================================================================
 OpenSceneGraph Engine Wrapped

 Author: Ole Ciliox (ole@ira.uka.de)
======================================================================*/
 
#ifndef __OsgCore_H__
#define __OsgCore_H__

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

// why oh why so many osg includes!? 

#include <Producer/RenderSurface>
#include <osg/Timer>
#include <osgUtil/SceneView>

#include <osg/Image>
#include <osgProducer/Viewer>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Material>
#include <osg/Vec3>
#include <osg/MatrixTransform>
#include <osg/Texture2D>
#include <osg/PolygonStipple>
#include <osg/TriangleFunctor>
#include <osg/io_utils> 
#include <osg/PositionAttitudeTransform>
#include <Producer/RenderSurface>
#include <osg/Timer>
#include <osgUtil/SceneView>
#include <osgDB/ReadFile>
#include <osgGA/GUIEventHandler>

//#include <osgUtil/Optimizer>
//#include <osgUtil/SmoothingVisitor>

#include <osgDB/Registry>

#include <osgText/Font>
#include <osgText/Text>

#include <osgGA/GUIEventHandler>

// boost
#include "boost/python/list.hpp"
#include "boost/python/tuple.hpp"

// supportlib
#undef max
#undef min

#include "worldobject.h"
#include "geomobject.h"
#include "trimeshgeom.h"
#include "glmaterial.h"

#include "lightsource.h"
#include "gldistantlight.h"
#include "glpointlight.h"
#include "glspotlight.h"

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// basic transformation dependents xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

class TransformDependent : public support3d::Dependent
{
public:
	osg::MatrixTransform *mpMatrixTransform;
	support3d::WorldObject* mpWorldObject;

	TransformDependent() {};	// constr
	~TransformDependent() {};	// destr

	void onValueChanged()
	{
		double pValueList[16];
		mpWorldObject->localTransform().toList( pValueList );
		mpMatrixTransform->setMatrix( osg::Matrixd(pValueList) );
	};
	void setup(support3d::WorldObject* pWorldObject, osg::MatrixTransform *pMTransform)
	{
		mpWorldObject = pWorldObject; mpMatrixTransform = pMTransform;		
	};
};

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// material dependent for all material dependencies xxxxxxxxxxxxxxxxxxxxxxx

class MaterialDependent : public support3d::Dependent
{
public:
	osg::Material *mpMaterial;
	support3d::GLMaterial *mpGLMaterial;
	osg::StateSet *mpStateSet;

	MaterialDependent() {};		// constr
	~MaterialDependent() {};	// destr

	void onValueChanged()
	{
		mpMaterial->setAmbient( osg::Material::Face::FRONT_AND_BACK, osg::Vec4( mpGLMaterial->ambient.getValue().x, mpGLMaterial->ambient.getValue().y, mpGLMaterial->ambient.getValue().z, 1.0 ) );
		mpMaterial->setDiffuse( osg::Material::Face::FRONT_AND_BACK, osg::Vec4( mpGLMaterial->diffuse.getValue().x, mpGLMaterial->diffuse.getValue().y, mpGLMaterial->diffuse.getValue().z, 1.0 ));
		mpMaterial->setSpecular( osg::Material::Face::FRONT_AND_BACK, osg::Vec4( mpGLMaterial->specular.getValue().x, mpGLMaterial->specular.getValue().y, mpGLMaterial->specular.getValue().z, 1.0 ));
		mpMaterial->setEmission( osg::Material::Face::FRONT_AND_BACK, osg::Vec4( mpGLMaterial->emission.getValue().x, mpGLMaterial->emission.getValue().y, mpGLMaterial->emission.getValue().z, 1.0 ));
		mpMaterial->setShininess( osg::Material::Face::FRONT_AND_BACK, mpGLMaterial->shininess.getValue() );
	};
	void setup(support3d::GLMaterial *pGLMaterial, osg::Material *pMaterial, osg::StateSet *pStateSet)
	{
		mpMaterial = pMaterial; mpGLMaterial = pGLMaterial; mpStateSet = pStateSet;
	};
};

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// uniform shader dependents xxxxxxxxxxxxxxxxxxxxxx

class UniformDependent : public support3d::Dependent
{
public:
	osg::Uniform *mpUniform;
	support3d::ISlot *mpSlot;
	std::string mName;
};

class UniformDoubleOrFloatDependent : public UniformDependent
{
public:
	UniformDoubleOrFloatDependent() {};
	~UniformDoubleOrFloatDependent() {};
	void onValueChanged()
	{
		// this is sooooo silly
		support3d::Slot<double>* slot2 = dynamic_cast<support3d::Slot<double>*>(mpSlot);
		if ( slot2 )
		{
			std::cout << "                 xxxxxxxx BING" << std::endl;
			mpUniform->set( (float)(slot2->getValue()) ); // TODO JUST TEST BEHAVIOUR
			return;
		}
	};
	void setup(osg::Uniform *pUniform, support3d::ISlot *pSlot, std::string name)
	{
		mpUniform = pUniform; mpSlot = pSlot; mName= name;
	};
};

class UniformVec4Dependent : public UniformDependent
{
public:
	UniformVec4Dependent() {};
	~UniformVec4Dependent() {};
	void onValueChanged()
	{
		// this is sooooo silly
		support3d::Slot< support3d::vec4d >* slot2 = dynamic_cast<support3d::Slot< support3d::vec4d >*>(mpSlot);
		if ( slot2 )
		{
			std::cout << "                 xxxxxxxx BING" << std::endl;
			support3d::vec4d tmp = slot2->getValue();
			mpUniform->set( osg::Vec4(tmp.x, tmp.y, tmp.z, tmp.w) ); // TODO JUST TEST BEHAVIOUR
			return;
		}
	};
	void setup(osg::Uniform *pUniform, support3d::ISlot *pSlot, std::string name)
	{
		mpUniform = pUniform; mpSlot = pSlot; mName= name;
	};
};

class UniformVec3Dependent : public UniformDependent
{
public:
	UniformVec3Dependent() {};
	~UniformVec3Dependent() {};
	void onValueChanged()
	{
		// this is sooooo silly
		support3d::Slot< support3d::vec3d >* slot2 = dynamic_cast<support3d::Slot< support3d::vec3d >*>(mpSlot);
		if ( slot2 )
		{
			std::cout << "                 xxxxxxxx BING" << std::endl;
			support3d::vec3d tmp = slot2->getValue();
			mpUniform->set( osg::Vec3(tmp.x, tmp.y, tmp.z) ); // TODO JUST TEST BEHAVIOUR
			return;
		}
	};
	void setup(osg::Uniform *pUniform, support3d::ISlot *pSlot, std::string name)
	{
		mpUniform = pUniform; mpSlot = pSlot; mName= name;
	};
};

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// material dependent for all light sources xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// TODO: Aufsplittern für einzelne Typen und einzelne Arten 

enum OSGLIGHTTYPE { DEP_POINT = 0, DEP_DIRECTIONAL = 1, DEP_SPOT = 2 };
class LightDependent : public support3d::Dependent
{
public:
	osg::MatrixTransform *mpMatrixTransform;
	support3d::WorldObject* mpWorldObject;

	osg::LightSource* mpLightSource;
	OSGLIGHTTYPE mLightType;
	unsigned int mLightNumber;

	void onValueChanged(); // see huge implementation in c++-file
	void setup(support3d::WorldObject* pWorldObject, osg::MatrixTransform *pMTransform)
	{
		mpWorldObject = pWorldObject; mpMatrixTransform = pMTransform;		
	};
	void setupLightStuff(unsigned int lightNum, OSGLIGHTTYPE lightType, osg::LightSource* pLightSource)
	{
		mLightNumber = lightNum; mLightType = lightType; mpLightSource = pLightSource;
	};
	LightDependent() {};
	~LightDependent() {};
};

// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// The Key Event Handling class xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

class MyEventHandler : public osgGA::GUIEventHandler
{
public:

	//boost::python::list keyDowns;
	//boost::python::list keyUps;
#define KEY_LIST_LENGTH 30
#define MOUSE_LIST_LENGTH 30
	int KeyDownInts[KEY_LIST_LENGTH];
	int KeyDownMods[KEY_LIST_LENGTH];
	int KeyUpInts[KEY_LIST_LENGTH];
	int KeyUpMods[KEY_LIST_LENGTH];

	int mouseButtonList[MOUSE_LIST_LENGTH];
	float mouseXList[MOUSE_LIST_LENGTH];
	float mouseOldX;
	float mouseYList[MOUSE_LIST_LENGTH];
	float mouseOldY;
	float mousedXList[MOUSE_LIST_LENGTH];
	float mousedYList[MOUSE_LIST_LENGTH];	
	int eventTypeList[MOUSE_LIST_LENGTH];	
	int oldMouseButonState;

	int currentKeyDownIndex;
	int currentKeyUpIndex;
	int currentMouseIndex;
	float posX, posY, moveX, moveY, oldX, oldY;
	bool isMotion;
	int mouseButtons;

	typedef void (*functionType) ();
	enum keyStatusType
	{
      KEY_UP, KEY_DOWN
	};

	struct functionStatusType
	{
		functionStatusType() {keyState = KEY_UP; keyFunction = NULL;}
		functionType keyFunction;
		keyStatusType keyState;
	};

	typedef std::map<int, functionStatusType > keyFunctionMap;

	MyEventHandler()
	{
		currentMouseIndex = 0; // new
		currentKeyDownIndex = 0;
		currentKeyUpIndex = 0;
		posX = 0.0f;
		posY = 0.0f;
		moveX = 0.0f;
		moveY = 0.0f;
		oldX = 0.0f;
		oldY = 0.0f;
		isMotion = false;
		mouseButtons = 0;

		mouseOldX = 0.0f;
		mouseOldY = 0.0f;

		oldMouseButonState = 0;
	}

	bool addFunction(int whatKey, functionType newFunction)
	{
		if ( keyFuncMap.end() != keyFuncMap.find( whatKey ))
		{
			std::cout << "duplicate key '" << whatKey << "' ignored." << std::endl;
			return false;
		}
		else
		{
			keyFuncMap[whatKey].keyFunction = newFunction;
			return true;
		}
	};

	bool addFunction(int whatKey, keyStatusType keyPressStatus, functionType newFunction)
	{
		if (keyPressStatus == KEY_DOWN)
		{
			return addFunction(whatKey,newFunction);
		}
		else
		{
			if ( keyUPFuncMap.end() != keyUPFuncMap.find( whatKey )) 
			{
				std::cout << "duplicate key '" << whatKey << "' ignored." << std::endl;
				return false;
			}
			else
			{
				keyUPFuncMap[whatKey].keyFunction = newFunction;
				return true;
			}
		} // KEY_UP
	};

	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter&)
	{
		bool newKeyDownEvent = false;
		bool newKeyUpEvent   = false;

		mouseButtons = (ea.getButtonMask());

		if( (mouseButtons != 0) && (oldMouseButonState == 0) )
		{
			// MOUSE PRESSED EVENT
			mouseButtonList[currentMouseIndex] = mouseButtons;
			mouseXList[currentMouseIndex] = ( ea.getX() + 1.0f)*0.5f;
			mouseYList[currentMouseIndex] = (-ea.getY() + 1.0f)*0.5f;
			mousedXList[currentMouseIndex] = mouseXList[currentMouseIndex] - mouseOldX;
			mousedYList[currentMouseIndex] = mouseYList[currentMouseIndex] - mouseOldY;
			eventTypeList[currentMouseIndex] = 0; // PRESS = 0

			std::cout << "MOUSE PRESSED!" << std::endl;

			oldMouseButonState = mouseButtons; // update
			mouseOldX = mouseXList[currentMouseIndex];
			mouseOldY = mouseYList[currentMouseIndex];
			currentMouseIndex++;
		}
		if( (mouseButtons != 0) && (oldMouseButonState != 0) )
		{
			// MOUSE HELD EVENT
/*			mouseButtonList[currentMouseIndex] = mouseButtons;
			mouseXList[currentMouseIndex] = ( ea.getX() + 1.0f)*0.5f;
			mouseYList[currentMouseIndex] = (-ea.getY() + 1.0f)*0.5f;
			mousedXList[currentMouseIndex] = mouseXList[currentMouseIndex] - mouseOldX;
			mousedYList[currentMouseIndex] = mouseYList[currentMouseIndex] - mouseOldY;
			eventTypeList[currentMouseIndex] = 0; // PRESS = 0

			//std::cout << "MOUSE PRESSED!" << std::endl;

			oldMouseButonState = mouseButtons; // update
			mouseOldX = mouseXList[currentMouseIndex];
			mouseOldY = mouseYList[currentMouseIndex];
			currentMouseIndex++;
*/
		}
		if( (mouseButtons == 0) && (oldMouseButonState != 0) )
		{
			// MOUSE RELEASE EVENT
			mouseButtonList[currentMouseIndex] = oldMouseButonState;
			mouseXList[currentMouseIndex] = ( ea.getX() + 1.0f)*0.5f;
			mouseYList[currentMouseIndex] = (-ea.getY() + 1.0f)*0.5f;
			mousedXList[currentMouseIndex] = mouseXList[currentMouseIndex] - mouseOldX;
			mousedYList[currentMouseIndex] = mouseYList[currentMouseIndex] - mouseOldY;
			eventTypeList[currentMouseIndex] = 1; // RELEASE = 1

			std::cout << "MOUSE RELEASED!" << std::endl;

			oldMouseButonState = mouseButtons; // update
			mouseOldX = mouseXList[currentMouseIndex];
			mouseOldY = mouseYList[currentMouseIndex];
			currentMouseIndex++;
		}

		if( ea.getEventType() == osgGA::GUIEventAdapter::MOVE || ea.getEventType() == osgGA::GUIEventAdapter::PUSH || ea.getEventType() == osgGA::GUIEventAdapter::RELEASE || ea.getEventType() == osgGA::GUIEventAdapter::DRAG )//|| ( ea.getButtonMask() != 0 ) )
		{
			// MOTIONS
			isMotion = true;
			posX = ( ea.getX() + 1.0f)*0.5f;
			posY = (-ea.getY() + 1.0f)*0.5f;
			moveX = posX-oldX;
			moveY = posY-oldY;
			oldX = posX; oldY = posY;

			//mouseButtons = (ea.getButtonMask());
			//std::cout << mouseButtons << " ... " << std::endl;
		}

		switch(ea.getEventType())
		{
			case(osgGA::GUIEventAdapter::KEYDOWN):
				{
					if ( ea.getKey() > 65280 )
						KeyDownInts[currentKeyDownIndex] = ea.getKey() - 65280;
					else
						KeyDownInts[currentKeyDownIndex] = ea.getKey();
					KeyDownMods[currentKeyDownIndex] = ea.getModKeyMask();
					currentKeyDownIndex++;

					keyFunctionMap::iterator itr = keyFuncMap.find(ea.getKey());
					if (itr != keyFuncMap.end())
					{
						if ( (*itr).second.keyState == KEY_UP )
						{
						(*itr).second.keyState = KEY_DOWN;
						newKeyDownEvent = true;
						}
						if (newKeyDownEvent)
						{
						(*itr).second.keyFunction();
						newKeyDownEvent = false;
						}
						return true;
					}
					return false;
				}
			case(osgGA::GUIEventAdapter::KEYUP):
				{
					if ( ea.getKey() > 65280 )
						KeyUpInts[currentKeyUpIndex] = ea.getKey() - 65280;
					else
						KeyUpInts[currentKeyUpIndex] = ea.getKey();
					KeyUpMods[currentKeyUpIndex] = ea.getModKeyMask();
					currentKeyUpIndex++;

					keyFunctionMap::iterator itr = keyFuncMap.find(ea.getKey());
					if (itr != keyFuncMap.end() )
					{
						(*itr).second.keyState = KEY_UP;
					}
					itr = keyUPFuncMap.find(ea.getKey());
					if (itr != keyUPFuncMap.end())
					{
						(*itr).second.keyFunction();
						return true;
					}
					return false; 
				}
			default:
				return false;
			}
	};

   // Overloaded accept method for dealing with event handler visitors
   virtual void accept(osgGA::GUIEventHandlerVisitor& v) 
   { 
	   v.visit(*this); 
   };

protected:

   // Storage for registered 'key down' methods and key status
   keyFunctionMap keyFuncMap;

   // Storage for registered 'key up' methods and key status
   keyFunctionMap keyUPFuncMap;
};
 
// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// The One And Only OscCore Class xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 

class OsgCore
{
	enum CGKIT_RESOURCE_TYPE
	{
		RT_MATERIAL, RT_TEXTURE, RT_VERTEXSHADER, RT_FRAGMENTSHADER
	};

	struct ResourceDescriptor
	{
		CGKIT_RESOURCE_TYPE type;
		void *pPtr;
		ResourceDescriptor(CGKIT_RESOURCE_TYPE t, void *ptr ) { type = t; pPtr = ptr; };
	};
public:
	OsgCore();
	~OsgCore();
	void initStuff(support3d::WorldObject& visumRoot);
	void buildTree(support3d::WorldObject& visumRoot, osg::MatrixTransform *pSnode);
	void fooz();
	void renderFrame();
	void setupCamera(support3d::WorldObject& visumRoot, support3d::WorldObject& cam, float nearclip, float farclip, float fov, unsigned int windowW, unsigned int windowH);

	boost::python::list pumpKeyDowns();
	boost::python::list pumpKeyUps();
	boost::python::list pumpMotions();
	boost::python::list pumpMouseDowns();
	MyEventHandler *mpEH;

private:
	osgProducer::Viewer theViewer;
	osg::StateSet *mpRootStateSet;
	osg::MatrixTransform *mpCameraMatrixTransform;
	std::vector<support3d::Dependent*> mDependents;
	std::vector<UniformDependent*> mUniformDependents;
	std::map< unsigned int, support3d::Dependent* > mDependentsMap;
	std::map< unsigned int, ResourceDescriptor > mResourcesMap;
	osg::MatrixTransform *mpRoot;
	unsigned int mLights;
	UniformDependent* findUniformDependentByName(std::string name)
	{
		std::vector<UniformDependent*>::iterator it = mUniformDependents.begin();
		while (it != mUniformDependents.end())
		{
			if( (*it)->mName == name )
				return *it;

			it++;
		}
		return 0;
	};

	void addUniformsByIterator(support3d::SlotIterator &it, osg::StateSet *pStateSet)
	{
								std::string name = it->first;
								std::string type_name = it->second->getSlot().typeName();

								std::cout << "[SHADER]      found a uniform variable slot: " << name << ", " << type_name << std::endl;
								
								if( type_name == "double" || type_name == "float")
								{
									osg::Uniform *pU = pStateSet->getOrCreateUniform(name, osg::Uniform::FLOAT);
									std::cout << "[SHADER]      Created a " << type_name << "-type uniform!" << std::endl;

									UniformDoubleOrFloatDependent *pUDep = new UniformDoubleOrFloatDependent(); // das ist noch nen bisschen doof

									pUDep->setup( pU, &(it->second->getSlot()), it->first );
									it->second->getSlot().addDependent( pUDep );
									mUniformDependents.push_back( pUDep ); // and remember dependent
								}
								else if( type_name == "class support3d::vec4<double>")
								{
									osg::Uniform *pU = pStateSet->getOrCreateUniform( name, osg::Uniform::FLOAT_VEC4 );
									std::cout << "[SHADER]      Created a " << type_name << "-type uniform!" << std::endl;

									UniformVec4Dependent *pUDep = new UniformVec4Dependent(); 

									pUDep->setup( pU, &(it->second->getSlot()), it->first );
									it->second->getSlot().addDependent( pUDep );
									mUniformDependents.push_back( pUDep ); // and remember dependent
								}
								else if( type_name == "class support3d::vec3<double>")
								{
									osg::Uniform *pU = pStateSet->getOrCreateUniform( name, osg::Uniform::FLOAT_VEC3 );
									std::cout << "[SHADER]      Created a " << type_name << "-type uniform!" << std::endl;

									UniformVec3Dependent *pUDep = new UniformVec3Dependent(); 

									pUDep->setup( pU, &(it->second->getSlot()), it->first );
									it->second->getSlot().addDependent( pUDep );
									mUniformDependents.push_back( pUDep ); // and remember dependent
								}	
	};

	void addResource(unsigned int key, CGKIT_RESOURCE_TYPE typeOfRresource, void *pPointerToResource)
	{
		mResourcesMap.insert( std::pair<unsigned int, ResourceDescriptor>(key, ResourceDescriptor(typeOfRresource, pPointerToResource) ) );
	};
	ResourceDescriptor * findResource(unsigned int key)
	{
		if( mResourcesMap.find(key) != mResourcesMap.end() )
		{
			return &(mResourcesMap.find(key)->second);
		}
		else return 0;
	};
	osg::Uniform::Type translateCgkitUTypeToOsgUType(const char* name)
	{
		if(name == "double")
		{
			return osg::Uniform::FLOAT;
		}
		// TODO: FILL IN
		else if(name == "float")
		{
			return osg::Uniform::FLOAT;
		}
	};
};

#endif

