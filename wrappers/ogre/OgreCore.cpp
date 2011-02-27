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

// DEBUG Makro
#define DEBUGOUT(stri, integ) if (mDebugMode==1) cout << "DEBUG: " << stri << " " << integ << endl;
#define DEBUGVECTOROUT(stri, integ1,integ2,integ3) if (mDebugMode==1) cout << "DEBUG: " << stri << " " << integ1 << " " << integ2 << " " << integ3 << endl;

#include "OgreCore.h"

// **********************************************************************
// * class OgreDependent						*
// *									*
// * Ogre objects that are dependent from values in cgkit.		*
// **********************************************************************


// **********************************************************************
// * OgreDependent::OgreDependent()					*
// *									*
// * A constructor, sets relation between cgkit world object and a	*
// * Ogre scene node.							*
// **********************************************************************

OgreDependent::OgreDependent(support3d::WorldObject* wo, SceneNode* sn)
{
	// set cgkit node and Ogre scene node
	mpTransi = wo;
	mpNodi = sn;

	// identifies type of node
	mIsPointLight = false;
	mIsSpotLight = false;
	mIsDistantLight = false;
	mIsCamera = false;

	// if geometry, it usually has a material
	mMaterialName = "";
}


// **********************************************************************
// * OgreDependent::~OgreDependent()					*
// *									*
// * A standard destructor.						*
// **********************************************************************

OgreDependent::~OgreDependent()
{
	// empty
}


// **********************************************************************
// * void OgreDependent::onValueChanged()				*
// *									*
// * Called when a value is changed from cgkit side.			*
// * Needs to aupdate appropriate Ogre side values, depending		*
// * on the type of the node (e.g. light, geometry, camera, etc).	*
// **********************************************************************

void OgreDependent::onValueChanged()
{	
	// to store the nodes transformation
	support3d::vec3d changePos;
	support3d::vec3d changeScale;
	support3d::mat4d changeRot;

	// check visible, applicable for all node types
	mpNodi->setVisible( ( ( mpTransi->visible ).getValue() ), false );

	// get local transformation of the node
	support3d::mat4d changeTransformation = mpTransi->localTransform();
	changeTransformation.decompose(changePos, changeRot, changeScale);

	// orientation in quaternions
	support3d::quat<double> rotquat;
	rotquat.fromMat(changeRot);
	
	// set new transformation
	mpNodi->setPosition( changePos.x, changePos.y, changePos.z );
	mpNodi->setOrientation (rotquat.w, rotquat.x, rotquat.y, rotquat.z);

	// camera should not be scaled and never shown (its frustum)
	if(mIsCamera)
	{
		mpNodi->setVisible(false, false);
		mpNodi->yaw( Degree(180.0) );
	}
	else if(!mIsPointLight)
	{
		mpNodi->setScale( changeScale.x, changeScale.y, changeScale.z );
	}

	// special tasks for pointlights
	if (mIsPointLight)
	{
		// some error checking
		support3d::GLPointLight* GLp = dynamic_cast<support3d::GLPointLight*>(mpTransi);
		if (GLp == 0)
			cout << "FORC: ERROR: GLp is null" << endl;
		if (mpNodi->numAttachedObjects() > 1)
			cout << "FORC: ERROR: Too many objects on one scenenode" << endl; 
		if (mpTheLight == 0)
			cout << "FORC: ERROR: Ogre light is not present" << endl;

		// update point light colors
		float intense = (GLp->intensity).getValue();
		support3d::vec3d diff = (GLp->diffuse).getValue();
		support3d::vec3d spec = (GLp->specular).getValue();
		mpTheLight->setDiffuseColour(ColourValue(diff.x, diff.y, diff.z)*intense);
		mpTheLight->setSpecularColour(ColourValue(spec.x, spec.y, spec.z)*intense);
		
		// TODO: find a decent range default rvalue
		// set attenuation for the point light
        	mpTheLight->setAttenuation(1000, (GLp->constant_attenuation).getValue(), \
				(GLp->linear_attenuation).getValue(), (GLp->quadratic_attenuation).getValue() );

		// Cgkits "enabled" translates to Ogres "visible"
		mpTheLight->setVisible((GLp->enabled).getValue());

		// check for shadow casting
		mpTheLight->setCastShadows((GLp->cast_shadow).getValue());

		// TODO: remove HACK
		mpTheLight->setCastShadows(true);
	}
	
	// special tasks for distantlights
	else if (mIsDistantLight)
	{
		// some error checking
		support3d::GLDistantLight* GLd = dynamic_cast<support3d::GLDistantLight*>(mpTransi);
		if (GLd == 0)
			cout << "FORC: ERROR: GLd is null" << endl;
		if (mpNodi->numAttachedObjects() > 1)
			cout << "FORC: ERROR: Too many objects on one scenenode" << endl; 
		if (mpTheLight == 0)
			cout << "FORC: ERROR: Ogre light is not present" << endl;

		// update point light colors
		float intense = (GLd->intensity).getValue();
		support3d::vec3d diff = (GLd->diffuse).getValue();
		support3d::vec3d spec = (GLd->specular).getValue();
		mpTheLight->setDiffuseColour(ColourValue(diff.x, diff.y, diff.z)*intense);
		mpTheLight->setSpecularColour(ColourValue(spec.x, spec.y, spec.z)*intense);
		
		// Cgkits "enabled" translates to Ogres "visible"
		mpTheLight->setVisible((GLd->enabled).getValue());

		// check for shadow casting
		mpTheLight->setCastShadows((GLd->cast_shadow).getValue());

		// TODO: remove HACK
		mpTheLight->setCastShadows(true);		
	}
	
	// special tasks for spotlights
	else if (mIsSpotLight)
	{
		support3d::GLSpotLight* GLs = dynamic_cast<support3d::GLSpotLight*>(mpTransi);
		if (GLs == 0)
			cout << "FORC: ERROR: GLs is null" << endl;
		if (mpNodi->numAttachedObjects() > 1)
			cout << "FORC: ERROR: Too many objects on one scenenode" << endl; 
		if (mpTheLight == 0)
			cout << "FORC: ERROR: Ogre light is not present" << endl;

		// update point light colors
		float intense = (GLs->intensity).getValue();
		support3d::vec3d diff = (GLs->diffuse).getValue();
		support3d::vec3d spec = (GLs->specular).getValue();
		mpTheLight->setDiffuseColour(ColourValue(diff.x, diff.y, diff.z)*intense);
		mpTheLight->setSpecularColour(ColourValue(spec.x, spec.y, spec.z)*intense);

		// TODO: find a decent range default value
		mpTheLight->setAttenuation(1000, (GLs->constant_attenuation).getValue(), \
				(GLs->linear_attenuation).getValue(), (GLs->quadratic_attenuation).getValue() );

		// Cgkits "enabled" translates to Ogres "visible"
		mpTheLight->setVisible((GLs->enabled).getValue());
		
		// TODO: got the right values for spotlight range?
		mpTheLight->setSpotlightRange( Degree(0.0), Degree( 2.0 * ( (GLs->cutoff).getValue() ) ), (GLs->exponent).getValue() ); 

		// check for shadow casting
		mpTheLight->setCastShadows((GLs->cast_shadow).getValue());

		// TODO: remove HACK
		mpTheLight->setCastShadows(true);		
	}
} // onValueChanged()


// **********************************************************************
// * class OgreCore							*
// *									*
// * Encapsulates needed OGRE support for cgkit.			*
// **********************************************************************


// **********************************************************************
// * OgreCore::OgreCore()						*
// *									*
// * Constructor, some standard initialization.				*
// **********************************************************************

OgreCore::OgreCore()
{
	mHaveToParse = true;
	mCaptureVideo = false;
	mIsStereo = 0;
	shadowFlags = -1;
	mLightCount = 0;
	mObjectCount = 0;
}


// **********************************************************************
// * OgreCore::~OgreCore()						*
// *									*
// * Destructor, release input handling, dependencies and currently	*
// * allocated memory.							*
// * TODO: Visible flag is nor treated as dependency ...		*
// **********************************************************************

