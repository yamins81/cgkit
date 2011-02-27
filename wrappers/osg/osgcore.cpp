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

#include "osgcore.h"

//////////////////////////////////////////////////////////////
 //////////////////////////////////////////////////////////////
  ///////////////////////////// -- /////////////////////////////
   //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////

void createMesh(support3d::TriMeshGeom *trigeom, osg::Geometry *pGeometry)
{ 
	int i = 0;
	int j = 0;
	bool normals;
	osg::Vec3Array* pMeshVertices = 0;
	osg::Vec3Array* pMeshNormals = 0;   
	osg::Vec2Array* pMeshTexCoords = 0;
	osg::DrawElementsUInt* pElements = 0;
	bool useStdCoords = false;
	support3d::PrimVarInfo* texCoords = 0;
	support3d::PrimVarInfo* texFaces = 0;
	osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType,4,4>* pMeshNormalIndices = 0;

	std::cout << std::endl;
	std::cout << "|||||||||||||||||||||||||||||||||||||||||||||||||||||||" << std::endl;
	std::cout << "||||||||||||||| creating OPENSCENEGRAPH mesh ||||||||||" << std::endl;
	std::cout << "|||||||||||||||||||||||||||||||||||||||||||||||||||||||" << std::endl; 
	std::cout << std::endl;

	if (trigeom == 0 || pGeometry == 0)
	{
		std::cout << "[GEOMETRY] Error: Invalid pointer, could not create mesh" << std::endl;
		return;
	}

	pMeshVertices = new osg::Vec3Array();
	pMeshNormals = new osg::Vec3Array();
	pMeshTexCoords = new osg::Vec2Array();
	pMeshNormalIndices = new osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType,4,4>;
	pElements = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);

	support3d::vec3d* pVertices = trigeom->verts.dataPtr();
	support3d::vec3d* vertsptr2 = trigeom->verts.dataPtr();

	int* faceptr = trigeom->faces.dataPtr();
	int* faceptr2 = trigeom->faces.dataPtr();
	int* faceptr3 = trigeom->faces.dataPtr();

	int numfaces = trigeom->faces.size();
	int numverts = trigeom->verts.size();

	//_________________________________
	// CHECKING FOR TEXTURE COORDINATES

	std::cout << "[GEOMETRY] Checking for a standard texture coordinate variable" << std::endl;
	texCoords = trigeom->findVariable("st");
	texFaces = trigeom->findVariable("stfaces");
	std::cout << "[GEOMETRY]   Texture coordinates variable " << texCoords << std::endl;
	double *pTexCoords=0;
	if ( texCoords == 0
		|| texFaces != 0
		|| texCoords->storage != support3d::VARYING 
		|| texCoords->type != support3d::FLOAT 
		|| texCoords->multiplicity != 2 )
	{

		std::cout << "[GEOMETRY]   Warning: Could not assign texture coordinates, given values are not supported yet" << std::endl;
		std::cout << "[GEOMETRY]      "<< texCoords << std::endl;
		std::cout << "[GEOMETRY]      "<< texCoords->storage << std::endl;
		std::cout << "[GEOMETRY]      "<< texCoords->type << std::endl;
		std::cout << "[GEOMETRY]      "<< texCoords->multiplicity << std::endl;
		useStdCoords = true;
	}
	else
	{
		std::cout << "[GEOMETRY]   Found st variable, creating a basic texture coordinate array" << std::endl;
		support3d::ArraySlot<double>* pTexArray = 0;
		pTexArray = dynamic_cast<support3d::ArraySlot<double>*>(texCoords->slot);
		if( !pTexArray )
			std::cout << "[GEOMETRY]   Fatal Error while loading texture coordinate set!" << std::endl;
		std::cout << "[GEOMETRY]   st variable contains " << pTexArray->size() << " items" << std::endl;
		pTexCoords = pTexArray->dataPtr();
	}

	// ____________________
	// CHECKING FOR NORMALS

	std::cout << "[GEOMETRY] Checking for specified geometry normals" << std::endl;
	normals = false;

	support3d::PrimVarInfo *pNormalsVarInfo = trigeom->findVariable("N");
	support3d::vec3d *pNormals=0;
	support3d::ArraySlot<support3d::vec3d>* normsptr = 0;

	if (pNormalsVarInfo == 0 
		|| pNormalsVarInfo->storage != support3d::VARYING 
		|| pNormalsVarInfo->type != support3d::NORMAL 
		|| pNormalsVarInfo->multiplicity != 1	)
	{
		std::cout << "[GEOMETRY]    Could not assign normals, hence i will calculate them" << std::endl;
		normals = false;
	}
	else
	{
		normsptr = dynamic_cast<support3d::ArraySlot<support3d::vec3d>*>(pNormalsVarInfo->slot);
		pNormals = normsptr->dataPtr();

		std::cout << "[GEOMETRY]    Found normals variable with " << normsptr->size() << " entries" << std::endl;
		normals = true;
	}

	 
	support3d::vec3d *normalsVector=0;
	normalsVector = new support3d::vec3d[numverts];

	for (i = 0; i < numfaces; i++)
	{
		int ia,ib,ic;
		support3d::vec3d normal;
		normal.set(0.0, 0.0, 0.0);

		ia = *faceptr;
		pElements->push_back(*faceptr);
		pMeshNormalIndices->push_back(*faceptr);
		faceptr++;

		ib = *faceptr;
		pElements->push_back(*faceptr);
		pMeshNormalIndices->push_back(*faceptr);
		faceptr++;

		ic = *faceptr;
		pElements->push_back(*faceptr);
		pMeshNormalIndices->push_back(*faceptr);
		faceptr++;

		// NORMALS
		if (!normals)
		{
			try
			{
				normal = ( normal.cross( *(vertsptr2+ib) - *(vertsptr2+ia), *(vertsptr2+ic) - *(vertsptr2+ia) ) );
				normal = normal.normalize();
			}
			catch(...)
			{
				std::cout << "[GEOMETRY]       Warning: (0,0,0) normal detected." << std::endl;
				normal.set(0,0,0);
			}

			// NORMALS part (face-normals) setzen, also insgesamt 3mal, zu den korrespondierenden ia, ib, ic vertices
			normalsVector[ia]=normal;
			normalsVector[ib]=normal;
			normalsVector[ic]=normal;
		}
	}
	std::cout << "[GEOMETRY] Index buffer finished" << std::endl;

	int bla = 0;
	for (i = 0; i < numverts; i++) 
	{
		if (normals)
		{
			pMeshVertices->push_back( osg::Vec3(pVertices->x, pVertices->y, pVertices->z) );
			pMeshNormals->push_back( osg::Vec3(pNormals->x, pNormals->y, pNormals->z) );
		}
		else
		{
			pMeshVertices->push_back( osg::Vec3(pVertices->x, pVertices->y, pVertices->z) );
			pMeshNormals->push_back( osg::Vec3(normalsVector[i].x, normalsVector[i].y, normalsVector[i].z) );
		}
		if(!useStdCoords) // then a tex coord-variable is present
		{
			pMeshTexCoords->push_back( osg::Vec2( *(pTexCoords), *(pTexCoords+1) ) );
			pTexCoords += 2;
		}
		else
		{
			if (bla)
				pMeshTexCoords->push_back( osg::Vec2( 1.0f, 0.0f ) );
			else
				pMeshTexCoords->push_back( osg::Vec2( 0.0f, 1.0f ) );
			bla = 1-bla;
		}
		pVertices++;
		pNormals++;
	}
	std::cout << "[GEOMETRY] Vertex buffer finished" << std::endl;

	pGeometry->setVertexArray( pMeshVertices );
	pGeometry->setNormalArray( pMeshNormals );
	pGeometry->setTexCoordArray(0, pMeshTexCoords);
	pGeometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX  );
	// TODO: WHAT ABOUT VERTEX ARRAY BINDING?
	pGeometry->addPrimitiveSet( pElements );
	//osgUtil::SmoothingVisitor::smooth(*pGeometry);
}

