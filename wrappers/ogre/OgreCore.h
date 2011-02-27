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
 Fast Ogre Rendering For CGKIT (FORC)

 Author: Ole Ciliox (ole@ira.uka.de)
======================================================================*/
 
#ifndef __OgreCore_H__
#define __OgreCore_H__


// **********************************************************************
// * Includes								*
// *									*
// **********************************************************************

#include "Ogre.h"

#if OGRE_PLATFORM == PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
//#include "Afx.h"
#include "windows.h"
#endif

#undef max
#undef min

//#include <Math.h>
#include "worldobject.h"
#include "geomobject.h"
#include "trimeshgeom.h"
#include "lightsource.h"
#include "gldistantlight.h"
#include "glpointlight.h"
#include "glspotlight.h"
#include <iostream>
#include "slot.h"
#include "quat.h"
#include "stdio.h"
#include "fstream.h"
#include "boost/python/list.hpp"
#include "boost/python/tuple.hpp"

#include "MyListener.h"
#include "MyMouseListener.h"
#include "MyMouseButtonListener.h"
//
//
// **********************************************************************
// * Namespaces								*
// *									*
// **********************************************************************
//
using std::cout;
using std::endl;
using namespace Ogre;
//
//
// **********************************************************************
// * class OgreDependent						*
// *									*
// * Ogre objects that are dependent from values in cgkit.		*
// **********************************************************************
//
class OgreDependent : public support3d::Dependent
{
public:
	// Attributes
	// 
	bool mIsPointLight;
	bool mIsSpotLight;
	bool mIsDistantLight;
	bool mIsCamera;
	SceneNode* mpNodi;
	Light* mpTheLight;
	Camera*	mpTheCamera;
	support3d::WorldObject* mpTransi;
	String mMaterialName;

	// Methods
	// 
	// **********************************************************************
	// * void OgreDependent::onValueChanged()				*
	// *									*
	// * Called when a value is changed from cgkit side.			*
	// * Needs to aupdate appropriate Ogre side values, depending		*
	// * on the type of the node (e.g. light, geometry, camera, etc).	*
	// **********************************************************************
	void onValueChanged();
	
	OgreDependent(support3d::WorldObject* wo, SceneNode* sn);
	~OgreDependent();

};
//
//
// **********************************************************************
// * class OgreCore							*
// *									*
// * Encapsulates needed OGRE support for cgkit.			*
// **********************************************************************
//
class OgreCore
{
public:
	// Methods
	// 
	// **********************************************************************
	// * OgreCore::OgreCore()						*
	// *									*
	// * Constructor, some standard initialization.				*
	// **********************************************************************
	OgreCore();

	// **********************************************************************
	// * OgreCore::~OgreCore()						*
	// *									*
	// * Destructor, release input handling, dependencies and currently	*
	// * allocated memory.							*
	// * TODO: Visible flag is nor treated as dependency ...		*
	// **********************************************************************
	~OgreCore();
	
	// **********************************************************************
	// * OgreCore::initialize(...)						*
	// *									*
	// * Callable from cgkit side.						*
	// * Initializes ogre, sets with event handling, window attributes,	*
	// * rendering subsystem and filesystem path.				*
	// * And, most importantly, parses whole cgkit scene node tree and	*
	// * creates an ogre tree along with meshes, entities, materials and	*
	// * scene nodes and cgkit-to-ogre dependencies.			*
	// **********************************************************************
	void initialize(support3d::WorldObject& root, unsigned int windowW, unsigned int windowH, bool fullscr, \
			bool shadows, bool opengl, const char * materialpath, int stereo);

	// **********************************************************************
	// * OgreCore::renderTree()						*
	// *									*
	// * Callable from cgkit.						*
	// * Renders the whole ogre scene graph.				*
	// **********************************************************************
	void renderTree();

	// **********************************************************************
	// * OgreCore::clock(unsigned int fps)					*
	// *									*
	// * Callable from cgkit.						*
	// * Called once in a frame, to hold the current frame rate.		*
	// **********************************************************************
	void clock(unsigned int fps);