OgreCore::~OgreCore()
{
      	cout << "FORC: OgreCore Destructor" << endl;

	// stop processing events
	mpEvpro->stopProcessingEvents();
	mpEvpro->removeMouseMotionListener(mpMoti);
	mpEvpro->removeMouseListener(mpButi);;
	mpEvpro->removeKeyListener(mpListi); 

	// Remove event listeners
	if (mpMoti!=0) delete mpMoti;
	if (mpListi!=0) delete mpListi;
	if (mpButi!=0) delete mpButi;
	if (mpEvpro!=0) delete mpEvpro;
	DEBUGOUT("EventProcessor successfully removed.",mpEvpro);

	// TODO: DER VISIBLE FLAG WIRD NICHT ALS DEPENDENT BEHANDELT, ÜBERALL ANPASSEN!
	
	// Remove dependents  
      	unsigned int h=0;
	unsigned int max = dependents.size();
      	for ( h=0; h < max; h++ )
      	{
	        // camera
        	if (dependents[h]->mIsCamera == true)
		{
        		cout << "FORC: Removing Camera" << endl;
              		(dependents[h]->mpTransi->transform).removeDependent(dependents[h]);
		}
		// distant light
		else if (dependents[h]->mIsDistantLight == true)
		{ 
 	             	cout << "FORC: Removing DistantLight" << endl;
	                (dependents[h]->mpTransi->transform).removeDependent(dependents[h]);
			( static_cast<support3d::GLDistantLight*>(dependents[h]->mpTransi) )->enabled.removeDependent(dependents[h]);
			( static_cast<support3d::LightSource*>(dependents[h]->mpTransi) )->intensity.removeDependent(dependents[h]);
			( static_cast<support3d::GLDistantLight*>(dependents[h]->mpTransi) )->diffuse.removeDependent(dependents[h]);
			( static_cast<support3d::GLDistantLight*>(dependents[h]->mpTransi) )->specular.removeDependent(dependents[h]);
			( static_cast<support3d::GLDistantLight*>(dependents[h]->mpTransi) )->cast_shadow.removeDependent(dependents[h]);
		}
		// spot light
		else if (dependents[h]->mIsSpotLight == true)
		{
	                cout << "FORC: Removing SpotLight" << endl;
	                (dependents[h]->mpTransi->transform).removeDependent(dependents[h]);
			( static_cast<support3d::GLSpotLight*>(dependents[h]->mpTransi) )->enabled.removeDependent(dependents[h]);
			( static_cast<support3d::LightSource*>(dependents[h]->mpTransi) )->intensity.removeDependent(dependents[h]);
			( static_cast<support3d::GLSpotLight*>(dependents[h]->mpTransi) )->diffuse.removeDependent(dependents[h]);
			( static_cast<support3d::GLSpotLight*>(dependents[h]->mpTransi) )->specular.removeDependent(dependents[h]);
			( static_cast<support3d::GLSpotLight*>(dependents[h]->mpTransi) )->cast_shadow.removeDependent(dependents[h]);
	
			( static_cast<support3d::GLSpotLight*>(dependents[h]->mpTransi) )->constant_attenuation.removeDependent(dependents[h]);
			( static_cast<support3d::GLSpotLight*>(dependents[h]->mpTransi) )->linear_attenuation.removeDependent(dependents[h]);
			( static_cast<support3d::GLSpotLight*>(dependents[h]->mpTransi) )->quadratic_attenuation.removeDependent(dependents[h]);
			( static_cast<support3d::GLSpotLight*>(dependents[h]->mpTransi) )->exponent.removeDependent(dependents[h]);
			( static_cast<support3d::GLSpotLight*>(dependents[h]->mpTransi) )->cutoff.removeDependent(dependents[h]);
		}
		// point light
		else if (dependents[h]->mIsPointLight == true)
		{
			cout << "FORC: Removing PointLight" << endl;
			(dependents[h]->mpTransi->transform).removeDependent(dependents[h]);
			( static_cast<support3d::GLPointLight*>(dependents[h]->mpTransi) )->enabled.removeDependent(dependents[h]);
			( static_cast<support3d::LightSource*>( dependents[h]->mpTransi) )->intensity.removeDependent(dependents[h]);
			( static_cast<support3d::GLPointLight*>(dependents[h]->mpTransi) )->diffuse.removeDependent(dependents[h]);
			( static_cast<support3d::GLPointLight*>(dependents[h]->mpTransi) )->specular.removeDependent(dependents[h]);
			( static_cast<support3d::GLPointLight*>(dependents[h]->mpTransi) )->cast_shadow.removeDependent(dependents[h]);
	
			( static_cast<support3d::GLPointLight*>(dependents[h]->mpTransi) )->constant_attenuation.removeDependent(dependents[h]);
			( static_cast<support3d::GLPointLight*>(dependents[h]->mpTransi) )->linear_attenuation.removeDependent(dependents[h]);
			( static_cast<support3d::GLPointLight*>(dependents[h]->mpTransi) )->quadratic_attenuation.removeDependent(dependents[h]);
		}
	        // no light, no camera, so standard object with transform and visibility dependency only     
	        else 
		{
	  	        cout << "FORC: Removing misc. dependent" << endl;
	   	        (dependents[h]->mpTransi->transform).removeDependent(dependents[h]);
			(dependents[h]->mpTransi->visible).removeDependent(dependents[h]);
		}
			
		// Finally, remove the dependent from memory
		DEBUGOUT("... Removing:",h);
	       	delete dependents[h];

	} // end for

	// Shut down main ogre rendering system
	if (mpRoot != 0)
	{
		cout << "FORC: deleting OGRE ..." << endl;
		try
		{
			delete mpRoot;
			mpRoot=0;
		}
		catch(...)
		{
			cout << "FORC: Error while closing OGRE" << endl;
		}
	}
	cout << "FORC: Finished" << endl;
} // OgreCore::~OgreCore()


// **********************************************************************
// * OgreCore::clock(unsigned int fps)					*
// *									*
// * Callable from cgkit.						*
// * Called once in a frame, to hold the current frame rate.		*
// **********************************************************************

void OgreCore::clock(unsigned int fps)
{
	while( (mpTimer->getMilliseconds() - mOldTime) < 1000.0f/fps )
	{
		// waiting ...
	}
	// update old time stamp
	mOldTime=mpTimer->getMilliseconds();
}


// **********************************************************************
// * OgreCore::setupData(...)						*
// *									*
// * Callable from cgkit.						*
// * Basically just for explicitly setting camera with its attributes,	*
// * should be called after the OgreCore is initialized			*
// **********************************************************************

void OgreCore::setupData(support3d::WorldObject& cam, float nearclip, float farclip, float fov)
{
	// set camera attributes
	mpCamera->setNearClipDistance(nearclip);
	mpCamera->setFarClipDistance(farclip);
	mpCamera->setFOVy(Degree( fov ));

	// attach camera to a scene node for controlling
	SceneNode * snode = static_cast<SceneNode*>( mpSceneMgr->getRootSceneNode()->createChildSceneNode() );
	snode->attachObject(mpCamera);

	// create a dependency for cgkit side modifications
	OgreDependent *testdependent = new OgreDependent( &cam, snode );
	testdependent->mIsCamera = true;
	testdependent->mpTheCamera = mpCamera;
	(cam.transform).addDependent(testdependent);
	dependents.push_back(testdependent);

	// When stereo rendering, adjust second camera
	if (mIsStereo == 1)
	{
		// set camera attributes
		mpCameraStereo->setNearClipDistance(nearclip);
		mpCameraStereo->setFarClipDistance(farclip);
		mpCameraStereo->setFOVy(Degree( fov ));

		// attach camera to a scene node ... child of the first camera
		SceneNode *snode_stereo = static_cast<SceneNode*>( snode->createChildSceneNode() );
		snode_stereo->attachObject(mpCameraStereo);		
	}
}


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

void OgreCore::insertToTree(support3d::WorldObject& newObject, support3d::WorldObject& fatherObject, int hasFather)
{
	// TODO!
}


// **********************************************************************
// * OgreCore::changeMaterial(...)					*
// *									*
// * Callable from cgkit.						*
// * When the material changes after the ogre scene graph is created,	*
// * use this method to set the new material for the entityName 	*
// * (that is also the name of the cgkit node).				*
// * TODO: Make materials scene node dependent or mesh dependent ...	*
// **********************************************************************

void OgreCore::changeMaterial(std::string newMaterial, std::string entityName)
{
	Resource* theMaterial = 0;

	// Parse sources if you haven't already
	if (mHaveToParse)
	{
		// TODO: Replace this call with sth equivalent
		//MaterialManager::getSingleton().parseAllSources();
		mHaveToParse = false;
	}
	try
	{
		// Change material
		theMaterial = MaterialManager::getSingleton().getByName(newMaterial).get();
		if (theMaterial != 0)
		{
			theMaterial->load();
			(mpSceneMgr->getEntity(entityName))->setMaterialName(newMaterial);
		}
		else
		{
			cout << "FORC: Specified material does not exist!" << endl;
		}
	}
	catch(...)
	{
		cout << "FORC: Material could not be assigned" << endl;
	}
}


// **********************************************************************
// * OgreCore::buildTree(...)						*
// *									*
// * Callable from cgkit.						*
// * Recursive method, that created the ogre scene node tree		*
// * as well as all dependents to the cgkit world.			*
// **********************************************************************