//////////////////////////////////////////////////////////////
 //////////////////////////// * * /////////////////////////////
  //////////////////////////// o ///////////////////////////////
   //////////////////////////////////////////////////////////////  

void LightDependent::onValueChanged()
{
	support3d::vec3d changePos;
	support3d::vec3d changeScale;
	support3d::mat4d changeRot;
	double pValueList[16];

	// first: usual stuff
	// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

	// TODO: Optimize this!
	mpWorldObject->localTransform().toList( pValueList );
	mpMatrixTransform->setMatrix( osg::Matrixd(pValueList) );

	support3d::mat4d changeTransformation = mpWorldObject->localTransform();
	changeTransformation.decompose(changePos, changeRot, changeScale);

	// next: light specific stuff
	// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

	osg::Light *pTheLight = mpLightSource->getLight();

	switch(mLightType)
	{
		case DEP_POINT:
		{
			pTheLight->setPosition( osg::Vec4(changePos.x, changePos.y, changePos.z, 1.0) );
			support3d::GLPointLight *point_light = static_cast<support3d::GLPointLight*>(mpWorldObject);
			support3d::vec3d color_amb = (point_light->ambient).getValue();
			support3d::vec3d color_diff = (point_light->diffuse).getValue();
			support3d::vec3d color_spec = (point_light->specular).getValue();
			pTheLight->setAmbient(osg::Vec4(color_amb.x, color_amb.y, color_amb.z, 1.0f));
			pTheLight->setDiffuse(osg::Vec4(color_diff.x, color_diff.y, color_diff.z, 1.0f));
			pTheLight->setSpecular(osg::Vec4(color_spec.x, color_spec.y, color_spec.z, 1.0f));
			break;
		};
		case DEP_DIRECTIONAL:
		{
			support3d::GLDistantLight *point_light = static_cast<support3d::GLDistantLight*>(mpWorldObject);
			support3d::vec3d color_amb = (point_light->ambient).getValue();
			support3d::vec3d color_diff = (point_light->diffuse).getValue();
			support3d::vec3d color_spec = (point_light->specular).getValue();
			pTheLight->setAmbient(osg::Vec4(color_amb.x, color_amb.y, color_amb.z, 1.0f));
			pTheLight->setDiffuse(osg::Vec4(color_diff.x, color_diff.y, color_diff.z, 1.0f));
			pTheLight->setSpecular(osg::Vec4(color_spec.x, color_spec.y, color_spec.z, 1.0f));
			pTheLight->setPosition( osg::Vec4(changePos.x, changePos.y, changePos.z, 0.0) );
			break;
		};
		case DEP_SPOT:
		{
			support3d::GLSpotLight *point_light = static_cast<support3d::GLSpotLight*>(mpWorldObject);
			support3d::vec3d color_amb = (point_light->ambient).getValue();
			support3d::vec3d color_diff = (point_light->diffuse).getValue();
			support3d::vec3d color_spec = (point_light->specular).getValue();
			pTheLight->setAmbient(osg::Vec4(color_amb.x, color_amb.y, color_amb.z, 1.0f));
			pTheLight->setDiffuse(osg::Vec4(color_diff.x, color_diff.y, color_diff.z, 1.0f));
			pTheLight->setSpecular(osg::Vec4(color_spec.x, color_spec.y, color_spec.z, 1.0f));
			pTheLight->setPosition( osg::Vec4(changePos.x, changePos.y, changePos.z, 1.0) );
			pTheLight->setSpotCutoff( static_cast<support3d::GLSpotLight*>(mpWorldObject)->cutoff.getValue() );
			pTheLight->setSpotExponent( static_cast<support3d::GLSpotLight*>(mpWorldObject)->exponent.getValue() );
			break;
		};
	}
}