	// **********************************************************************
	// * OgreCore::insertToTree(...)					*
	// *									*
	// * Callable from cgkit.						*
	// * (Not implemented yet)						*
	// * Inserts nodes to the ogre scene graph after its initialization	*
	// * (useful when there are new objects in cgkit side, which ogre	*
	// * doesnt know about, as it parses the cgkit tree just once)		*
	// * TODO: Implement ...						*
	// **********************************************************************
	void insertToTree(support3d::WorldObject& newObject, support3d::WorldObject& fatherObject, int hasFather);

	// **********************************************************************
	// * OgreCore::changeMaterial(...)					*
	// *									*
	// * Callable from cgkit.						*
	// * When the material changes after the ogre scene graph is created,	*
	// * use this method to set the new material for the entityName 	*
	// * (that is also the name of the cgkit node).				*
	// * TODO: Make materials scene node dependent or mesh dependent ...	*
	// **********************************************************************
	void changeMaterial(std::string newMaterial, std::string entityName);

	// **********************************************************************
	// * OgreCore::setupShadowMode(...)					*
	// *									*
	// * Callable from cgkit.						*
	// **********************************************************************
	void setupShadowMode(int typeFlags, int otherFlags);	

	// **********************************************************************
	// * OgreCore::buildTree(...)						*
	// *									*
	// * Callable from cgkit.						*
	// * Recursive method, that created the ogre scene node tree		*
	// * as well as all dependents to the cgkit world.			*
	// **********************************************************************
	void buildTree(support3d::WorldObject& VisumRoot, SceneNode * snode);

	// **********************************************************************
	// * OgreCore::setAmbient(float x, float y, float z)			*
	// *									*
	// * Callable from cgkit.						*
	// * Sets the global ambient light color value.				*
	// **********************************************************************
	int setAmbient(float x, float y, float z);

	// **********************************************************************
	// * OgreCore::rebuildOgreTree(support3d::WorldObject& VisumRoot)	*
	// *									*
	// * Callable from cgkit.						*
	// * Whenever the cgkit scene changes completely, use this function	*
	// * to recreate the ogre scene graph. Don't forget to call setupData() *
	// * afterwards.							*
	// * This method is best used when you have different scenes in		*
	// * single files that change dynamically.				*
	// * TODO: Still crashes if camera is not set by setupData() ... 	*
	// **********************************************************************
	int rebuildOgreTree(support3d::WorldObject& VisumRoot);


	void changeBackgroundColor(int r, int g, int b);

	// **********************************************************************
	// * OgreCore::getMeshFromCgKit(Root *root, const char *name)		*
	// *									*
	// * Creates an ogre mesh from a cgkit TriMeshGeom type.		*
	// **********************************************************************
	Mesh* getMeshFromCgKit(support3d::TriMeshGeom *trigeom, const char *name);

	// **********************************************************************
	// * OgreCore::captureFrameBuffer()					*
	// *									*
	// * Callable from cgkit.						*
	// * Renders the frame buffer to a texture that is available to		*
	// * peviously specified material.					*
	// **********************************************************************
	void captureFrameBuffer();

	// **********************************************************************
	// * OgreCore::setupData(...)						*
	// *									*
	// * Callable from cgkit.						*
	// * Basically just for explicitly setting camera with its attributes,	*
	// * should be called after the OgreCore is initialized			*
	// **********************************************************************
	void setupData(support3d::WorldObject& camera, float nearclip, float farclip, float fov);
	
	// **********************************************************************
	// * OgreCore::setCaptureFrameBuffer(...)				*
	// *									*
	// * Callable from cgkit.						*
	// * Initializes "saving" the frame buffer to a texture, that is	*
	// * internally specified in material whichMaterial in technique	*
	// * whichTechnique and pass whichPass.					*
	// * Useful e.g. for post render effects, e.g. halftoning, blurring	*
	// * depth of field, etc...						*
	// * Actual rendering to the texture is done by captureFrameBuffer().	*
	// **********************************************************************
	int setCaptureFrameBuffer(std::string whichMaterial, int width, int height, unsigned short whichTechnique, unsigned short whichPass );

	// **********************************************************************
	// * OgreCore::setRenderOverlay(std::string overlayMaterial)		*
	// *									*
	// * Callable from cgkit.						*
	// * Enables the rendering of a screen filling quad right before the	*
	// * camera. This billboard is rendered with specified overlayMaterial	*
	// * and could be used for post filters, in combinations with the	*
	// * captureFrameBuffer function.					*
	// * TODO: Allow different sized overlays ...				*
	// **********************************************************************
	void setRenderOverlay(std::string overlayMaterial);