void OgreCore::buildTree(support3d::WorldObject& VisumRoot, SceneNode *snode)
{
	// Get an iterator
	support3d::WorldObject::ChildIterator childit = VisumRoot.childsBegin();
	
    	DEBUGOUT( "Calling buildTree with scene node at", snode );

	// Recursive exit condition: there are no children
	if (childit->second.get() == 0)
	{
		return;
	}	

	// There are children, loop through and create the according scene nodes
	while ( childit != VisumRoot.childsEnd() )
	{
		// Some needed pointers
		SceneNode *msnode = 0;
		Entity *ent = 0;
		support3d::LightSource *p_ls = 0;
		p_ls = dynamic_cast<support3d::LightSource*>( childit->second.get() );

		cout << "FORC: ---------------- Possible node identified: " << childit->second.get()->getName() << " " << (*childit).second.get() << endl;

		// **********************************************************************
		// * Case 1								*
		// *									*
		// * Geometry								*
		// **********************************************************************
		
		if ((*childit).second->getGeom().get() != 0)
		{
			// Some outputs
			mMyString=childit->second->getName();
			DEBUGOUT( ":::::::::::::::: New geometry found ::::::::::::::::::::::", (*childit).second->getGeom().get() );
			DEBUGOUT( childit->second->getName(), (*childit).second->getGeom().get() );

			// Check if a trimesh
			support3d::TriMeshGeom *tri_geom = 0;
			tri_geom = dynamic_cast<support3d::TriMeshGeom*>( (*childit).second->getGeom().get() );
			if ( tri_geom != 0 )
			{
                		DEBUGOUT( "New geometry is a TriMesh", (*childit).second->getGeom().get() );
				
				// Create an ogre mesh based on the TriMesh
				Mesh *trimesh = 0;
				trimesh = getMeshFromCgKit( tri_geom, (mMyString+"tm").c_str() );
				DEBUGOUT( "Created individual OgreMesh", trimesh );

				// Create an ogre entity based on the ogre mesh
				ent = mpSceneMgr->createEntity(mMyString, trimesh->getName());
				DEBUGOUT( "Entity created", ent );

				// Get a child scene node
				msnode = static_cast<SceneNode*>( snode->createChildSceneNode() );
				

				DEBUGOUT( "Created new SceneNode", msnode );
				
			mpObjectList[mObjectCount].cgkitObject=(*childit).second;
			mpObjectList[mObjectCount].ogreObject=ent;
			mObjectCount++;
			cout << "Current Object Count is: " << mObjectCount << endl;
			
			}
			else // Not a trimesh
			{
                		DEBUGOUT( "New geometry is not a TriMesh", (*childit).second->getGeom().get() );
				support3d::TriMeshGeom *trim = 0;
				trim = new support3d::TriMeshGeom();
				Mesh *trimesh = 0;
				DEBUGOUT( "Starting converting to a TriMesh", (*childit).second->getGeom().get() );					
				
				// Not a TriMesh, so try to convert to a supported TriMesh type
				try
				{
					// Convert
					(childit->second->getGeom().get())->convert(trim);
					DEBUGOUT( "Able to convert the geometry at least", trimesh );
					// Create an ogre mesh based on the TriMesh
					trimesh = getMeshFromCgKit( trim, (mMyString+"tm").c_str() );

					DEBUGOUT( "Conversion was successful", trimesh );	
				}
				catch(...) // Could not convert
				{
					cout << "FORC: Conversion to TriMesh is not possible .. creating plane" << endl;
					// TODO: Insert plane creation
					//trimesh = MeshManager::getSingletonPtr()->createPlane( (mMyString+"tm").c_str(), (mMyString+"gm").c_str(), \
					//	      (mMyString+"nm").c_str(), Plane( Vector3(0.0,0.0,-1.0), 1.0 ), 1.0, 1.0);

				}

				// Other steps as above
				ent = mpSceneMgr->createEntity(mMyString, trimesh->getName());
				msnode = static_cast<SceneNode*>( snode->createChildSceneNode() );

			mpObjectList[mObjectCount].cgkitObject=(*childit).second;
			mpObjectList[mObjectCount].ogreObject=ent;
			mObjectCount++;
			cout << "Current Object Count is: " << mObjectCount << endl;				

				// clean up
				delete trim;
			}
			
			// Now search for materials
			DEBUGOUT( "Searching for assigned materials for Mesh at", msnode );
		       	sptrMaterial = childit->second->getMaterial();
			
			// Is a material specified?
			if ( sptrMaterial.get() == 0 )
			{
				cout << "FORC: Warning: No specified materials found, using standard bright white" << endl;
				DEBUGOUT( "No materials assigned for Mesh at", msnode );
			}
			else
			{
				mMyMaterialName = ( (*(sptrMaterial.get())).getName() );
				DEBUGOUT( mMyMaterialName, msnode );

				// Check if you have to parse the specified file system paths first
				if (mHaveToParse)
				{
					DEBUGOUT( "Parsing, only done once", msnode );
					// TODO: Replace this call with sth equivalent
					//MaterialManager::getSingleton().parseAllSources();
					mHaveToParse = false;
				}
				try
				{
					DEBUGOUT( "Searching material", msnode );
					mpMyMaterial = MaterialManager::getSingleton().getByName(mMyMaterialName).get();
					cout << "The Name is ..." << mMyMaterialName << " " << endl;

					// Assign the material to the entity
					if (mpMyMaterial != 0)
					{
						cout << "Specified Material could is not zero, grrrrrrrrrrrrrrrrrr" << endl;
						cout << "The Name is ..." << mMyMaterialName << " " << endl;
						mpMyMaterial->load();
						DEBUGOUT( "Loding it", mpMyMaterial );
						ent->setMaterialName( mMyMaterialName );
						DEBUGOUT( "Setting it on entity", mpMyMaterial );
					}
					else
					{
						// leave white blank material
						cout << "Specified Material could not be loaded, grrrrrrrrrrrrrrrrrr" << endl;
					}
				}
				catch(...) // Could not assign the material
				{
					cout << "FORC, Warning: Could not find specified Material: " << mMyMaterialName << endl;
				}
			} // end else sptrMaterial.get() != 0

			// Attach the Object to the created scene node
			DEBUGOUT( "Attaching entity to SceneNode (Entity)", ent );
			if (msnode != 0)
				msnode->attachObject(ent);
			else
				cout << "FORC: |||||||||||||||||||||| Error: Scene node could not be created" << endl;

			// Create a dependent for the new geometry, basically just transformations and visibility
			DEBUGOUT( "Creating new dependent", msnode );
			OgreDependent* testdependent = 0;
			testdependent = new OgreDependent( (*childit).second.get(), msnode );

			// Set the material name in the dependend object for faster access later
			// TODO: Not consistent, when OgreCore::ChangeMaterial is called, still to correct
			if (mpMyMaterial != 0)
				testdependent->mMaterialName = mMyMaterialName;
			((*childit).second->transform).addDependent(testdependent);
			((*childit).second->visible).addDependent(testdependent);

			// Store the new dependent in a our dependents vector
			dependents.push_back(testdependent);

		} // end if geom != 0

		// **********************************************************************
		// * Case 2								*
		// *									*
		// * Light Sources							*
		// **********************************************************************

		else if (p_ls != 0)
		{
			mpLightList[mLightCount] = p_ls;
			mLightCount++;
			cout << "Current Light Source Count is: " << mLightCount << endl;

				
			// supported light types
			support3d::LightSource* tmpSource = 0;
			support3d::GLDistantLight* tmpDist = 0;
			support3d::GLPointLight* tmpPoint = 0;
			support3d::GLSpotLight* tmpSpot = 0;

			// check for light type
			tmpDist = dynamic_cast<support3d::GLDistantLight*>( childit->second.get());
			tmpPoint = dynamic_cast<support3d::GLPointLight*>( childit->second.get());
			tmpSpot = dynamic_cast<support3d::GLSpotLight*>( childit->second.get());

			// safe, as you already are in correct if case
			tmpSource = static_cast<support3d::LightSource*>( childit->second.get());

			// start light names with "l"
			mMyStringLight.append("l");

			// temporary give "wrong" scene node, override it later
			OgreDependent* testdependent = new OgreDependent( tmpSource, msnode );

			// create a new Ogre light and attach it to a scene node
			mpMyLight = 0;
			mpMyLight = mpSceneMgr->createLight(mMyStringLight);
			msnode = 0;
			msnode = static_cast<SceneNode*>( snode->createChildSceneNode() );
			msnode->attachObject(mpMyLight);

			DEBUGOUT(":::::::::::::::: Light Detected :::::::::::::::::::::", tmpSource);

			// important: override the scene node pointer
			testdependent->mpNodi = msnode;
			
			// create a standard transformation dependency
			((*childit).second->transform).addDependent(testdependent);			
			tmpSource->enabled.addDependent(testdependent);	
			tmpSource->intensity.addDependent(testdependent);

			// shadow dependency
			tmpSource->cast_shadow.addDependent(testdependent);

			// we have a point light
			if (tmpPoint != 0)
			{
				// modify ogre light type
				mpMyLight->setType( Ogre::Light::LT_POINT );
				testdependent->mpTheLight = mpMyLight;
				testdependent->mpTheLight->setType( Ogre::Light::LT_POINT );

				// set dependent type -- very important
				testdependent->mIsPointLight = true;

				// light depends on which attributes?
				tmpPoint->diffuse.addDependent(testdependent);
				tmpPoint->specular.addDependent(testdependent);
				tmpPoint->constant_attenuation.addDependent(testdependent);
				tmpPoint->linear_attenuation.addDependent(testdependent);
				tmpPoint->quadratic_attenuation.addDependent(testdependent);
				
				// finally set correct values
				testdependent->onValueChanged();
			}
			// we have a distant light
			else if (tmpDist != 0)
			{
				// modify ogre light type
				mpMyLight->setType( Ogre::Light::LT_DIRECTIONAL );
				testdependent->mpTheLight = mpMyLight;
				testdependent->mpTheLight->setType( Ogre::Light::LT_DIRECTIONAL );
				
				// set dependent type -- very important
				testdependent->mIsDistantLight = true;

				// light depends on which attributes?
				tmpDist->diffuse.addDependent(testdependent);
				tmpDist->specular.addDependent(testdependent);	
				
				// finally set correct values
				testdependent->onValueChanged();
			}
			// we have a spot light
			else if (tmpSpot != 0)
			{
				// modify ogre light type
				mpMyLight->setType( Ogre::Light::LT_SPOTLIGHT );
				testdependent->mpTheLight = mpMyLight;
				testdependent->mpTheLight->setType( Ogre::Light::LT_SPOTLIGHT );
				
				// set dependent type -- very important
				testdependent->mIsSpotLight = true;

				// light depends on which attributes?
				tmpSpot->diffuse.addDependent(testdependent);
				tmpSpot->specular.addDependent(testdependent);	
				tmpSpot->constant_attenuation.addDependent(testdependent); 
				tmpSpot->linear_attenuation.addDependent(testdependent);
				tmpSpot->quadratic_attenuation.addDependent(testdependent); 
				tmpSpot->exponent.addDependent(testdependent);	
				tmpSpot->cutoff.addDependent(testdependent);	

				// finally set correct values
				testdependent->onValueChanged();
			}
			// We have an unknown light type (e.g. 3ds light sources)
			else
			{
				cout << "FORC: Error: The light type is not supported by Ogre, verify the light type" << endl;
			}

			// Store the fresh light into our dependents list
			dependents.push_back(testdependent);

		} // end if light source

		// **********************************************************************
		// * Case 3								*
		// *									*
		// * Unknown objects, e.g. empty father nodes				*
		// **********************************************************************
		
		else
		{
			DEBUGOUT( ":::::::::::: Unidentifiable node detected ::::::::::::::", childit->second.get() );
			msnode = static_cast<SceneNode*>( snode->createChildSceneNode() );
			
			// Create a new dependent
			OgreDependent *testdependent = new OgreDependent( (*childit).second.get(), msnode );

			// Some outputs
			DEBUGOUT( "Perhaps an empty father: new dependent at", testdependent );
			DEBUGOUT( childit->second.get()->getName(), childit->second.get() );

			// Make it transform dependent and visible dependent
			((*childit).second->transform).addDependent(testdependent);
			((*childit).second->visible).addDependent(testdependent);

			// Finally, store it in our dependents vector
			dependents.push_back(testdependent);
		} // end else unknown node

		// Recursion: do the same with each child node
		buildTree( *(childit->second), msnode);

		// Back from recursion
		DEBUGOUT( "Back from Recursion, incrementing iterator", &childit );
		childit++;
	} // end while 

	// Scene graph was sucessfully created
	DEBUGOUT( "fully built one leaf", 111 );
}