//////////////////////////////////////////////////////////////
 //////////////////////////////////////////////////////////////
  ///////////////////////////// -- /////////////////////////////
   //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////

OsgCore::OsgCore() { mLights = 0; };
OsgCore::~OsgCore() { };

//////////////////////////////////////////////////////////////
 //////////////////////////////////////////////////////////////
  ///////////////////////////// -- /////////////////////////////
   //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////

void OsgCore::initStuff(support3d::WorldObject& visumRoot)
{
	////////////////////////////////////////////////////////////////// 
	///////////////////////////////////////////////////////
	///////////////////////// create a root node
	/*
	std::cout << "" << std::endl;
	std::cout << "[INIT] Initializing osg scene graph" << std::endl;
	//mpRoot = 0;
	//mpRoot = new osg::MatrixTransform();
	//mpRootStateSet = new osg::StateSet();
	//mpRoot->setStateSet(mpRootStateSet);

	std::cout << "[INIT]    Root node: " << mpRoot << std::endl;
	std::cout << "[INIT]    Root state set: " << mpRootStateSet << std::endl;

	//////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////
	///////////////////////// build the tree
	std::cout << "[INIT]    Building osg scene graph" << std::endl;
	//buildTree(visumRoot, mpRoot);
	std::cout << "[INIT]    Osg scene graph was successfully built" << std::endl;

	std::cout << "" << std::endl;

	std::cout << "[VIEWER] Creating the osg/producer viewer" << std::endl;
	*/

 /*
	//////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////
	///////////////////////// ceating the vewer
	// cam tests
	Producer::CameraConfig* pCameraConfig = new Producer::CameraConfig();
	Producer::Camera* pCamera = new Producer::Camera();
	pCamera->setRenderSurfaceWindowRectangle(0,0, 800.0f, 600.0f);
	Producer::RenderSurface* pSurf = pCamera->getRenderSurface();
	pSurf->setWindowName(std::string("OSG VIEWER FOR CGKIT"));
	pSurf->fullScreen(false);
	pCameraConfig->addCamera("maincam", pCamera);
	theViewer = osgProducer::Viewer(pCameraConfig);



	theViewer.setUpViewer(osgProducer::Viewer::STANDARD_SETTINGS);
	theViewer.setSceneData( pRoot );
	mpEH = new MyEventHandler();
	theViewer.getEventHandlerList().push_front(mpEH); 
	theViewer.realize();
	//////////////////////////////////////////////////////////////////

	std::cout << "|||||||||||||||||||||||||||||||" << std::endl;
	std::cout << "||||||| starting viewer |||||||" << std::endl;
	std::cout << "|||||||||||||||||||||||||||||||" << std::endl; 
	std::cout << "|||||||||||||||||||||||||||||||" << std::endl;  
	*/
}

//////////////////////////////////////////////////////////////
 //////////////////////////////////////////////////////////////
  ///////////////////////////// -- /////////////////////////////
   //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////

boost::python::list OsgCore::pumpMouseDowns()
{
	boost::python::list mouseList;  
	int h = 0;
	for( h=0; h<mpEH->currentMouseIndex; h++)  
	{
		mouseList.append( mpEH->mouseButtonList[h] );
		mouseList.append( mpEH->eventTypeList[h]  );	
		mouseList.append( mpEH->mouseXList[h] );		
		mouseList.append( mpEH->mouseYList[h] );
		mouseList.append( mpEH->mousedXList[h] );
		mouseList.append( mpEH->mousedYList[h] );
	}

	mpEH->currentMouseIndex = 0; // do the reset

	return mouseList;
}

boost::python::list OsgCore::pumpKeyDowns()
{
	boost::python::list eventList; 
	int h = 0;
	for( h=0; h<mpEH->currentKeyDownIndex; h++)
	{
		eventList.append( (char)(mpEH->KeyDownInts[h]) );	// keyChar
		eventList.append(  (mpEH->KeyDownInts[h]) );		// keyInt
		eventList.append(  (mpEH->KeyDownMods[h]) );		// keyMod
	}

	mpEH->currentKeyDownIndex = 0; // do the reset

	return eventList;  
}

boost::python::list OsgCore::pumpKeyUps()
{
	boost::python::list eventList; 
	int h = 0;
	for( h=0; h<mpEH->currentKeyUpIndex; h++)
	{
		eventList.append( (char)(mpEH->KeyUpInts[h]) );	// keyChar
		eventList.append(  (mpEH->KeyUpInts[h]) );		// keyInt
		eventList.append(  (mpEH->KeyUpMods[h]) );		// keyMod
	}

	mpEH->currentKeyUpIndex = 0; // do the reset

	return eventList;
}

boost::python::list OsgCore::pumpMotions() 
{
	boost::python::list motionList;  

	if (mpEH->isMotion)  
	{ 
		motionList.append( mpEH->posX );
		motionList.append( mpEH->posY );
		motionList.append( mpEH->moveX );
		motionList.append( mpEH->moveY );  

		//std::cout << "Buttons: " << mpEH->mouseButtons << std::endl;

		if( mpEH->mouseButtons & osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON )
			motionList.append( 1 );
		else 
			motionList.append( 0 );
		if( mpEH->mouseButtons & osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON )
			motionList.append( 1 );
		else 
			motionList.append( 0 );
		if( mpEH->mouseButtons & osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON )
			motionList.append( 1 );
		else
			motionList.append( 0 );

		mpEH->isMotion = false; // do the reset
	}
	else
	{
		//std::cout << "no motions to pump" << std::endl;
		mpEH->isMotion = false; // do the reset
	}

	return motionList;
}