	// **********************************************************************
	// * OgreCore::createStaticPlane(...)					*
	// *									*
	// * Callable from cgkit.						*
	// * Creates a pure ogre plane type, this geometry is not		*
	// * handled by cgkit, only internally by ogre.				*
	// **********************************************************************
	int createStaticPlane(float nx, float ny, float nz, float d, float width, float height, \
				const char* name, const char* material, float px, float py, float pz, \
				float rx, float ry, float rz, int wu);

	// **********************************************************************
	// * OgreCore::removeStaticPlane(...)					*
	// *									*
	// * Callable from cgkit.						*
	// * Removes a plane created by createStaticPlane()			*
	// **********************************************************************
	int removeStaticPlane(const char* name);
	
	// **********************************************************************
	// * OgreCore::transformStaticPlane(...)				*
	// *									*
	// * Callable from cgkit.						*
	// * Transforms a plane created by createStaticPlane() 			*
	// **********************************************************************
	int transformStaticPlane(const char* name, float xp, float yp, float zp, float rx, float ry, float rz);

	// **********************************************************************
	// * OgreCore::switchVisibility()					*
	// *									*
	// * Callable from cgkit.						*
	// * Overlay must have been set before this method can be used.		*
	// * Changes visibility method that defines if the scene is renderd	*
	// * normally or only the previous set overlay can be seen.		*
	// **********************************************************************
	int switchVisibility();

	// **********************************************************************
	// * OgreCore::setCaptureVideo(int jaNein)				*
	// *									*
	// * Callable from cgkit.						*
	// * Enables/disabled video capturing.					*
	// **********************************************************************
	int setCaptureVideo(int jaNein);

	void checkShadowCastReceive();

	// these 5 event handling methods are only temporarily
	// they are not fine and will be replaced with ogre 1.0
	boost::python::list pumpKeyDowns();
	boost::python::list pumpMotions();
	boost::python::list pumpKeyUps();
	boost::python::list pumpMouseButtons();

	// Attributes
	//
	// vector for storing all dependent types
	std::vector<OgreDependent*> dependents;

private:
	// Attributes
	// 
	int mIsStereo;
	bool mCaptureVideo;
	bool mTextureShadows;
	int shadowFlags;
	int otherShadowFlags;

	int mLightCount;
	support3d::LightSource *mpLightList[20]; // careful, implicit max lights count

	struct ObjectList
	{
		boost::shared_ptr<support3d::WorldObject>	cgkitObject;
		Entity						*ogreObject;
	};
	int mObjectCount;
	ObjectList mpObjectList[100]; // careful, implicit max object count
	
	Overlay* mpOverlay;
	long mOldTime;
	int mFrameNumber;
	bool mGlobalVisibleFlag;
	int mDebugMode;
	MyMouseListener* mpMoti;
	MyListener* mpListi;
	MyMouseButtonListener* mpButi;
	EventProcessor * mpEvpro;
	bool mHaveToParse;
	bool mIsCaptureFrameBuffer;
	RenderTexture* mpRenderTexture;

	char* mpBuffer;
	std::string mMyString;
	std::string mMyStringLight;
	std::string mMyMaterialName;
	Viewport*  mpVp;

	boost::shared_ptr< support3d::Material > sptrMaterial;

	Resource* mpMyMaterial; 
	SceneNode* mpOverlayNode;
	SceneNode* mpOldSceneGraph;
   	bool mShadowsEnabled;
	Light* mpMyLight;
	Root *mpRoot;
	RenderWindow *mpWindow;
	Timer *mpTimer;
	Camera* mpCamera;
	Camera *mpCameraStereo;
	SceneManager *mpSceneMgr;

	// Methods
	//
	// **********************************************************************
	// * OgreCore::setRenderSystem(Root *root, const char *name)		*
	// *									*
	// * Choose a suitable render system, e.g. 				*
	// * "Direct3D9 Rendering SubSystem" or 				*
	// * "GL Rendering SubSystem".						*
	// **********************************************************************
	bool setRenderSystem(Root *root, const char *name);
};

#endif