// **********************************************************************
// * boost::python::list OgreCore::checkShadowCastReceive()		*
// *									*
// **********************************************************************

void OgreCore::checkShadowCastReceive()
{
	int i,j;

	cout << " |||||||| ... checking the shadow cast/receive thingy" << endl;

	// TODO: some light priority checking/sorting ...
	//
	for(i=0;i<mObjectCount;i++)
	{
		for(j=0; j<mLightCount;j++)
		{
			cout << " |||||||| ... lightsource # " << j << endl;
				
			if ( mpLightList[j]->isShadowCaster(mpObjectList[i].cgkitObject) )
			{
				cout << " |||||||| ... is shadow caster" << endl;
				mpObjectList[i].ogreObject->setCastShadows(true);
			}
			else if ( mpLightList[j]->isShadowReceiver(mpObjectList[i].cgkitObject) )
			{
				cout << " |||||||| ... is shadow receiever" << endl;
				mpObjectList[i].ogreObject->setCastShadows(false);
			}
			else
			{
				cout << " |||||||| ... is default: casts shadows" << endl;
				mpObjectList[i].ogreObject->setCastShadows(true);
			}			
		}	
	}
}

// **********************************************************************
// * boost::python::list OgreCore::pumpKeyDowns()			*
// *									*
// * Callable from cgkit.						*
// * Pumps key down events to cgkit side.				*
// **********************************************************************

boost::python::list OgreCore::pumpKeyDowns()
{
	boost::python::list li;
      	int i=0;
	for (i=0; i < mpListi->pressedCount; i++)
	{
		li.append( mpListi->keyCharList[i] );
		li.append( mpListi->keyIntList[i] );
		li.append( mpListi->keyModifiersList[i] );
	}
	mpListi->flush();
	return li;
}


// **********************************************************************
// * boost::python::list OgreCore::pumpKeyUps()				*
// *									*
// * Callable from cgkit.						*
// * Pumps key release events to cgkit side.				*
// **********************************************************************

boost::python::list OgreCore::pumpKeyUps()
{
	boost::python::list li;
	int i=0;
	for (i=0; i < mpListi->releasedCount; i++)
	{
		li.append( mpListi->keyCharListUp[i] );
		li.append( mpListi->keyIntListUp[i] );
		li.append( mpListi->keyModifiersListUp[i] );
	}
	mpListi->flushUp();
	return li;
}


// **********************************************************************
// * boost::python::list OgreCore::pumpMouseButtons()			*
// *									*
// * Callable from cgkit.						*
// * Pumps mouse button downs to cgkit side.				*
// **********************************************************************

boost::python::list OgreCore::pumpMouseButtons()
{
	boost::python::list li;
	int i=0;
	for (i=0; i < mpButi->mouseCount; i++)
	{
		li.append( mpButi->mouseButtonList[i] );
		li.append( mpButi->eventTypeList[i] );
		li.append( mpButi->mouseXList[i] );
		li.append( mpButi->mouseYList[i] );
		li.append( mpButi->mousedXList[i] );
		li.append( mpButi->mousedXList[i] );
	}
	mpButi->flush();
	return li;
}


// **********************************************************************
// * boost::python::list OgreCore::pumpMotions()			*
// *									*
// * Callable from cgkit.						*
// * Pumps mouse button motions to cgkit side.				*
// **********************************************************************

boost::python::list OgreCore::pumpMotions()
{
	boost::python::list li2;

	// Check if motion happened
	if(! mpMoti->motion )
		return li2;

	// New coordinates
	li2.append(mpMoti->moveX);
	li2.append(mpMoti->moveY);
	li2.append(mpMoti->moveRelX);
	li2.append(mpMoti->moveRelY);

	// New mouse buttons
	boost::python::list li2a;
	if (mpMoti->lMB)
		li2a.append(1);
	else
		li2a.append(0);
	if (mpMoti->mMB)
		li2a.append(1);
	else
		li2a.append(0);
	if (mpMoti->rMB)
		li2a.append(1);
	else
		li2a.append(0);

	li2.append(li2a);

	// Reset motion state
	mpMoti->motion = false;
	return li2;
}


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