void OsgCore::buildTree(support3d::WorldObject& visumRoot, osg::MatrixTransform *pSnode)
{
	support3d::WorldObject::ChildIterator childit = visumRoot.childsBegin();

	// Recursive exit condition: there are no children
	if (childit->second.get() == 0)
	{
		std::cout << "no VISUM CHILDS left" << std::endl;
		return;
	}	

	std::cout << "VISUM nodes found " << pSnode << std::endl;
	std::cout << "identifying the VISUM nodes" << std::endl;

	while ( childit != visumRoot.childsEnd() )
	{
		osg::Geode* pGeode = 0;
		osg::Geometry* pGeometry = 0; 
		osg::MatrixTransform* pMForm = 0;

		std::cout << " " << std::endl;
		std::cout << "[NODE] Found a cgkit node " << childit->second.get()->name << std::endl;
		std::cout << "[NODE] Checking osg group node " << pSnode << std::endl;

		support3d::LightSource *light_source = 0;
		light_source = dynamic_cast<support3d::LightSource*>( childit->second.get() );

		// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
		// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx geometry found?
		// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

		if ( (*childit).second->getGeom().get() != 0 )
		{
			std::cout << " " << std::endl;
			std::cout << "[NODE] CGkit geometry node found" << std::endl;

			pGeode = new osg::Geode();
			pGeometry = new osg::Geometry();
			pMForm = new osg::MatrixTransform();

			// Build the architecture
			pMForm->addChild( pGeode );
			pGeode->addDrawable( pGeometry );
			pSnode->addChild( pMForm );
			// Build architecture

			support3d::TriMeshGeom *pTriGeom = 0;
			pTriGeom = dynamic_cast<support3d::TriMeshGeom*>( (*childit).second->getGeom().get() );

			if ( pTriGeom != 0 )
			{
				std::cout << "[NODE]     Creating OSG GEOMETRY from TRIMESH" << std::endl;
				createMesh( pTriGeom, pGeometry );
			}
			else
			{
				support3d::TriMeshGeom *pTri;
				std::cout << "[NODE]     Creating OSG GEOMETRY from CONVERTED TRIMESH" << std::endl;
				pTri = new support3d::TriMeshGeom();

				try
				{
					(childit->second->getGeom().get())->convert(pTri);
					// Create an ogre mesh based on the TriMesh
					createMesh( pTri, pGeometry );	
				}
				catch(...) // Could not convert
				{
					std::cout << "error error error error ... RETURNING!" << std::endl;
					return;
				}
				pTriGeom = pTri;
			}

			// DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD
			// dependencies DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD

			TransformDependent *pTemp = new TransformDependent();
			pTemp->setup( (*childit).second.get(), pMForm );
			((*childit).second->transform).addDependent(pTemp);
			mDependents.push_back( pTemp );

			//((*childit).second->visible).addDependent(pTemp);

			// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
			// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx material found?
			// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

			std::cout << " " << std::endl;
			std::cout << "[MATERIAL] Looking for cgkit material" << std::endl;
			support3d::GLMaterial *tempMaterial = (support3d::GLMaterial *)((*childit).second.get()->getMaterial(0).get());
			if ( tempMaterial != 0 )
			{
				unsigned int temp_mat_key = (unsigned int)( tempMaterial );
				std::cout << "[MATERIAL] Detected a cgkit material with key " << temp_mat_key  << std::endl;

				if( findResource(temp_mat_key) )
				{
					// material already exists
					// not much work here as most structures are already created 
					// (shaders, textures, material)
					// ----------------------------- 
					// todo: to check does using the same cgkit-material
					// imply using the same osg state set?
					std::cout << "[MATERIAL]    Corresponding OSG material already exists - using this one" << std::endl;
					//MaterialDependent *pMD = (MaterialDependent *)(mDependentsMap.find(temp_mat_key)->second);
					
					if(findResource(temp_mat_key)->type != RT_MATERIAL)
					{
						std::cout << "[MATERIAL]    Resource loading failed" << std::endl;
					}
					osg::StateSet *pNewStateSet = (osg::StateSet *)(findResource(temp_mat_key)->pPtr);
					pGeode->setStateSet( pNewStateSet ); // M
				}
				else
				{	
					// _______________________________________________
					// material was not created, must create a new one
					
					osg::StateSet *pStateSet = new osg::StateSet();
					pGeode->setStateSet( pStateSet );

					std::cout << "[MATERIAL]    Creating a fresh new osg material type" << std::endl;
					osg::Material *pMaterial = new osg::Material(); 
					pStateSet->setAttribute(pMaterial);

					MaterialDependent *pMatDep = new MaterialDependent();
					pMatDep->setup(tempMaterial, pMaterial, pStateSet);
					(tempMaterial->ambient).addDependent(pMatDep);
					(tempMaterial->diffuse).addDependent(pMatDep);
					(tempMaterial->specular).addDependent(pMatDep);
					(tempMaterial->shininess).addDependent(pMatDep);
					(tempMaterial->emission).addDependent(pMatDep);

					mDependents.push_back( pMatDep );

					// _________________
					// checking textures

					std::cout << " " << std::endl;
					std::cout << "[TEXTURES] Checking for cgkit textures" << std::endl;

					if( tempMaterial->getNumTextures() <= 0)
					{
						std::cout << "[TEXTURES]    No textures detected!" << std::endl;
					}
					else
					{
						unsigned int temp_tex_key = 0;
						boost::shared_ptr<support3d::GLTexture> texture = tempMaterial->getTexture(0);

						temp_tex_key = (unsigned int)( texture.get() );

						if( temp_tex_key )
						{
							std::cout << "[TEXTURES]    Found a cgkit texture object" << std::endl;
							if( findResource(temp_tex_key))
							{
								std::cout << "[TEXTURES]    Texture already loaded - using this one" << std::endl;
								// use the already loaded texture ...	
								osg::Texture2D* pTex = (osg::Texture2D*)(findResource(temp_tex_key)->pPtr);
								pStateSet->setTextureAttributeAndModes(0, pTex, osg::StateAttribute::ON);
								
								// ... and modifiy the state set only as needed

								// TODO: Rethink what should be set and what can be omitted
								pStateSet->setTextureAttributeAndModes(0, pTex, osg::StateAttribute::ON);

								osg::TexEnv *tex_en = new osg::TexEnv((osg::TexEnv::Mode)(tempMaterial->getTexture(0)->mode));
								tex_en->setColor( osg::Vec4(tempMaterial->getTexture(0)->texenvcolor.x, 
															tempMaterial->getTexture(0)->texenvcolor.y,
															tempMaterial->getTexture(0)->texenvcolor.z,
															tempMaterial->getTexture(0)->texenvcolor.w) );
								pStateSet->setAttribute(tex_en);

								/// TODO: ... Environment map flag
								// bool environment_map;
								/// TODO: ... Mip map flag
								// bool mipmap;
								/// TODO: ... Texture coordinate transformation matrix
								// mat4d transform;
							}
							else
							{
								std::cout << "[TEXTURES]    Texture not present - loading from image from file" << std::endl;
								std::cout << "[TEXTURES]    " << texture->getImageName() << std::endl;
								
								osg::Image* pTextureImage = 0;
								pTextureImage = osgDB::readImageFile( texture->getImageName() );
								
								if( (!pTextureImage) )
									std::cout << "[TEXTURES]    Could not load/find the texture, check image name and file name!" << std::endl;
								
								osg::Texture2D* pTex = new osg::Texture2D;
								pTex->setDataVariance(osg::Object::DYNAMIC);
								pTex->setImage( pTextureImage );
								pStateSet->setTextureAttributeAndModes(0, pTex, osg::StateAttribute::ON);

								pTex->setInternalFormat(tempMaterial->getTexture(0)->internalformat);
								pTex->setWrap( osg::Texture::WRAP_S, (osg::Texture::WrapMode)(tempMaterial->getTexture(0)->wrap_s));
								pTex->setWrap( osg::Texture::WRAP_T, (osg::Texture::WrapMode)(tempMaterial->getTexture(0)->wrap_t));
								pTex->setFilter( osg::Texture::MIN_FILTER, (osg::Texture::FilterMode)(tempMaterial->getTexture(0)->min_filter) );
								pTex->setFilter( osg::Texture::MAG_FILTER, (osg::Texture::FilterMode)(tempMaterial->getTexture(0)->mag_filter) );

								osg::TexEnv *tex_en = new osg::TexEnv((osg::TexEnv::Mode)(tempMaterial->getTexture(0)->mode));
								tex_en->setColor( osg::Vec4(tempMaterial->getTexture(0)->texenvcolor.x, 
															tempMaterial->getTexture(0)->texenvcolor.y,
															tempMaterial->getTexture(0)->texenvcolor.z,
															tempMaterial->getTexture(0)->texenvcolor.w) );
								pStateSet->setAttribute(tex_en);

								/// TODO: ... Environment map flag
								// bool environment_map;
								/// TODO: ... Mip map flag
								// bool mipmap;
								/// TODO: ... Texture coordinate transformation matrix
								// mat4d transform;

								addResource(temp_tex_key, RT_TEXTURE, (void *)pTex );
							}
						}
					}

					// _______________
					// checking shader

					boost::shared_ptr<support3d::GLShader> vtx_shader;	// = tempMaterial->vertex_shader;
					boost::shared_ptr<support3d::GLShader> frag_shader; // = tempMaterial->fragment_shader;

					std::cout << " " << std::endl;
					std::cout << "[SHADER] Checking for programmable glslang shader" << std::endl;
					
					osg::Shader *pVtxShader = 0;
					osg::Program* pProg = new osg::Program;

					unsigned int temp_vtx_key = 0;
					int i = 0;
					bool has_shader = false;

					for( i=0; i< tempMaterial->getNumVertexShaders(); i++ ) //temp_vtx_key )
					{
						std::cout << "[SHADER]    Found a glslang/cgkit vertex shader" << std::endl;
						
						vtx_shader = tempMaterial->getVertexShader(i);
						temp_vtx_key = (unsigned int)(vtx_shader.get());
						
						if(temp_vtx_key != 0)
						{	
							has_shader = true;
							if( findResource(temp_vtx_key))
							{
								std::cout << "[SHADER]    Vertex shader already created - using existing one" << std::endl;
								pVtxShader = (osg::Shader *)(findResource(temp_vtx_key)->pPtr);
							}
							else
							{
								std::cout << "[SHADER]    Creating new osg vertex shader" << std::endl;

								pVtxShader = new osg::Shader(osg::Shader::VERTEX);
								if( !(pVtxShader->loadShaderSourceFromFile((const std::string)vtx_shader->filename) ))
									std::cout << "[SHADER]    Could not load vertex shader" << std::endl;

								addResource(temp_vtx_key, RT_VERTEXSHADER, (void *)pVtxShader );
							}
   
							// _________________________
							// HANDLE UNIFORM PARAMETERS

							std::cout << "[SHADER]    Searching for uniform parameters, vertex shader object has " << vtx_shader->numSlots() << " slots."<< std::endl;
							support3d::SlotIterator it = vtx_shader->slotsBegin();
							while( it != vtx_shader->slotsEnd() )
							{
								addUniformsByIterator(it, pStateSet);
								it++;
							}
							it = pTriGeom->slotsBegin();
							std::cout << "[SHADER]    More uniform parameters: geom object has " << pTriGeom->numSlots() << " slots."<< std::endl;
							while( it != pTriGeom->slotsEnd() )
							{
								if( findUniformDependentByName(it->first) != 0 )
									std::cout << "[SHADER]       Found a geom uniform that overwrites the shader default uniform!" << std::endl;
								it++;
							}

							// _____________________________________________________
							// using fresh one or already created one in new program

							pProg->addShader(pVtxShader);
						}
					}

					unsigned int temp_frag_key = 0;
					osg::Shader *pFragShader = 0;

					for( i=0; i< tempMaterial->getNumFragmentShaders(); i++ )
					{
						std::cout << "[SHADER]    Found a glslang/cgkit fragment shader" << std::endl;

						frag_shader = tempMaterial->getFragmentShader(i);
						temp_frag_key = (unsigned int)(frag_shader.get());

						if( temp_frag_key != 0)
						{
							has_shader = true;

							if( findResource(temp_frag_key))
							{
								std::cout << "[SHADER]    Fragment shader already created - using existing one" << std::endl;
								pFragShader = (osg::Shader *)(findResource(temp_frag_key)->pPtr);
							}
							else
							{
								pFragShader = new osg::Shader(osg::Shader::FRAGMENT);
								if( !(pFragShader->loadShaderSourceFromFile((const std::string)frag_shader->filename) ))
									std::cout << "could not load fragment shader, check filename" << std::endl;

								addResource(temp_frag_key, RT_FRAGMENTSHADER, (void *)pFragShader );
							}

							// _________________________
							// HANDLE UNIFORM PARAMETERS

							std::cout << "[SHADER]    Searching for uniform parameters, fragment shader object has " << frag_shader->numSlots() << " slots."<< std::endl;
							support3d::SlotIterator it = frag_shader->slotsBegin();
							while( it != frag_shader->slotsEnd() )
							{
								addUniformsByIterator(it, pStateSet);
								it++;
							}

							// _____________________________________________________
							// using fresh one or already created one in new program

							pProg->addShader(pFragShader);

						}

					}

					// Link Shader Objects into a Program, add Program to state set
					if( has_shader )
						pStateSet->setAttributeAndModes(pProg, osg::StateAttribute::ON);
					else
					{
						// TODO:
						// delete pProg;
					}


					// ______________________________________
					// and now think about all used resources

					addResource(temp_mat_key, RT_MATERIAL, (void *)pStateSet );
				}
			}
		}

		// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
		// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx lightsource found?
		// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

		else if(light_source!=0)
		{
			std::cout << "VISUM LIGHT found" << std::endl;

			pMForm = new osg::MatrixTransform();

			// Build the architecture
			// TODO: Apparently MatrixTransform doesnt influence light source direction/position, why?
			pSnode->addChild( pMForm );

			// Implement the GL Light stuff
			std::cout << "Building the architecture ..." << std::endl;
			osg::Light* pLight = new osg::Light;
			pLight->setLightNum( mLights );
			osg::LightSource* pLightSource = new osg::LightSource();    
			pLightSource->setLight(pLight);
			pLightSource->setLocalStateSetModes(osg::StateAttribute::ON); 
			pLightSource->setStateSetModes(*mpRootStateSet, osg::StateAttribute::ON);
			pMForm->addChild(pLightSource);
			
			support3d::GLPointLight *glp = 0; 
			glp = dynamic_cast<support3d::GLPointLight *>(light_source);
			support3d::GLDistantLight *gdl = 0;
			gdl = dynamic_cast<support3d::GLDistantLight *>(light_source);
			support3d::GLSpotLight *dsl = 0;
			dsl = dynamic_cast<support3d::GLSpotLight *>(light_source);

			// DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD
			// dependencies DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD

			std::cout << "Creating dependents ..." << std::endl;
			LightDependent *pTemp = new LightDependent();
			pTemp->setup( (*childit).second.get(), pMForm );

			OSGLIGHTTYPE theType;
			if (glp != 0)
			{
				std::cout << "Point Light" << std::endl;
				theType = DEP_POINT;

				// DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD
				// dependencies DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD

				pTemp->setupLightStuff(mLights, theType, pLightSource);
				((*childit).second->transform).addDependent(pTemp);
				((*childit).second->visible).addDependent(pTemp);
				glp->ambient.addDependent(pTemp);
				glp->diffuse.addDependent(pTemp);
				glp->specular.addDependent(pTemp);
				mDependents.push_back( pTemp );
			}
			else if (gdl != 0)
			{
				std::cout << "Directional Light" << std::endl;
				theType = DEP_DIRECTIONAL;

				// DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD
				// dependencies DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD

				pTemp->setupLightStuff(mLights, theType, pLightSource);
				((*childit).second->transform).addDependent(pTemp);
				((*childit).second->visible).addDependent(pTemp);
				gdl->ambient.addDependent(pTemp);
				gdl->diffuse.addDependent(pTemp);
				gdl->specular.addDependent(pTemp); 
				mDependents.push_back( pTemp );
			}
			else if (dsl != 0)
			{
				std::cout << "Spot Light" << std::endl;
				theType = DEP_SPOT;

				// DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD
				// dependencies DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD

				pTemp->setupLightStuff(mLights, theType, pLightSource);
				((*childit).second->transform).addDependent(pTemp);
				((*childit).second->visible).addDependent(pTemp);
				dsl->ambient.addDependent(pTemp);
				dsl->diffuse.addDependent(pTemp);
				dsl->specular.addDependent(pTemp);
				dsl->cutoff.addDependent(pTemp);
				dsl->exponent.addDependent(pTemp);
				mDependents.push_back( pTemp );
			}
			else
			{
				std::cout << "light error, could not detect light type" << std::endl;
			}

			// increase light counter
			// TODO: Watch and warn maximum lights #!
			mLights++;
		}

		// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
		// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx simple group node?
		// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

		else
		{
			std::cout << "VISUM ETC found" << std::endl;

			pMForm = new osg::MatrixTransform();

			// Build the architecture
			pSnode->addChild( pMForm );

			// TODO: IMPLEMENT IT THE OSG-WAY
			TransformDependent *pTemp = new TransformDependent();
			pTemp->setup( (*childit).second.get(), pMForm );

			// Make it transform dependent and visible dependent
			((*childit).second->transform).addDependent(pTemp);
			((*childit).second->visible).addDependent(pTemp);

			// Finally, store it in our dependents vector
			mDependents.push_back( pTemp );
		}

		// Recursion: do the same with each child node
		if (pMForm == 0)
		{
			std::cout << "error 82379ADJ" << std::endl;
			return; 
		} 
		buildTree( *(childit->second), pMForm );

		// Back from recursion 
		childit++;
	} // end while 
 
} 