void OgreCore::initialize(support3d::WorldObject& VisumRoot, unsigned int windowW, \
	       	unsigned int windowH, bool fullscr, bool shadows, bool opengl, const char * materialpath, int stereo)
{
	// initialize vector that stores all cgkit-ogre dependencies
	dependents.resize(50);
	dependents.clear();

	// some standard values
	mIsCaptureFrameBuffer = false;
	mGlobalVisibleFlag = true;
	mpOverlayNode = 0;
	//mDebugMode = dm;
	mIsStereo = stereo;

	if (shadowFlags == -1)
		cout << "WARNING: MAKE A SHADOW SETUP FIRST!" << endl;
	
	try {
		mpRoot = 0;
		mpRoot = new Root();

		// loading most important plugins
		//#if OGRE_PLATFORM == PLATFORM_WIN32
		mpRoot->loadPlugin("RenderSystem_Direct3D9");
		//#include "windows.h"
		//#endif
		mpRoot->loadPlugin("RenderSystem_GL");
		//mpRoot->loadPlugin("Plugin_FileSystem");
		mpRoot->loadPlugin("Plugin_BSPSceneManager");
		mpRoot->loadPlugin("Plugin_OctreeSceneManager");
		//mpRoot->loadPlugin("Plugin_GuiElements");
		mpRoot->loadPlugin("Plugin_CgProgramManager"); 

		
		// Set specified rendering subsystem
		if (!opengl) // directX
		{
			if (!setRenderSystem(mpRoot, "Direct3D9 Rendering SubSystem"))
				throw( Exception(0, "Specified Rendering System is not available!", "setRenderSystem"));
		}
		else // openGL
		{
			if (!setRenderSystem(mpRoot, "OpenGL Rendering Subsystem"))
				throw( Exception(0, "Specified Rendering System is not available!", "setRenderSystem"));
		}

		// Window
		mpRoot->initialise(false);
		mpWindow = mpRoot->createRenderWindow("myWindow", windowW, windowH, fullscr);

		// Timer
		mpTimer = 0;
		mpTimer = mpRoot->getTimer();

		// Scene manager, very important
		mpSceneMgr = mpRoot->getSceneManager(ST_GENERIC); 

		// Setup shadow technique
		// UPDATE:
		// 	Ignore shadow parameter as there
		// 	is now an unique function for doing this
		
		if (shadowFlags != -1)
		{
			mTextureShadows = false;
			if (shadowFlags == 0)
			{
		               	mShadowsEnabled = true;
				cout<< " *S*H*A*D*O*W*S*************************** SHADOWTYPE_STENCIL_MODULATIVE" << endl;
				mpSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_MODULATIVE);
				MeshManager::getSingleton().setPrepareAllMeshesForShadowVolumes(true);
				shadowFlags = 0;
			}	
			if (shadowFlags == 1)
			{
		               	mShadowsEnabled = true;
				cout<< " *S*H*A*D*O*W*S*************************** SHADOWTYPE_STENCIL_ADDITIVE" << endl;
				mpSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);
				MeshManager::getSingleton().setPrepareAllMeshesForShadowVolumes(true);
				shadowFlags = 1;
			}	
			if (shadowFlags == 2)
			{
		               	mShadowsEnabled = true;
				cout<< " *S*H*A*D*O*W*S*************************** SHADOWTYPE_TEXTURE_MODULATIVE" << endl;
				mpSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
				mTextureShadows = true;
				mpSceneMgr->setShadowTextureSize(otherShadowFlags);
				shadowFlags = 2;
			}
		}
		
		
		// Create camera and viewport, background color
		if (stereo == 0)
		{
			mpCamera = mpSceneMgr->createCamera("PlayerCam");		
			mpVp = mpWindow->addViewport(mpCamera);
			mpVp->setBackgroundColour(ColourValue(0.5,0.5,0.5));
		}
		else // stereo == 1
		{
			// Need two cameras, two viewports
			mpCamera = mpSceneMgr->createCamera("PlayerCam");
			mpCamera->setAspectRatio((float)windowW/2.0 / (float)windowH);
			mpCameraStereo = mpSceneMgr->createCamera("StereoCam");
			mpCameraStereo->setAspectRatio((float)windowW/2.0 / (float)windowH);

			mpVp = mpWindow->addViewport(mpCamera, 0, 0.0f, 0.0f, 0.5f, 1.0f);
			mpVp->setBackgroundColour(ColourValue(0.5,0.5,0.5));

			Viewport *viewport_stereo = mpWindow->addViewport(mpCameraStereo, 1, 0.5f, 0.0f, 0.5f, 1.0f);
			viewport_stereo->setBackgroundColour(ColourValue(0.5,0.5,0.5));
		}			

//		mpCamera->setDetailLevel (SDL_WIREFRAME);
		
		// Set file system path
		try
		{
			mpRoot->addResourceLocation(materialpath, "FileSystem");

			ResourceGroupManager::getSingleton().initialiseResourceGroup("General");
			cout << "FILE SYSTEM IS SET ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^!" << endl;
		}
		catch(...)
		{
			mpRoot->addResourceLocation(".", "FileSystem");

			ResourceGroupManager::getSingleton().initialiseResourceGroup("General");
			cout << "Error while setting Material Path!" << endl;
		}
		mHaveToParse = true;
		
		// standard ambient light, can be overridden by special method, available from cgkit side
		mpSceneMgr->setAmbientLight(ColourValue(0.2,0.2,0.2));


		// **********************************************************************
		// * Build the ogre Scene Node Tree					*
		// *									*
		// **********************************************************************

		// Get a standard root scene node, on which the whole cgkit tree will be based on
		SceneNode *snode = static_cast<SceneNode*>( mpSceneMgr->getRootSceneNode()->createChildSceneNode() );

		// Build it
		cout << "build the tree" << endl;
		mMyString = "o";
		DEBUGOUT("Built a new SceneNode in initialize", snode );
		buildTree(VisumRoot, snode);
		cout << "finished" << endl;
		checkShadowCastReceive();
		cout << "finished with shadows thingy" << endl;

		// **********************************************************************
		// * Finished building the ogre scene node tree				*
		// * 									*
		// **********************************************************************

		
		// Create event handling system
		cout << "FORC: Initializing eventhandling " << endl;
		mpListi = new MyListener();
		mpMoti = new MyMouseListener();
		mpButi = new MyMouseButtonListener();
		mpEvpro = new EventProcessor();
		mpEvpro->addMouseMotionListener(mpMoti);
		mpEvpro->addMouseListener(mpButi);
		mpEvpro->addKeyListener(mpListi);
		mpEvpro->initialise(mpWindow);
		mpEvpro->startProcessingEvents();

		// Set start time
		mOldTime = mpTimer->getMilliseconds();

	} // try
	catch ( Exception& e )
       	{
	#if OGRE_PLATFORM == PLATFORM_WIN32
        //MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	fprintf(stderr, "An exception has occured: %s\n", e.getFullDescription().c_str());
	#else
        fprintf(stderr, "An exception has occured: %s\n", e.getFullDescription().c_str());
	#endif
    	}
} // end initialize()

void OgreCore::changeBackgroundColor(int r, int g, int b)
{
 	mpVp->setBackgroundColour(ColourValue(r, g, b));
}

// **********************************************************************
// * OgreCore::createStaticPlane(...)					*
// *									*
// * Callable from cgkit.						*
// * Creates a pure ogre plane type, this geometry is not		*
// * handled by cgkit, only internally by ogre.				*
// **********************************************************************

int OgreCore::createStaticPlane(float nx, float ny, float nz, float d, float width, float height, 
				const char* name, const char* material, float px, float py, float pz,
				float rx, float ry, float rz, int wu) 
{
	// TODO: Revise for OGRE 1.0
	/*
	// Create plane
	Mesh* p_overlay_mesh = MeshManager::getSingletonPtr()->createPlane(name, \
		Plane( Vector3(nx,ny,nz), d ), width, height, 10,10, true,1,  wu, wu);
	Entity* p_overlay_entity = mpSceneMgr->createEntity(std::string(name)+"_ent", name);

	// Assign specified material
	if(mHaveToParse)
	{
		// TODO: Replace this call with sth equivalent
		//MaterialManager::getSingleton().parseAllSources();
		mHaveToParse=false;
	}
	p_overlay_entity->setMaterialName(material);

	// Link into the scene graph
	SceneNode* p_overlay_snode = static_cast<SceneNode*>( mpSceneMgr->getRootSceneNode()->createChildSceneNode() );
	p_overlay_snode->attachObject(p_overlay_entity);

	// Make basic transforms, specified by parameters
	p_overlay_snode->translate(px,py,pz);
	p_overlay_snode->roll(Radian(Degree(rz)));
	p_overlay_snode->pitch(Radian(Degree(rx)));
	p_overlay_snode->yaw(Radian(Degree(ry)));
	*/
	return 1;
	
}


// **********************************************************************
// * OgreCore::removeStaticPlane(...)					*
// *									*
// * Callable from cgkit.						*
// * Removes a plane created by createStaticPlane()			*
// **********************************************************************

int OgreCore::removeStaticPlane(const char* name) 
{
	String entity_name = std::string(name) + "_ent";
	Entity *plane_entity = 0;
	plane_entity = mpSceneMgr->getEntity(entity_name);
	SceneNode* plane_node = 0;
	if ( plane_entity->isAttached() )
		plane_node = plane_entity->getParentSceneNode();
	else
	{
		cout << "FORC: Could not remove node, as entity is not firmly attached" << endl;
		return -1;
	}

	// Firstly, remove the entity
	if ( plane_entity != 0 )
		mpSceneMgr->removeEntity(plane_entity);
	else
	{
		cout << "FORC: Could not remove entity, however, couldnt even find it" << endl;
		return -1;
	}

	// Secondly, remove the scene node
	if ( plane_node != 0 )
		mpSceneMgr->destroySceneNode(plane_node->getName());
	else
	{
		cout << "FORC: Could not remove scene node, however, couldnt even find it" << endl;
		return -1;
	}
	return 1;
}

// **********************************************************************
// * OgreCore::transformStaticPlane(...)				*
// *									*
// * Callable from cgkit.						*
// * Transforms a plane created by createStaticPlane() 			*
// **********************************************************************

int OgreCore::transformStaticPlane(const char* name, float xp, float yp, float zp, float rx, float ry, float rz) 
{
	// Get the scene node
	String entity_name = std::string(name) + "_ent";
	Entity *plane_entity = 0;
	plane_entity = mpSceneMgr->getEntity(entity_name);
	SceneNode* plane_node = 0;
	if ( plane_entity->isAttached() )
		plane_node = plane_entity->getParentSceneNode();
	else
	{
		cout << "FORC: Could not get a scene node, as entity is not firmly attached" << endl;
		return -1;
	}

	// Transform the scene node
	if ( plane_node != 0 )
	{
		// rotate plane
		plane_node->yaw(Radian(ry));
		plane_node->roll(Radian(rz));
		plane_node->pitch(Radian(rx));

		// move plane
		plane_node->translate(xp,yp,zp);
	}
	else
	{
		cout << "FORC: Could not transform scene node, however, couldnt even find it" << endl;
		return -1;
	}
	return 1;
}


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

int OgreCore::rebuildOgreTree(support3d::WorldObject& VisumRoot)
{
	try
	{
	// Remove dependents  
      	unsigned int h=0;
	unsigned int max = dependents.size();
      	for ( h=0; h < max; h++ )
      	{
	        // camera
        	if (dependents[h]->mIsCamera == true)
		{
        		cout << "FORC: Removing Camera" << endl;
              		(dependents[h]->mpTransi->transform).removeDependent(dependents[h]);
		}
		// distant light
		else if (dependents[h]->mIsDistantLight == true)
		{ 
 	             	cout << "FORC: Removing DistantLight" << endl;
	                (dependents[h]->mpTransi->transform).removeDependent(dependents[h]);
			( static_cast<support3d::GLDistantLight*>(dependents[h]->mpTransi) )->enabled.removeDependent(dependents[h]);
			( static_cast<support3d::LightSource*>(dependents[h]->mpTransi) )->intensity.removeDependent(dependents[h]);
			( static_cast<support3d::GLDistantLight*>(dependents[h]->mpTransi) )->diffuse.removeDependent(dependents[h]);
			( static_cast<support3d::GLDistantLight*>(dependents[h]->mpTransi) )->specular.removeDependent(dependents[h]);
		}
		// spot light
		else if (dependents[h]->mIsSpotLight == true)
		{
	                cout << "FORC: Removing SpotLight" << endl;
	                (dependents[h]->mpTransi->transform).removeDependent(dependents[h]);
			( static_cast<support3d::GLSpotLight*>(dependents[h]->mpTransi) )->enabled.removeDependent(dependents[h]);
			( static_cast<support3d::LightSource*>(dependents[h]->mpTransi) )->intensity.removeDependent(dependents[h]);
			( static_cast<support3d::GLSpotLight*>(dependents[h]->mpTransi) )->diffuse.removeDependent(dependents[h]);
			( static_cast<support3d::GLSpotLight*>(dependents[h]->mpTransi) )->specular.removeDependent(dependents[h]);
	
			( static_cast<support3d::GLSpotLight*>(dependents[h]->mpTransi) )->constant_attenuation.removeDependent(dependents[h]);
			( static_cast<support3d::GLSpotLight*>(dependents[h]->mpTransi) )->linear_attenuation.removeDependent(dependents[h]);
			( static_cast<support3d::GLSpotLight*>(dependents[h]->mpTransi) )->quadratic_attenuation.removeDependent(dependents[h]);
			( static_cast<support3d::GLSpotLight*>(dependents[h]->mpTransi) )->exponent.removeDependent(dependents[h]);
			( static_cast<support3d::GLSpotLight*>(dependents[h]->mpTransi) )->cutoff.removeDependent(dependents[h]);
		}
		// point light
		else if (dependents[h]->mIsPointLight == true)
		{
			cout << "FORC: Removing PointLight" << endl;
			(dependents[h]->mpTransi->transform).removeDependent(dependents[h]);
			( static_cast<support3d::GLPointLight*>(dependents[h]->mpTransi) )->enabled.removeDependent(dependents[h]);
			( static_cast<support3d::LightSource*>( dependents[h]->mpTransi) )->intensity.removeDependent(dependents[h]);
			( static_cast<support3d::GLPointLight*>(dependents[h]->mpTransi) )->diffuse.removeDependent(dependents[h]);
			( static_cast<support3d::GLPointLight*>(dependents[h]->mpTransi) )->specular.removeDependent(dependents[h]);
	
			( static_cast<support3d::GLPointLight*>(dependents[h]->mpTransi) )->constant_attenuation.removeDependent(dependents[h]);
			( static_cast<support3d::GLPointLight*>(dependents[h]->mpTransi) )->linear_attenuation.removeDependent(dependents[h]);
			( static_cast<support3d::GLPointLight*>(dependents[h]->mpTransi) )->quadratic_attenuation.removeDependent(dependents[h]);
		}
	        // no light, no camera, so standard object with transform and visibility dependency only     
	        else 
		{
	  	        cout << "FORC: Removing misc. dependent" << endl;
	   	        (dependents[h]->mpTransi->transform).removeDependent(dependents[h]);
			(dependents[h]->mpTransi->visible).removeDependent(dependents[h]);
		}
			
		// Finally, remove the dependent from memory
		DEBUGOUT("... Removing:",h);
	       	delete dependents[h];

	} // end for

	// Clear the scene
	mMyString = "o";
	mpSceneMgr->clearScene();

	// Rebuild the scene
	SceneNode * snode = static_cast<SceneNode*>( mpSceneMgr->getRootSceneNode()->createChildSceneNode() );
	DEBUGOUT("Built a new SceneNode in initialize", snode );
	buildTree(VisumRoot, snode);
	}
	catch(...)
	{
		cout << "FORC: Error: Scene graph cannot be changed." << endl;
		return -1;
	}
	return 1;
}


// **********************************************************************
// * OgreCore::setCaptureVideo(int jaNein)				*
// *									*
// * Callable from cgkit.						*
// * Enables/disabled video capturing.					*
// **********************************************************************

int OgreCore::setCaptureVideo(int jaNein)
{
	if (jaNein == 1)
		mCaptureVideo=true;
	else
		mCaptureVideo=false;
	
	mFrameNumber = 0;
	return 1;
}



// **********************************************************************
// * OgreCore::setAmbient(float x, float y, float z)			*
// *									*
// * Callable from cgkit.						*
// * Sets the global ambient light color value.				*
// **********************************************************************

int OgreCore::setAmbient(float x, float y, float z)
{
	if (mpSceneMgr !=0)
	{
		mpSceneMgr->setAmbientLight(ColourValue(x, y, z));
		return 1;
	}
	else
	{
		cout << "FORC: Error: Scene Manager is not present!" << endl;
		return -1;
	}
}


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

int OgreCore::setCaptureFrameBuffer(std::string whichMaterial, int width, \
					int height, unsigned short whichTechnique, unsigned short whichPass )
{
	// TODO: Revise for Ogre 1.0
	/*
	if (mpSceneMgr !=0)
	{
		// Check if material is available
		if (mHaveToParse)
		{
			cout << "FORC: Not all materials loaded, have to Parse, therefore sloooooow" << endl;
			try
			{
				// TODO: Replace this call with sth equivalent
				//MaterialManager::getSingleton().parseAllSources();
				mHaveToParse = false;
			}
			catch(...)
			{
				cout << "FORC: Error while loading materials" << endl;
			}
		}

		// Prepare texture
		mpRenderTexture = 0;
		mpRenderTexture = mpRoot->getRenderSystem()->createRenderTexture("FrameBuffer", width, height);
		mpRenderTexture->setAutoUpdated(false);
		Viewport* p_texture_vp = mpRenderTexture ->addViewport(mpCamera);
		p_texture_vp->setBackgroundColour( mpVp->getBackgroundColour() );

		// Create a new texture unit state for the specified material
		try
		{
			TextureUnitState* p_tex_unit = mpSceneMgr->getMaterial(whichMaterial)->getTechnique(whichTechnique)->getPass(whichPass)	\
				->createTextureUnitState("FrameBuffer");
			
			// Important: filtering, depending on wanted effects, speed and quality
			// (e.g. no filtering for blocky filters, linear for blurry filters, etc..)
			p_tex_unit->setTextureFiltering(TFO_NONE);
		} catch(...)
		{
			cout << "FORC: Material does not exist" << whichMaterial << endl;
		}

		// whichMaterial now contains an additional TexUnit rpresentating the Framebuffer, available for sampling
		// Its best not to use this material when capturing the actual frame buffer of course.
		// Acess the framebuffer in whichMaterial via tex2d(...FrameBuffer...) in your shader

		mIsCaptureFrameBuffer = true;
		return 1;
	}
	else
	{
		cout << "FORC: Error: Scene manager not present!" << endl;
		return -1;
	}
	*/
	return 1;
}


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