//////////////////////////////////////////////////////////////
 //////////////////////////////////////////////////////////////
  ///////////////////////////// -- /////////////////////////////
   //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////

void OsgCore::setupCamera(support3d::WorldObject& visumRoot, support3d::WorldObject& cam, 
						  float nearclip, float farclip, float fov, 
						  unsigned int windowW, unsigned int windowH )
{
	std::cout << "" << std::endl;
	std::cout << "[CAMERA] Creating the camera for viewer" << std::endl;

	float ratio = (float)windowW / (float)windowH;
	TransformDependent *pDep = new TransformDependent();
	mpCameraMatrixTransform = new osg::MatrixTransform();
	pDep->setup(&cam, mpCameraMatrixTransform); 

	cam.transform.addDependent( pDep ); 
	mDependents.push_back( pDep ); 

	////////////////////////////////////////////////////////////////// 
	///////////////////////////////////////////////////////
	///////////////////////// ceating the vewer

	std::cout << "[CAMERA]    Configuring ..." << std::endl;
	Producer::CameraConfig* pCameraConfig = new Producer::CameraConfig();
	Producer::Camera* pCamera = new Producer::Camera();
	pCamera->setRenderSurfaceWindowRectangle(32.0f, 32.0f, (float)windowW, (float)windowH);
	pCamera->setLensPerspective(fov*ratio, fov, nearclip, farclip);
	Producer::RenderSurface* pSurf = pCamera->getRenderSurface();
	pSurf->setWindowName(std::string("OSG VIEWER FOR CGKIT"));
	pSurf->fullScreen(false);
	pCameraConfig->addCamera("maincam", pCamera);

	std::cout << "" << std::endl;
	std::cout << "[VIEWER] Creating the viewer from camera" << std::endl;
	theViewer = osgProducer::Viewer(pCameraConfig);
 
	std::cout << "[VIEWER]    Configuring the viewer" << std::endl;
	theViewer.setUpViewer(osgProducer::Viewer::STANDARD_SETTINGS);


	std::cout << "[VIEWER]    Creating the custom event handler" << std::endl;
	mpEH = new MyEventHandler();
	theViewer.getEventHandlerList().push_front(mpEH); 
	
	//mpRoot = 0;
	std::cout << "" << std::endl;
	std::cout << "[SCENE GRAPH] Initializing osg scene graph" << std::endl;
	mpRoot = new osg::MatrixTransform();
	mpRootStateSet = new osg::StateSet();
	mpRoot->setStateSet(mpRootStateSet);

	std::cout << "[SCENE GRAPH]    Constructing ..." << std::endl;
	std::cout << "" << std::endl;
	buildTree(visumRoot, mpRoot); // BUILD OSG SCENE GRAPH

	std::cout << "[SCENE GRAPH]    Setting scene graph data" << std::endl;
	theViewer.setSceneData( mpRoot );
	theViewer.realize();
	//////////////////////////////////////////////////////////////////

	std::cout << "[SCENE GRAPH]    Starting viewer" << std::endl;
	std::cout << "" << std::endl;
};