void OgreCore::setRenderOverlay(std::string overlayMaterial)
{
	// TODO: Revise for OGRE 1.0
	/*
	try
	{
		// Check if material available
		if (mHaveToParse)
		{
			cout << "FORC: Not all materials loaded... parsing specified targets for material definitions" << endl;
			// TODO: Replace this call with sth equivalent
			//MaterialManager::getSingleton().parseAllSources();
			mHaveToParse = false;
		}

		// Calculate dimensions
		float ratio = mpWindow->getWidth()/mpWindow->getHeight();
		float near_clip = mpCamera->getNearClipDistance();
		Radian alpha = mpCamera->getFOVy(); // evtl. typecast
		float tresh = 1.0;
		float hoehe_halbe = (near_clip+tresh) * sin (alpha.valueRadians()/2) / cos (alpha.valueRadians()/2);
		float hoehe = hoehe_halbe*2.0;
		float breite= hoehe * ratio * mpCamera->getAspectRatio();
		Mesh* overlay_mesh = MeshManager::getSingletonPtr()->createPlane("OverlayMesh", Plane( Vector3(0,0,1),-near_clip - tresh ), breite, hoehe);

		// Create entity and scene node for the overlay
		Entity* overlay_entity = mpSceneMgr->createEntity("OverlayEntity", "OverlayMesh");
		overlay_entity->setMaterialName(overlayMaterial);
		SceneNode* p_cam_node = mpCamera->getParentSceneNode();
		DEBUGOUT("camera Scene Node", p_cam_node);
		mpOverlayNode = static_cast<SceneNode *>( p_cam_node->createChildSceneNode() );
		DEBUGOUT("Overlay Scene Node", mpOverlayNode);
		mpOverlayNode->attachObject(overlay_entity);

		// Don't render it at first, set the render state with the switchVisibility() method below
		mpOverlayNode->setVisible(false);
	}
	catch(...)
	{
		cout << "FORC: Could not create Overlay" << endl;
	}
	*/
}


// **********************************************************************
// * OgreCore::switchVisibility()					*
// *									*
// * Callable from cgkit.						*
// * Overlay must have been set before this method can be used.		*
// * Changes visibility method that defines if the scene is renderd	*
// * normally or only the previous set overlay can be seen.		*
// **********************************************************************

int OgreCore::switchVisibility()
{
	if (mpOverlayNode == 0)
	{
		cout << "FORC: Error: Overlay was not set, so it can not be rendered" << endl;
		return -1;
	}

	if (mGlobalVisibleFlag)
	{
		mGlobalVisibleFlag = false;

		// Make whole ogre scene graph invisible!
		mpSceneMgr->getRootSceneNode()->flipVisibility(true);
		
		// But not the overlay node
		mpOverlayNode->setVisible(true);
	}
	else
	{
		mGlobalVisibleFlag = true;

		// Make whole ogre scene graph visible!
		mpSceneMgr->getRootSceneNode()->flipVisibility(true);

		// But not the overly node
		mpOverlayNode->setVisible(false);
	}
	return 1;
}


// **********************************************************************
// * OgreCore::captureFrameBuffer()					*
// *									*
// * Callable from cgkit.						*
// * Renders the frame buffer to a texture that is available to		*
// * peviously specified material.					*
// **********************************************************************

void OgreCore::captureFrameBuffer()
{
	if (mIsCaptureFrameBuffer)
		mpRenderTexture->update();
}


// **********************************************************************
// * OgreCore::renderTree()						*
// *									*
// * Callable from cgkit.						*
// * Renders the whole ogre scene graph.				*
// **********************************************************************
 
void OgreCore::renderTree()
{
	// Check for stereo rendering
	if (mIsStereo)
	{
		mpCamera->getParentSceneNode()->translate(Vector3(-0.3f, 0.0f, 0.0f), Node::TS_LOCAL);
		mpCameraStereo->getParentSceneNode()->translate(Vector3(+0.6f, 0.0f, 0.0f), Node::TS_LOCAL);

		mpRoot->renderOneFrame();

		mpCamera->getParentSceneNode()->translate(Vector3(+0.3f, 0.0f, 0.0f), Node::TS_LOCAL);
		mpCameraStereo->getParentSceneNode()->translate(Vector3(-0.6f, 0.0f, 0.0f), Node::TS_LOCAL);
	}
	else // Render the normal way
	{
		mpRoot->renderOneFrame();
	}

	// Write screenshots to files eventually
	if (mCaptureVideo)
	{
 		char buffer[20];
  		sprintf (buffer, "OgreBild%d.jpg", mFrameNumber);
		mpWindow->writeContentsToFile( (const char*) buffer);
		cout << "FORC: Frame Captured: " << mFrameNumber << ", written to file: " << buffer << endl;
		mFrameNumber++;

		mCaptureVideo = false;
	}
}
// **********************************************************************
// * OgreCore::setupShadowMode(...)					*
// *									*
// * Callable from cgkit						*
// **********************************************************************

void OgreCore::setupShadowMode(int typeFlags, int otherFlags)
{
	mShadowsEnabled = false;
	mTextureShadows = false;
	otherShadowFlags = -1;
	
	shadowFlags = 0;
	if (typeFlags == 0)
	{
               	mShadowsEnabled = true;
		cout<< " *S*H*A*D*O*W*S*************************** SHADOWTYPE_STENCIL_MODULATIVE" << endl;
		shadowFlags = 0;
	}	
	if (typeFlags == 1)
	{
               	mShadowsEnabled = true;
		cout<< " *S*H*A*D*O*W*S*************************** SHADOWTYPE_STENCIL_ADDITIVE" << endl;
		shadowFlags = 1;
	}	
	if (typeFlags == 2)
	{
               	mShadowsEnabled = true;
		cout<< " *S*H*A*D*O*W*S*************************** SHADOWTYPE_TEXTURE_MODULATIVE" << endl;
		shadowFlags = 2;
		otherShadowFlags = otherFlags;
		mTextureShadows = true;
	}	
}


// **********************************************************************
// * OgreCore::setRenderSystem(Root *root, const char *name)		*
// *									*
// * Choose a suitable render system, e.g. 				*
// * "Direct3D9 Rendering SubSystem" or 				*
// * "GL Rendering SubSystem".						*
// **********************************************************************

bool OgreCore::setRenderSystem(Root *root, const char *name)
{
	// Fill a vector with all available Renderers:
	cout << "Searching for correct Rendering SubSystem" << endl;
	RenderSystemList *renderers = root->getAvailableRenderers();
	RenderSystemList::iterator it = renderers->begin();

	// Iterating through all available Rendering Systems:
	while(true)
	{
		if( it == renderers->end() )
		{
			// Our desired System is not available.
			cout << "Correct Rendering SubSystem is not available" << endl;
			return false;
		}
		if( (*it)->getName() == Ogre::String(name)  )
		{
			// We have found it, now set the Rendering System:
			root->setRenderSystem(*it);
			cout << "The correct Render System has been found: " << (*it)->getName() << endl;
			return true;
		}
		it++;
	}
}


// **********************************************************************
// * OgreCore::getMeshFromCgKit(Root *root, const char *name)		*
// *									*
// * Creates an ogre mesh from a cgkit TriMeshGeom type.		*
// **********************************************************************