void OsgCore::renderFrame()		
{								
   if( !theViewer.done() )		
   {							
      theViewer.sync();			
      theViewer.update();		
	  osg::Matrixd M = mpCameraMatrixTransform->getInverseMatrix();
	  //std::cout << mpCameraMatrixTransform->getMatrix() << std::endl;
	  //std::cout << M << std::endl;
	  theViewer.setViewByMatrix( Producer::Matrix(M.ptr()) * Producer::Matrix::rotate( osg::DegreesToRadians(180.0), 0, 1, 0 ) );
	  //theViewer.setViewByMatrix( Producer::Matrix::translate(0.0f, 0.0f, -5.0f) );
      theViewer.frame();		
   }							
}								


//////////////////////////////////////////
//   this is just a test method			//
//   nothing really useful inside here	//
//////////////////////////////////////////
void OsgCore::fooz() 
{
	osg::Geode* pyramidGeode = 0;
	osg::Geometry* pyramidGeometry = 0; 
	osg::Group* root = 0;
	std::cout << "STARTING!" << std::endl;
   osgProducer::Viewer viewer;
   std::cout << "checkpoint a1" << std::endl;
   root = new osg::Group();
   std::cout << "checkpoint a2" << std::endl;
   pyramidGeode = new osg::Geode();
   std::cout << "checkpoint a3" << std::endl;
   pyramidGeometry = new osg::Geometry();
   std::cout << "checkpoint a4" << std::endl;

   std::cout << "checkpoint a5" << std::endl;

   //Associate the pyramid geometry with the pyramid geode 
   //   Add the pyramid geode to the root node of the scene graph.

   pyramidGeode->addDrawable(pyramidGeometry); 
   root->addChild(pyramidGeode);

   //Declare an array of vertices. Each vertex will be represented by 
   //a triple -- an instances of the vec3 class. An instance of 
   //osg::Vec3Array can be used to store these triples. Since 
   //osg::Vec3Array is derived from the STL vector class, we can use the
   //push_back method to add array elements. Push back adds elements to 
   //the end of the vector, thus the index of first element entered is 
   //zero, the second entries index is 1, etc.
   //Using a right-handed coordinate system with 'z' up, array 
   //elements zero..four below represent the 5 points required to create 
   //a simple pyramid.

   osg::Vec3Array* pyramidVertices = new osg::Vec3Array;
   pyramidVertices->push_back( osg::Vec3( 0, 0, 0) ); // front left 
   pyramidVertices->push_back( osg::Vec3(10, 0, 0) ); // front right 
   pyramidVertices->push_back( osg::Vec3(10,10, 0) ); // back right 
   pyramidVertices->push_back( osg::Vec3( 0,10, 0) ); // back left 
   pyramidVertices->push_back( osg::Vec3( 5, 5,10) ); // peak

   //Associate this set of vertices with the geometry associated with the 
   //geode we added to the scene.

   pyramidGeometry->setVertexArray( pyramidVertices );

   std::cout << "checkpoint1" << std::endl;

   //Next, create a primitive set and add it to the pyramid geometry. 
   //Use the first four points of the pyramid to define the base using an 
   //instance of the DrawElementsUint class. Again this class is derived 
   //from the STL vector, so the push_back method will add elements in 
   //sequential order. To ensure proper backface cullling, vertices 
   //should be specified in counterclockwise order. The arguments for the 
   //constructor are the enumerated type for the primitive 
   //(same as the OpenGL primitive enumerated types), and the index in 
   //the vertex array to start from.

   osg::DrawElementsUInt* pyramidBase = 
      new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
   pyramidBase->push_back(3);
   pyramidBase->push_back(2);
   pyramidBase->push_back(1);
   pyramidBase->push_back(0);
   pyramidGeometry->addPrimitiveSet(pyramidBase);

   //Repeat the same for each of the four sides. Again, vertices are 
   //specified in counter-clockwise order. 

   osg::DrawElementsUInt* pyramidFaceOne = 
      new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
   pyramidFaceOne->push_back(0);
   pyramidFaceOne->push_back(1);
   pyramidFaceOne->push_back(4);
   pyramidGeometry->addPrimitiveSet(pyramidFaceOne);

   osg::DrawElementsUInt* pyramidFaceTwo = 
      new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
   pyramidFaceTwo->push_back(1);
   pyramidFaceTwo->push_back(2);
   pyramidFaceTwo->push_back(4);
   pyramidGeometry->addPrimitiveSet(pyramidFaceTwo);

   std::cout << "checkpoint2" << std::endl;

   osg::DrawElementsUInt* pyramidFaceThree = 
      new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
   pyramidFaceThree->push_back(2);
   pyramidFaceThree->push_back(3);
   pyramidFaceThree->push_back(4);
   pyramidGeometry->addPrimitiveSet(pyramidFaceThree);

   osg::DrawElementsUInt* pyramidFaceFour = 
      new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
   pyramidFaceFour->push_back(3);
   pyramidFaceFour->push_back(0);
   pyramidFaceFour->push_back(4);
   pyramidGeometry->addPrimitiveSet(pyramidFaceFour);

   //Declare and load an array of Vec4 elements to store colors. 

   std::cout << "checkpoint3" << std::endl;

   osg::Vec4Array* colors = new osg::Vec4Array;
   colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); //index 0 red
   colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f) ); //index 1 green
   colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f) ); //index 2 blue
   colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) ); //index 3 white

   //Declare the variable that will match vertex array elements to color 
   //array elements. This vector should have the same number of elements 
   //as the number of vertices. This vector serves as a link between 
   //vertex arrays and color arrays. Entries in this index array 
   //coorespond to elements in the vertex array. Their values coorespond 
   //to the index in he color array. This same scheme would be followed 
   //if vertex array elements were matched with normal or texture 
   //coordinate arrays.
   //   Note that in this case, we are assigning 5 vertices to four 
   //   colors. Vertex array element zero (bottom left) and four (peak) 
   //   are both assigned to color array element zero (red).

   osg::TemplateIndexArray
      <unsigned int, osg::Array::UIntArrayType,4,4> *colorIndexArray;
   colorIndexArray = 
      new osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType,4,4>;
   colorIndexArray->push_back(0); // vertex 0 assigned color array element 0
   colorIndexArray->push_back(1); // vertex 1 assigned color array element 1
   colorIndexArray->push_back(2); // vertex 2 assigned color array element 2
   colorIndexArray->push_back(3); // vertex 3 assigned color array element 3
   colorIndexArray->push_back(0); // vertex 4 assigned color array element 0

   //The next step is to associate the array of colors with the geometry, 
   //assign the color indices created above to the geometry and set the 
   //binding mode to _PER_VERTEX.

   std::cout << "checkpoint4" << std::endl;

   pyramidGeometry->setColorArray(colors);
   pyramidGeometry->setColorIndices(colorIndexArray);
   pyramidGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

   //Now that we have created a geometry node and added it to the scene 
   //we can reuse this geometry. For example, if we wanted to put a 
   //second pyramid 15 units to the right of the first one, we could add 
   //this geode as the child of a transform node in our scene graph. 

   // Declare and initialize a transform node.
   osg::PositionAttitudeTransform* pyramidTwoXForm =
      new osg::PositionAttitudeTransform();

   // Use the 'addChild' method of the osg::Group class to
   // add the transform as a child of the root node and the
   // pyramid node as a child of the transform.

   root->addChild(pyramidTwoXForm);
   pyramidTwoXForm->addChild(pyramidGeode);

   std::cout << "checkpoint5" << std::endl;

   // Declare and initialize a Vec3 instance to change the
   // position of the tank model in the scene

   osg::Vec3 pyramidTwoPosition(15,0,0);
   pyramidTwoXForm->setPosition( pyramidTwoPosition ); 

   //The final step is to set up and enter a simulation loop.

   viewer.setUpViewer(osgProducer::Viewer::STANDARD_SETTINGS);
   viewer.setSceneData( root );

   viewer.realize();

   std::cout << "VIEWING!" << std::endl;

   while( !viewer.done() )
   {
      viewer.sync();
      viewer.update();
      viewer.frame();
   } 

   std::cout << "DONE!" << std::endl;
};