Mesh* OgreCore::getMeshFromCgKit(support3d::TriMeshGeom *trigeom, const char *name)
{
	// Nach wie vor die Problemfunktion ...

	// ||||||||||||||||| V A R I A B L E N |||||||||||||||||||||||||||||||||||||||||
	// ||||||||||||||||| V A R I A B L E N |||||||||||||||||||||||||||||||||||||||||

	// externes Mesh (ptr) - Rückgabewert
	Mesh *mesh = Ogre::MeshManager::getSingleton().createManual(name, (String(name)+"gr").c_str()).get();
	DEBUGOUT("Created empty manual mesh: "+ std::string(name), mesh);

	// bis jetzt hat jedes trimesh ein submesh und ein mesh,
	// TODO: Anpassen
	SubMesh *sub = mesh->createSubMesh(); 
	sub->useSharedVertices = false;

	support3d::vec3d* vertsptr = trigeom->verts.dataPtr();
	support3d::vec3d* vertsptr2 = trigeom->verts.dataPtr();
	int* faceptr = trigeom->faces.dataPtr();
	int* faceptr2 = trigeom->faces.dataPtr();
	int* faceptr3 = trigeom->faces.dataPtr();

	int numfaces = trigeom->faces.size();
	int numverts = trigeom->verts.size();

	DEBUGOUT("Faces: ", numfaces);
	DEBUGOUT("Vertices: ", numverts);

	// TODO: für später schlau einrichten
	int vertsize = 8;
	int totalvertsize = vertsize * numverts;

	// ||||||||||||||||| H A R D W A R E B U F F E R ||||||||||||||||||||||||||||||
	// ||||||||||||||||| H A R D W A R E B U F F E R ||||||||||||||||||||||||||||||
	
	// VertexBuffer erstellen
	HardwareVertexBufferSharedPtr vbuf;
	vbuf = HardwareBufferManager::getSingleton().createVertexBuffer(
			vertsize * sizeof(Real),				// size of one vertex
			numverts,								// number of vertices
			HardwareBuffer::HBU_STATIC_WRITE_ONLY,	// no need to read, no need to update often
			true );									// holding a shadowbuffer in main memory

	// IndexBuffer erstellen
	sub->indexData->indexCount = numfaces*3;
	sub->indexData->indexStart = 0;
	HardwareIndexBufferSharedPtr ibuf;
	ibuf = HardwareBufferManager::getSingleton().createIndexBuffer(
			HardwareIndexBuffer::IT_32BIT,			// 32 Bit defines the amount of indexes you could theoretically use
			sub->indexData->indexCount,				// how much indexes
			HardwareBuffer::HBU_STATIC_WRITE_ONLY,	// as above, for organization and optimization of the buffer.
			true );									// again, we want a shadowbuffer in main memory
	sub->indexData->indexBuffer = ibuf;				// which index buffer to use

	// ||||||||||||||||| B U F F E R   F Ü L L E N ||||||||||||||||||||||||||||||||
	// ||||||||||||||||| B U F F E R   F Ü L L E N ||||||||||||||||||||||||||||||||

	// Vorarbeit für von außen spezifizierte Texturkoordinaten.......................................
	bool standardCoords = false;
	support3d::PrimVarInfo* texCoords = trigeom->findVariable("st");
	support3d::PrimVarInfo* texFaces = trigeom->findVariable("stfaces");
	double *mp=0;

	DEBUGOUT("Checking for texture coordinates", texCoords);
	if (texCoords == 0 || texCoords->storage != support3d::VARYING || texCoords->type != support3d::FLOAT || texCoords->multiplicity != 2 )
	{
		// check for USER // stfaces
		/*if (texCoords != 0 && texCoords->storage != support3d::USER && texFaces != 0)
		{
			DEBUGOUT("User Texture Coordinates and IndexSet", texFaces);
			// habe jetzt einen "st" und "stfaces" bereich, hui
			
		}
		else
		{*/
			cout << "FORC: Warning: Could not assing texture coordinates" << endl;
			standardCoords = true;
		//}
	}
	else
	{
		// du bist ein array slot
		support3d::ArraySlot<double>* arry = dynamic_cast<support3d::ArraySlot<double>*>(texCoords->slot);
		mp = arry->dataPtr();
	}
	// ........................ Vorarbeit Ende


	// Vorarbeit für von außen spezifizierte Normalen................................................
	bool normals = false;

	support3d::PrimVarInfo *specNormals = trigeom->findVariable("N");
	support3d::vec3d *np=0; // evtl problemquelle hier

	DEBUGOUT("Checking for normals", specNormals);
	if (specNormals == 0 || specNormals->storage != support3d::VARYING || specNormals->type != support3d::NORMAL || specNormals->multiplicity != 1 )
	{
		cout << "FORC: Warning: Could not assign normals, hence i will calculate them." << endl;
	}
	else
	{
		// alle variablen sind arrayslots
		support3d::ArraySlot<support3d::vec3d>* norry = dynamic_cast<support3d::ArraySlot<support3d::vec3d>*>(specNormals->slot);

		np = norry->dataPtr();
		cout << "FORC: Normals detected: Count: " << norry->size() << endl;
		normals = true;
	}
	// ......................... Vorarbeit Ende
	
	// 3 Spezial-Pointer
	Real *pReal=0; //Real *pReal2=0; Real *pReal3=0;
	pReal  = (Real*) vbuf->lock(HardwareBuffer::HBL_DISCARD); // Pointer für Vertices
	//pReal2 = pReal; // Basepointer für Normalen
	//pReal3 = pReal; // zum eintragen der normalen in den vertexbuffer

	cout << "FORC: Filling Index Buffer and calculating normals if not specified" << endl;
	support3d::vec3d *normalsVector=0;
	normalsVector = new support3d::vec3d[numverts];
	int i=0;

	// ( 3 ) IndexBuffer füllen (einfach)
	unsigned int* pIndexes = static_cast<unsigned int*>( ibuf->lock(HardwareBuffer::HBL_DISCARD) );
	for (i = 0; i < numfaces; ++i)
	{
		int ia,ib,ic;
		support3d::vec3d normal;
		normal.set(0.0, 0.0, 0.0);

		ia = *(faceptr);
		*pIndexes++ = *faceptr;
		//DEBUGOUT("Index", *faceptr);
		faceptr++;

		ib = *faceptr;
		*pIndexes++ = *faceptr;
		//DEBUGOUT("Index", *faceptr);
		faceptr++;

		ic = *faceptr;
		*pIndexes++ = *faceptr;
		//DEBUGOUT("Index", *faceptr);
		//
		faceptr++;

		// NORMALS part (face-normals), eine normale
		if (!normals)
		{
			try
			{
				normal = ( normal.cross( *(vertsptr2+ib) - *(vertsptr2+ia), *(vertsptr2+ic) - *(vertsptr2+ia) ) );
				normal = normal.normalize();
			}
			catch(...)
			{
				cout << "FORC / Warning: (0,0,0) normal detected." << endl;
				normal.set(0,0,0);
			}

			// NORMALS part (face-normals) setzen, also insgesamt 3mal, zu den korrespondierenden ia, ib, ic vertices
			normalsVector[ia]=normal;
			normalsVector[ib]=normal;
			normalsVector[ic]=normal;
		}

	}

	cout << "FORC: Filling Vertex Buffer" << endl;
	// ( 1 ) Vertices einfügen

	DEBUGOUT("Writing Vertices|Normals|TexCoords in Buffer starting at", pReal);

	cout << "NORMALS --------------------------------------------" << endl;
	for (i = 0; i < numverts; ++i) 
	{
		// Koordinaten
		*pReal++ = vertsptr->x;
		*pReal++ = vertsptr->y;
		*pReal++ = vertsptr->z;
		vertsptr++;

		// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		// Normalen ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		if (normals) // zuweisen
		{
			*pReal++ = np->x;
			*pReal++ = np->y;
			*pReal++ = np->z;
			
			//cout << " NORMAL X Y Z  ptr* " << np->x << " " << np->y << " " << np->z;

			np++;
		}
		else // zuweisen
		{
			*pReal++=normalsVector[i].x;
			*pReal++=normalsVector[i].y;
			*pReal++=normalsVector[i].z;

			//cout << " NORMAL X Y Z  # " << normalsVector[i].x << " " << normalsVector[i].y << " " << normalsVector[i].z;
		}

		// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		// Texturkorrdinaten ;;;;;;;;;;;;;;;;;;;;;;;
		// ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		if (!standardCoords)
		{
			*pReal++ = (*mp);
			mp++;
			*pReal++ = (*mp);
			mp++;
		}
		else // standardCoords == true
		{
			*pReal++ = 0.0; // schwachsinn
			*pReal++ = 0.0;	// schwachsinn
		}
	}
	cout << " ! ! " << endl;

	cout << "FORC: Unlocking Buffers" << endl;

	// Fertig, daher unlock
	ibuf->unlock(); 
	vbuf->unlock();

	if( normalsVector !=0 )
		delete[] normalsVector;


	// ||||||||||||||||| V E R T E X   D A T A |||||||||||||||||||||||||||||||||||
	// ||||||||||||||||| V E R T E X   D A T A |||||||||||||||||||||||||||||||||||

	DEBUGOUT("Setting up VertexData", mesh);

	sub->vertexData = new VertexData(); 
	sub->vertexData->vertexStart = 0; 
	sub->vertexData->vertexCount = numverts; 
	sub->vertexData->vertexBufferBinding->setBinding(0, vbuf);

	// ||||||||||||||||| V E R T E X   D E C L A R A T I O N |||||||||||||||||||||
	// ||||||||||||||||| V E R T E X   D E C L A R A T I O N |||||||||||||||||||||

	size_t offset = 0; 
	VertexDeclaration* decl = sub->vertexData->vertexDeclaration; 
	decl->addElement(0, offset, VET_FLOAT3, VES_POSITION);				// first 3 represents position
	offset += VertexElement::getTypeSize(VET_FLOAT3); 
	decl->addElement(0, offset, VET_FLOAT3, VES_NORMAL);				// next 3 normals
	offset += VertexElement::getTypeSize(VET_FLOAT3); 
	decl->addElement(0, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES, 0); // and 2 texture coordinates
	offset += VertexElement::getTypeSize(VET_FLOAT2); 
	// Remember to pass the right source index (here: 0), as you could have more than one bound VertexBuffer.

	DEBUGOUT("BoundingBox", mesh);
	// Define the BoundingBox (our duty when creating manual Meshes) given by two "extremity points" of the box.
	support3d::BoundingBox bb = trigeom->boundingBox();
	support3d::vec3d min;
	support3d::vec3d max;
	bb.getBounds(min, max);
	mesh->_setBounds(AxisAlignedBox(min.x, min.y, min.z, max.x, max.y, max.z));

      	if (mShadowsEnabled)
      	{
		cout << "FORC: Building Edge List for OGRE" << endl;
		// Now: Prepare for STENCIL SHADOWS:
		//mesh->prepareForShadowVolume();
		//mesh->buildEdgeList();   
      	}

      	DEBUGOUT("Created mesh, returning", mesh);
      	return mesh;
}

