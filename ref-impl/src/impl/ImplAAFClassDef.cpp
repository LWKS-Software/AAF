//=---------------------------------------------------------------------=
//
// $Id$ $Name$
//
// The contents of this file are subject to the AAF SDK Public Source
// License Agreement Version 2.0 (the "License"); You may not use this
// file except in compliance with the License.  The License is available
// in AAFSDKPSL.TXT, or you may obtain a copy of the License from the
// Advanced Media Workflow Association, Inc., or its successor.
//
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See
// the License for the specific language governing rights and limitations
// under the License.  Refer to Section 3.3 of the License for proper use
// of this Exhibit.
//
// WARNING:  Please contact the Advanced Media Workflow Association,
// Inc., for more information about any additional licenses to
// intellectual property covering the AAF Standard that may be required
// to create and distribute AAF compliant products.
// (http://www.amwa.tv/policies).
//
// Copyright Notices:
// The Original Code of this file is Copyright 1998-2009, licensor of the
// Advanced Media Workflow Association.  All rights reserved.
//
// The Initial Developer of the Original Code of this file and the
// licensor of the Advanced Media Workflow Association is
// Avid Technology.
// All rights reserved.
//
//=---------------------------------------------------------------------=

// Conditional symbol for testing the creation of a new optional 
// strong reference set property on the dictionary. This is experimental
// because the AAFDictionary is still considered "axiomatic" by
// ImplAAFBuiltinClasses and itself. The problem is that the
// class definitions for such axiomatic objects are unpersisted
// but NOT used by the DM, the "builtin" class definition is used instead!
// 2000-SEPT-14 transdel.
#ifndef SUPPORT_EXPERIMENTAL_OPTIONAL_SETS
#define SUPPORT_EXPERIMENTAL_OPTIONAL_SETS 0
#endif


#ifndef __ImplEnumAAFPropertyDefs_h__
#include "ImplEnumAAFPropertyDefs.h"
#endif

#include "ImplAAFSmartPointer.h"
typedef ImplAAFSmartPointer<ImplEnumAAFPropertyDefs> ImplEnumAAFPropertyDefsSP;

#ifndef __ImplAAFPropertyDef_h__
#include "ImplAAFPropertyDef.h"
#endif

#ifndef __ImplAAFTypeDef_h__
#include "ImplAAFTypeDef.h"
#endif

#ifndef __ImplAAFClassDef_h__
#include "ImplAAFClassDef.h"
#endif

#ifndef __ImplAAFDictionary_h__
#include "ImplAAFDictionary.h"
#endif

#ifndef __ImplAAFMetaDictionary_h__
#include "ImplAAFMetaDictionary.h"
#endif

#include "AAFStoredObjectIDs.h"
#include "AAFPropertyIDs.h"
#include "ImplAAFObjectCreation.h"
#include "ImplAAFBuiltinDefs.h"
#include "AAFUtils.h"

#include "OMAssertions.h"
#include <string.h>

extern "C" const aafClassID_t CLSID_EnumAAFPropertyDefs;
extern "C" const aafClassID_t CLSID_AAFObject;

//
// locals obj mgmt utils
//
#define check_result(result)     \
  if (AAFRESULT_FAILED (result)) \
	return result;

/* Note!  Will modify argument... */
#define release_if_set(pIfc)     \
{                                \
  if (pIfc)                      \
	{                            \
	  pIfc->ReleaseReference (); \
	  pIfc = 0;                  \
	}                            \
}

/* Note!  Will modify argument... */
#define release_and_zero(pIfc)   \
{                                \
  ASSERTU (pIfc);                \
  pIfc->ReleaseReference ();     \
  pIfc = 0;                      \
}



ImplAAFClassDef::ImplAAFClassDef ()
  : _ParentClass  ( PID_ClassDefinition_ParentClass,  
                    L"ParentClass",
                    L"/MetaDictionary/ClassDefinitions", 
                    PID_MetaDefinition_Identification),
    _Properties   ( PID_ClassDefinition_Properties,
                    L"Properties",
                    PID_MetaDefinition_Identification),
    _IsConcrete   ( PID_ClassDefinition_IsConcrete,   
                    L"IsConcrete"),
	_BootstrapParent(0),
	_propTypesLoaded (false)
{
  _persistentProperties.put(_ParentClass.address());
  _persistentProperties.put(_Properties.address());
  _persistentProperties.put(_IsConcrete.address());
}


ImplAAFClassDef::~ImplAAFClassDef ()
{
	OMStrongReferenceSetIterator<OMUniqueObjectIdentification, ImplAAFPropertyDef>propertyDefinitions(_Properties);
	while(++propertyDefinitions)
	{
		ImplAAFPropertyDef *pProperty = propertyDefinitions.clearValue();
		if (pProperty)
		{
		  pProperty->ReleaseReference();
		  pProperty = 0;
		}
	}
}


AAFRESULT STDMETHODCALLTYPE
    ImplAAFClassDef::Initialize (
      const aafUID_t & classID,
      ImplAAFClassDef * pParentClass,
      const aafCharacter * pClassName,
	  aafBool isConcrete)
{
  if (!pClassName) return AAFRESULT_NULL_PARAM;

  return pvtInitialize (classID, pParentClass, pClassName, isConcrete);
}


AAFRESULT STDMETHODCALLTYPE
    ImplAAFClassDef::pvtInitialize (
      const aafUID_t & classID,
      ImplAAFClassDef * pParentClass,
      const aafCharacter * pClassName,
	  aafBool isConcrete)
{
//	ImplAAFClassDef	*oldParent;
  if (!pClassName) return AAFRESULT_NULL_PARAM;	
  if (pParentClass && pParentClass != this && !pParentClass->attached())  // a root class has itself as a parent
    return AAFRESULT_OBJECT_NOT_ATTACHED;	
	
  HRESULT hr;
  hr = ImplAAFMetaDefinition::Initialize(classID, pClassName, NULL);
  if (AAFRESULT_FAILED (hr))
    return hr;
	
  SetParent (pParentClass);
  pvtSetIsConcrete (isConcrete);

  return AAFRESULT_SUCCESS;
}


aafBool ImplAAFClassDef::pvtIsConcrete () const
{
  return _IsConcrete;
}

void ImplAAFClassDef::pvtSetIsConcrete (aafBoolean_t isConcrete)
{
  _IsConcrete = isConcrete;
}

AAFRESULT STDMETHODCALLTYPE
    ImplAAFClassDef::GetPropertyDefs (
      ImplEnumAAFPropertyDefs ** ppEnum)
{
  ImplEnumAAFPropertyDefs * theEnum = 0;

  if (NULL == ppEnum)
	return AAFRESULT_NULL_PARAM;

  theEnum = (ImplEnumAAFPropertyDefs *)CreateImpl (CLSID_EnumAAFPropertyDefs);
  if (NULL == theEnum)
  	return AAFRESULT_NOMEMORY;

  AAFRESULT hr;
  OMStrongReferenceSetIterator<OMUniqueObjectIdentification, ImplAAFPropertyDef>* iter = 
	new OMStrongReferenceSetIterator<OMUniqueObjectIdentification, ImplAAFPropertyDef>(_Properties);
  if(iter == 0)
	hr = AAFRESULT_NOMEMORY;
  else
	hr = theEnum->Initialize(&CLSID_EnumAAFPropertyDefs,this,iter);
  if (AAFRESULT_FAILED (hr))
	{
		theEnum->ReleaseReference();
		theEnum = NULL;
		return hr;
	}
	
  *ppEnum = theEnum;

  return AAFRESULT_SUCCESS;
}

AAFRESULT STDMETHODCALLTYPE
    ImplAAFClassDef::InitOMPropertiesForObject (
      ImplAAFObject *obj)
{
  OMObject		*omObj;
  ImplAAFPropertyDef	*propDef;
  OMPropertySet *ps;

 // if (!_Properties.isPresent())
  {
	OMStrongReferenceSetIterator<OMUniqueObjectIdentification, ImplAAFPropertyDef>iter(_Properties);
	ps = obj->propertySet();
	while(++iter)
	{
	  omObj = iter.currentObject();
	  propDef = static_cast<ImplAAFPropertyDef*>(omObj);
	  obj->InitOMProperty(propDef, ps);
	}
  }

  return AAFRESULT_SUCCESS;
}

AAFRESULT STDMETHODCALLTYPE
    ImplAAFClassDef::CountPropertyDefs (
      aafUInt32 *  pCount)
{
  if (! pCount) return AAFRESULT_NULL_PARAM;

  ASSERTU (pCount);
  if (!_Properties.isPresent())
	*pCount = 0;

  else  *pCount = _Properties.count ();
	
  return AAFRESULT_SUCCESS;
}


AAFRESULT STDMETHODCALLTYPE
    ImplAAFClassDef::RegisterNewPropertyDef (
      const aafUID_t &      id,
      const aafCharacter *  pName,
      ImplAAFTypeDef *      pTypeDef,
      aafBool               isOptional,
      aafBool               isIsUniqueIdentifier,
      ImplAAFPropertyDef ** ppPropDef)
{
  // This will only work if this class has not yet been registered.

  if (! pTypeDef)
	return AAFRESULT_NULL_PARAM;

  AAFRESULT hr;
  ImplAAFDictionarySP pDict;
  hr = GetDictionary (&pDict);
  if (AAFRESULT_SUCCEEDED (hr))
	{
	  // A dictionary has been identified; see if we're registered in
	  // it.
	  aafUID_t thisClassID;

	  // get our class ID
	  hr = GetAUID (&thisClassID);
	  ASSERTU (AAFRESULT_SUCCEEDED (hr));

	  if (pDict->PvtIsClassPresent(thisClassID))
		{
		  // pClassDef is unused; we only want to know the result of
		  // the lookup.
		  return AAFRESULT_OBJECT_ALREADY_ATTACHED;
		}
	}

  // If we're here, we're not already registered.  OK to continue.

  aafUID_t typeId;
  hr = pTypeDef->GetAUID (&typeId);
  if (AAFRESULT_FAILED (hr))
	return hr;

  // Check that this class or any parent class is already uniquely
  // identified.
  if (isIsUniqueIdentifier)
  {
    aafBoolean_t alreadyUniquelyIdentified = kAAFFalse;
    hr = IsUniquelyIdentified(&alreadyUniquelyIdentified);
    if (AAFRESULT_FAILED(hr))
      return hr;
    
    // "There can be only one" unique identifier per class.
    if (alreadyUniquelyIdentified)
      return AAFRESULT_ALREADY_UNIQUELY_IDENTIFIED;
  }

	//GMSB! 05-Dec-2001 Documentation says AAFRESULT_BAD_PARAM for this case (but AAFRESULT_BAD_PARAM doesn't really exist!)
	//  and the code had AAFRESULT_SUCCESS before (which doesn't seem right, because ppPropDef won't have been filled in)
	if(PvtIsPropertyDefRegistered(id))
		return AAFRESULT_INVALID_PARAM;
	else
	{
		bool	isDuplicate;
		
		hr = pDict->PvtIsPropertyDefDuplicate(id, this, &isDuplicate);
		if (AAFRESULT_FAILED(hr))
			return hr;
		if(isDuplicate)
			return AAFRESULT_PROPERTY_DUPLICATE;
		return pvtRegisterPropertyDef (id,
									pName,
									typeId,
									isOptional,
									isIsUniqueIdentifier,
									ppPropDef);
	}
}	


AAFRESULT STDMETHODCALLTYPE
    ImplAAFClassDef::RegisterOptionalPropertyDef (
      const aafUID_t &      id,
      const aafCharacter *  pName,
      ImplAAFTypeDef *      pTypeDef,
      ImplAAFPropertyDef ** ppPropDef)
{
  AAFRESULT hr;
  if (ppPropDef)
    *ppPropDef = NULL;
  if (! pTypeDef)
	return AAFRESULT_NULL_PARAM;

  ImplAAFDictionarySP pDict;
  hr = GetDictionary (&pDict);
  ASSERTU (AAFRESULT_SUCCEEDED (hr));

  aafUID_t myAuid;
  hr = GetAUID (&myAuid);
  ASSERTU (AAFRESULT_SUCCEEDED (hr));

  // Test to see if this class is axiomatic; if so, then we can't
  // augment it.
  if (pDict->IsAxiomaticClass (myAuid))
  {
		return AAFRESULT_NOT_EXTENDABLE;
	}

  aafUID_t typeId;
  hr = pTypeDef->GetAUID (&typeId);
  if (AAFRESULT_FAILED (hr))
	return hr;

	//GMSB! 05-Dec-2001 Documentation says AAFRESULT_BAD_PARAM for this case (but AAFRESULT_BAD_PARAM doesn't really exist!)
	//  and the code had AAFRESULT_SUCCESS before (which doesn't seem right, because ppPropDef won't have been filled in)
	if(PvtIsPropertyDefRegistered(id))
		return AAFRESULT_INVALID_PARAM;
	else
	{
		bool	isDuplicate;
		
		hr = pDict->PvtIsPropertyDefDuplicate(id, this, &isDuplicate);
		if (AAFRESULT_FAILED(hr))
			return hr;
		if(isDuplicate)
			return AAFRESULT_PROPERTY_DUPLICATE;
		ImplAAFPropertyDefSP pNewPropertyDef;
	  hr = pvtRegisterPropertyDef (id,
								 pName,
								 typeId,
								 kAAFTrue,
								 kAAFFalse, /* cannot be a unique identifier */
								 &pNewPropertyDef);
								 
		if (AAFRESULT_SUCCEEDED(hr))
		{
		  // Check to make sure that the new optional property
		  // is complete for this class defintion.
		  hr = pNewPropertyDef->CompleteClassRegistration();
		  if (AAFRESULT_FAILED(hr))
		  {
		    // remove the invalid property definition from the set of properties.
		    _Properties.removeValue(pNewPropertyDef);
		    pNewPropertyDef->ReleaseReference(); // remove the reference from the set.
		  }
		  else
		  {
		    if (ppPropDef)
		    {
		      *ppPropDef = pNewPropertyDef;
		      (*ppPropDef)->AcquireReference();
		    }
		  }
		}
	  		 
    return hr;	  
	}
}


  // Low-level recursive methods the OM find method instead of a linear search
  // that will necessarily load all of the property definitions for the class
  // definition instance.
AAFRESULT
    ImplAAFClassDef::generalLookupPropertyDef (
      aafUID_constref propId,
      ImplAAFPropertyDef ** ppPropDef)
{
	AAFRESULT result = AAFRESULT_SUCCESS;
  // NOTE: The following type cast is temporary. It should be removed as soon
	// as the OM has a declarative sytax to include the type
	// of the key used in the set. (trr:2000-MAR-11)
	if (_Properties.find((*reinterpret_cast<const OMObjectIdentification *>(&propId)),
                             *ppPropDef))
	{
		ASSERTU(NULL != *ppPropDef);
    (*ppPropDef)->AcquireReference();
	}
	else
	{
    // no recognized class guid in dictionary
    result = AAFRESULT_NO_MORE_OBJECTS; // s/b AAFRESULT_PROPERTY_NOT_FOUND
	}

	return (result);
}

/******/
bool
    ImplAAFClassDef::PvtIsPropertyDefRegistered (
      aafUID_constref propId)
{
  // NOTE: The following type cast is temporary. It should be removed as soon
	// as the OM has a declarative sytax to include the type
	// of the key used in the set. (trr:2000-MAR-11)
	return (_Properties.contains((*reinterpret_cast<const OMObjectIdentification *>(&propId))));
}

AAFRESULT /*STDMETHODCALLTYPE*/
    ImplAAFClassDef::generalLookupPropertyDef (
      const pvtPropertyIdentifier & propId,
      ImplAAFPropertyDef ** ppPropDef)
{
		///!!!JeffB: Leave this in original loop form until the OM  genericizes the lookup key
  if (! ppPropDef)
	return AAFRESULT_NULL_PARAM;

  ImplEnumAAFPropertyDefsSP pPropEnum;
  ImplAAFPropertyDefSP pPropDef;

  check_result (GetPropertyDefs (&pPropEnum));
  ASSERTU (pPropEnum);

  aafUInt32 count;
  check_result (CountPropertyDefs (&count));
  aafUInt32 i;
  for (i = 0; i < count; i++)
	{
	  ASSERTU (pPropEnum);
	  check_result (pPropEnum->NextOne (&pPropDef));
	  ASSERTU (pPropDef);
	  if (propId.DoesMatch (pPropDef))
		{
		  // Yup, this is the one.
		  ASSERTU (ppPropDef);
		  *ppPropDef = pPropDef;
		  ASSERTU (*ppPropDef);
		  (*ppPropDef)->AcquireReference ();

		  // Get us outta the loop
		  return AAFRESULT_SUCCESS;
		}
	  else
		{
		  // nope, this prop doesn't match.  Try the next one.
		  pPropDef = 0;
		}
	}
  // If we're here, there aren't any more props so that prop
  // wasn't found in this class; try any base classes.
  ImplAAFClassDefSP parentSP;
  aafBool			isRootObject;
  AAFRESULT hr = IsRoot (&isRootObject);
  if (AAFRESULT_FAILED (hr))
	  return hr;
  if(isRootObject)
	  return AAFRESULT_NO_MORE_OBJECTS;
  else
  {
	  hr = GetParent (&parentSP);
	  if (AAFRESULT_FAILED (hr))
	     return hr;
      return parentSP->generalLookupPropertyDef (propId, ppPropDef);
  }
}


AAFRESULT STDMETHODCALLTYPE
    ImplAAFClassDef::LookupPropertyDef (
      const aafUID_t & propID,
      ImplAAFPropertyDef ** ppPropDef) const
{
  const pvtPropertyIdentifierAUID generalPropId = propID;

  // cast away bitwise const-ness; maintaining conceptual const-ness
  return ((ImplAAFClassDef*)this)->generalLookupPropertyDef
	(generalPropId, ppPropDef);
}



AAFRESULT STDMETHODCALLTYPE
    ImplAAFClassDef::LookupPropertyDefbyOMPid (
      OMPropertyId omPid,
      ImplAAFPropertyDef ** ppPropDef) const
{
  const pvtPropertyIdentifierOMPid generalPropId = omPid;

  return ((ImplAAFClassDef*)this)->generalLookupPropertyDef
	(generalPropId, ppPropDef);
}


AAFRESULT STDMETHODCALLTYPE
    ImplAAFClassDef::GetParent (
      ImplAAFClassDef ** ppClassDef)
{
	aafBool		isRoot;
	if (! ppClassDef) return AAFRESULT_NULL_PARAM;
	
	IsRoot (&isRoot);
	if(isRoot)
		return AAFRESULT_IS_ROOT_CLASS;

	*ppClassDef = bootstrapClassWeakReference(_ParentClass);
	ASSERTU(*ppClassDef != NULL);
	if(*ppClassDef != NULL)
		(*ppClassDef)->AcquireReference ();
	
	return AAFRESULT_SUCCESS;
}

AAFRESULT STDMETHODCALLTYPE
    ImplAAFClassDef::IsRoot (
      aafBool * pIsRoot)
{
	if (! pIsRoot) return AAFRESULT_NULL_PARAM;

	if (hasParent())
	  *pIsRoot = kAAFFalse;
	else
	  *pIsRoot = kAAFTrue;

	return AAFRESULT_SUCCESS;
}


AAFRESULT STDMETHODCALLTYPE
    ImplAAFClassDef::IsConcrete (
      aafBool * pResult)
{
	if (! pResult) return AAFRESULT_NULL_PARAM;
	*pResult = pvtIsConcrete ();
	return AAFRESULT_SUCCESS;
}


// Check that this class or any parent class is already uniquely
// identified.
AAFRESULT STDMETHODCALLTYPE
    ImplAAFClassDef::IsUniquelyIdentified (
      aafBoolean_t * isUniquelyIdentified)
{
	if (! isUniquelyIdentified)
    return AAFRESULT_NULL_PARAM;
	
  if (NULL != pvtGetUniqueIdentifier())
    *isUniquelyIdentified = kAAFTrue;
  else
    *isUniquelyIdentified = kAAFFalse;


  return AAFRESULT_SUCCESS;
}


// Check that this class or any parent class is already uniquely
// identified.
AAFRESULT STDMETHODCALLTYPE
    ImplAAFClassDef::GetUniqueIdentifier (
      ImplAAFPropertyDef ** ppUniqueIdentifier)
{
	if (! ppUniqueIdentifier)
    return AAFRESULT_NULL_PARAM;

  *ppUniqueIdentifier = pvtGetUniqueIdentifier();
  if (*ppUniqueIdentifier)
  {
    (*ppUniqueIdentifier)->AcquireReference();
    return AAFRESULT_SUCCESS;
  }
  else
  {
    // The unique identifier property in property definition
    // is optional. So if our low-level search did not find
    // a property definition with this property we should 
    // return corresponding result code.
    return AAFRESULT_PROP_NOT_PRESENT;
  }
}

const OMPropertyDefinition* ImplAAFClassDef::propertyDefinition(
                            const OMUniqueObjectIdentification& id ) const
{
  aafUID_t  aafID = *(aafUID_t*)&id;
  ImplAAFPropertyDef* pDef = 0;
  LookupPropertyDef( aafID, &pDef);
  ASSERTU( pDef );

  // This method does not increment the reference count
  // of the returned property definition.
  pDef->ReleaseReference();

  return pDef;
}

PropertyDefinitionsIterator* ImplAAFClassDef::propertyDefinitions(void) const
{
  return _Properties.createIterator();
}

bool ImplAAFClassDef::hasParent(void) const
{
  aafUID_t uid;
  GetAUID (&uid);

  bool result = true;
  if( uid == AUID_AAFObject ||
      uid == AUID_AAFMetaDefinition ||
      uid == AUID_AAFMetaDictionary )
  {
      result = false;
  }


  return result;
}

const OMClassDefinition* ImplAAFClassDef::parent(void) const
{
  ASSERTU( !isRoot() );


  ImplAAFClassDef* pNonConstThis = const_cast<ImplAAFClassDef*>(this);
  ImplAAFClassDef* pParent = 0;
  pNonConstThis->GetParent( &pParent );
  ASSERTU( pParent );


  // This method does not increment the reference count
  // of the returned class definition.
  pParent->ReleaseReference();


  return pParent;
}

bool ImplAAFClassDef::isConcrete(void) const
{
  bool result = false;
  aafBool b = pvtIsConcrete();
  if (b == kAAFTrue) {
    result = true;
  }
  return result;
}

bool ImplAAFClassDef::omIsConcrete(void) const
{
    return _IsConcrete == kAAFTrue;
}
  
OMClassDefinition* ImplAAFClassDef::omParentClass(void) const
{
    ImplAAFClassDef* pParentClassDef = 0;
    
    ImplAAFClassDef* nonConstThis = const_cast<ImplAAFClassDef*>(this);
    
    aafBoolean_t isRoot = kAAFFalse;
    nonConstThis->IsRoot(&isRoot);
    if (isRoot == kAAFFalse)
    {
        HRESULT hr = nonConstThis->GetParent(&pParentClassDef);
        if (AAFRESULT_SUCCESS == hr)
        {
            // This method does not increment the reference count
            // of the returned class definition.
            pParentClassDef->ReleaseReference();
        }
        else
        {
            pParentClassDef = 0;
        }
    }
    
    return pParentClassDef;
}

bool ImplAAFClassDef::omRegisterExtPropertyDef(OMPropertyDefinition* propertyDef)
{
    if (PvtIsPropertyDefRegistered(*(reinterpret_cast<const aafUID_t*>(
        &(propertyDef->identification())))))
    {
        return false;
    }
    
    HRESULT result = pvtRegisterExistingPropertyDef(dynamic_cast<ImplAAFPropertyDef*>(
        propertyDef));
        
    return AAFRESULT_SUCCEEDED(result);
}



// Find the unique identifier property defintion for this class or any parent class
// (RECURSIVE)
ImplAAFPropertyDef * ImplAAFClassDef::pvtGetUniqueIdentifier(void) // result is NOT reference counted.
{
  ImplAAFPropertyDef *result = NULL;

  aafBoolean_t isRoot, isUniquelyIdentified;
  AAFRESULT hr = IsRoot(&isRoot);
  if (AAFRESULT_FAILED(hr))
    return NULL;

  // Look for the unique identifier in the parent class(es) before looking in the
  // current class definition.
  if (!isRoot)
  {
    ImplAAFClassDefSP pParent;
    hr = GetParent(&pParent);
    if (AAFRESULT_FAILED(hr))
      return NULL;

    result = pParent->pvtGetUniqueIdentifier();
  }

  // If we have not already found the unique identifier in a parent
  // class then continue to look in this class' propert definitions.
  if (!result)
  {
	  OMStrongReferenceSetIterator<OMUniqueObjectIdentification, ImplAAFPropertyDef>propertyDefinitions(_Properties);
	  while(++propertyDefinitions)
	  {
		  ImplAAFPropertyDef *pProperty = propertyDefinitions.value();
		  if (pProperty)
		  {
		    hr = pProperty->GetIsUniqueIdentifier(&isUniquelyIdentified);
        if (AAFRESULT_FAILED(hr))
          return NULL;
        if (isUniquelyIdentified)
        {
          result = pProperty;
          break;
        }
		  }
	  }
  }

  return result;
}


// SetParent is SDK INTERNAL
AAFRESULT STDMETHODCALLTYPE
    ImplAAFClassDef::SetParent (
      ImplAAFClassDef *pClassDef)
{
	if (! pClassDef)
		return AAFRESULT_NULL_PARAM;

	_ParentClass = pClassDef;
  return AAFRESULT_SUCCESS;
}

AAFRESULT STDMETHODCALLTYPE
    ImplAAFClassDef::SetBootstrapParent (
	ImplAAFClassDef *pClassDef)
{
	if (! pClassDef)
		return AAFRESULT_NULL_PARAM;
	
	_BootstrapParent = pClassDef;
	return AAFRESULT_SUCCESS;
}

AAFRESULT STDMETHODCALLTYPE
    ImplAAFClassDef::CreateInstance (
      ImplAAFObject ** ppObject)
{
  if (! ppObject)
	return AAFRESULT_NULL_PARAM;

  if (! pvtIsConcrete ())
	return AAFRESULT_ABSTRACT_CLASS;

  AAFRESULT hr;
  ImplAAFDictionarySP pDict;
  aafUID_t thisClassID;
  // get our class ID
  hr = GetAUID (&thisClassID);
  if (AAFRESULT_FAILED (hr))
	return hr;

  hr = GetDictionary (&pDict);
  if (AAFRESULT_FAILED (hr))
	return hr;
  ASSERTU (pDict);

  // We should not create an instance of a MetaDefinition with CreateInstance().
  // CreateMetaInstance() should be used for that instead.
  if (pDict->metaDictionary()->isMeta(*reinterpret_cast<const OMObjectIdentification *>(&thisClassID)))
	return AAFRESULT_INVALID_CLASS_ID;

  ImplAAFObject * pObj =
	pDict->CreateAndInit (this);
  ASSERTU (pObj);
  *ppObject = pObj;
  // don't bother with *ppObject->addref() and pObj->ReleaseRef();
  // consider the ref count to be transferred from pObj to *ppObject.
  pObj = 0;  // to keep boundschecker happy
  return AAFRESULT_SUCCESS;
}


AAFRESULT STDMETHODCALLTYPE
    ImplAAFClassDef::pvtRegisterPropertyDef (
      const aafUID_t &      id,
      const aafCharacter *  pName,
      const aafUID_t &      typeId,
      aafBool               isOptional,
      aafBool               isUniqueIdentifier,
      ImplAAFPropertyDef ** ppPropDef)
{
  if (! pName) return AAFRESULT_NULL_PARAM;

  ImplAAFDictionarySP pDict;
  ImplAAFPropertyDefSP pd;

  check_result (GetDictionary (&pDict));
  ASSERTU (pDict);
  OMPropertyId omPid;
  check_result (pDict->GenerateOmPid (id, omPid));

  AAFRESULT hr;
  hr = pDict->CreateMetaInstance(AUID_AAFPropertyDef, (ImplAAFMetaDefinition **)&pd);
  if (AAFRESULT_FAILED (hr))
	return hr;

  check_result (pd->pvtInitialize (id,
								omPid,
								pName,
								typeId,
								isOptional,
								isUniqueIdentifier,
								this));

//  ImplAAFPropertyDef * pdTemp = pd;
//  _Properties.appendValue(pdTemp);
//  pdTemp->AcquireReference ();
  pvtRegisterExistingPropertyDef(pd);

  if (ppPropDef)
  {
	*ppPropDef = pd;
	(*ppPropDef)->AcquireReference ();
  }

  return AAFRESULT_SUCCESS;
}


AAFRESULT STDMETHODCALLTYPE
ImplAAFClassDef::pvtRegisterExistingPropertyDef
(ImplAAFPropertyDef * pPropDef)
{
  if (!pPropDef)
	return AAFRESULT_NULL_PARAM;
  if (pPropDef->attached())
	return AAFRESULT_OBJECT_ALREADY_ATTACHED;

  _Properties.appendValue(pPropDef);
  pPropDef->AcquireReference ();

  return AAFRESULT_SUCCESS;
}


//
// Implementations of private class
//
aafBool ImplAAFClassDef::pvtPropertyIdentifierOMPid:: DoesMatch
  (const ImplAAFPropertyDef * pTestPropDef) const
{
  OMPropertyId testPid;
  ASSERTU (pTestPropDef);
  testPid = pTestPropDef->OmPid ();
  return (_id == testPid) ? kAAFTrue : kAAFFalse;
}

aafBool ImplAAFClassDef::pvtPropertyIdentifierAUID::DoesMatch
  (const ImplAAFPropertyDef * pTestPropDef) const
{
  aafUID_t testUID;
  ASSERTU (pTestPropDef);
  ARESULT (AAFRESULT hr) pTestPropDef->GetAUID (&testUID);
  ASSERTU (AAFRESULT_SUCCEEDED (hr));
  return (EqualAUID (&_id, &testUID) ? kAAFTrue : kAAFFalse);
}


void ImplAAFClassDef::AssurePropertyTypesLoaded ()
{
  if (! _propTypesLoaded)
	{
	  _propTypesLoaded = true;

	  AAFRESULT hr;
	  ImplAAFClassDefSP spDef;
	  spDef	= this;
	  ASSERTU (spDef);
	  while (1)
		{
		  // Loop through this class and all its parents
		  ImplEnumAAFPropertyDefsSP pdEnumSP;
		  hr = spDef->GetPropertyDefs (&pdEnumSP);
		  ASSERTU (AAFRESULT_SUCCEEDED (hr));

		  ImplAAFPropertyDefSP propDefSP;

		  // Loop through all properties of this class
		  while (AAFRESULT_SUCCEEDED (pdEnumSP->NextOne (&propDefSP)))
			{
			  // return value unused; just force the lookup.
			  propDefSP->type ();
			}

		  // Look at the parent of this class
		  ImplAAFClassDefSP parentSP;
		  aafBool	isRoot;

		  hr = spDef->IsRoot(&isRoot);
		  ASSERTU (AAFRESULT_SUCCEEDED (hr));
		  if (isRoot)
			break;
		  hr = spDef->GetParent (&parentSP);
		  ASSERTU (AAFRESULT_SUCCEEDED (hr));
//!!!		  if (parentSP == NULL)
//		  {
//			 ImplAAFDictionary *pDict;
//			 ImplAAFClassDef	*curClass;
//			aafUID_t	uid;
//
//			hr = spDef->GetAUID (&uid);
//		    ASSERTU (AAFRESULT_SUCCEEDED (hr));
//			hr = spDef->GetDictionary(&pDict);
//		    ASSERTU (AAFRESULT_SUCCEEDED (hr));
//			hr = pDict->LookupClassDef (uid, &curClass);
//		    ASSERTU (AAFRESULT_SUCCEEDED (hr));
//			hr = curClass->GetParent(&parentSP);
//		    ASSERTU (AAFRESULT_SUCCEEDED (hr));
//		  }

		  ASSERTU (parentSP != NULL);
		  spDef = parentSP;
		}
	}
}






// override from OMStorable.
const OMClassId& ImplAAFClassDef::classId(void) const
{
  return (*reinterpret_cast<const OMClassId *>(&AUID_AAFClassDef));
}

// Override callbacks from OMStorable
void ImplAAFClassDef::onSave(void* clientContext) const
{
  ImplAAFMetaDefinition::onSave(clientContext);
}

void ImplAAFClassDef::onRestore(void* clientContext) const
{
  ImplAAFMetaDefinition::onRestore(clientContext);

  // Register PrimaryMob property definition on a restored instance
  // of Header class.
  //
  //
  // When opening a KLV-encoded file that doesn't have Header::PrimaryMob
  // defined a Header instance is created using the file's Header class
  // definition which is updated afterwards (see calls to
  // MergeBuiltinClassDefs() in ImplAAFFile::Open() and other file open
  // methods) to add missing properties present in the built-in Header
  // definition but missing from the file's Header definition. As a result
  // it ends up with a Header class instance that doesn't match its
  // definition, i.e. the instance doesn't have PrimaryMob property but
  // the class definition does.
  //
  // This breaks the mechanism that follows weak references to objects
  // contained in the Header, such as Data Definitions. See, for example,
  // ImplAAFComponent::GetDataDef().
  // 
  // This hack takes care of registering PrimaryMob property after the
  // Header class definition has been read from the file but before the
  // Header is instantiated.
  //
  // This hack relies on ImplAAFClassDef::onRestore() being called after
  // the metadata dictionary has been restored from the file and before
  // metadata instances are created. A corresponding change has been made
  // in OMKLVStoredObject::restore(OMFile&) to ensure this happens.
  //
  // TODO: This issue is likely to be not specific to PrimaryMob definition
  // but to affect any built-in optional property definition added to the
  // Header class. Furhtermore, it may not be specific to the Header class
  // but affect any class containing strong reference sets that can be
  // referenced by weak reference properties.
  //
  // The core issue is likely to be the KLV file restore mechanism that
  // creates class instances using definitions from the file before they
  // are updated with information from the built-in dictionary (see
  // OMKLVStoredObject::restore(OMFile&)).
  // 
  static const aafUID_t Class_Header =
  {0x0d010101, 0x0101, 0x2f00, {0x06, 0x0e, 0x2b, 0x34, 0x02, 0x06, 0x01, 0x01}};
  static const aafUID_t Property_Header_PrimaryMob =
  {0x06010104, 0x0108, 0x0000, {0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x04}};

  ImplAAFClassDef* This = const_cast<ImplAAFClassDef*>(this);

  aafUID_t classID = {0};
  GetAUID(&classID);
  if (classID == Class_Header &&
      !This->PvtIsPropertyDefRegistered(Property_Header_PrimaryMob))
  {
#if 1
	ImplAAFPropertyDefSP pPropDef;
	AAFRESULT hr = This->pvtRegisterPropertyDef(Property_Header_PrimaryMob,
												L"PrimaryMob",
												kAAFTypeID_MobWeakReference,
												kAAFTrue,
												kAAFFalse,
												&pPropDef);
	if (AAFRESULT_FAILED(hr))
		throw hr;
#else
	// Copy PrimaryMob property definition from the built-in dictionary into
    // Header class definition restored from file.
	ImplAAFDictionarySP pDict;
	AAFRESULT hr = GetDictionary (&pDict);
	if (AAFRESULT_SUCCEEDED(hr))
	{
	  ImplAAFClassDefSP pBuiltinClassDef;
	  hr = pDict->LookupClassDef(classID, &pBuiltinClassDef);
	  if (AAFRESULT_SUCCEEDED(hr))
	  {
		ImplAAFPropertyDefSP pBuiltinPropDef;
		hr = pBuiltinClassDef->LookupPropertyDef(Property_Header_PrimaryMob, &pBuiltinPropDef);
		if (AAFRESULT_SUCCEEDED(hr))
		{
		  ImplAAFPropertyDefSP pPropDef;
		  hr = This->pvtRegisterPropertyDef(Property_Header_PrimaryMob, pBuiltinPropDef->name(), kAAFTypeID_MobWeakReference, kAAFTrue, kAAFFalse, &pPropDef);
		}
	  }
	}
#endif
  }
}

const OMUniqueObjectIdentification& ImplAAFClassDef::identification(void) const
{
  // to prevent ambiguity
  return ImplAAFMetaDefinition::identification();
}

const wchar_t* ImplAAFClassDef::name(void) const
{
  // to prevent ambiguity
  return ImplAAFMetaDefinition::name();
}

bool ImplAAFClassDef::hasDescription(void) const
{
  // to prevent ambiguity
  return ImplAAFMetaDefinition::hasDescription();
}

const wchar_t* ImplAAFClassDef::description(void) const
{
  // to prevent ambiguity
  return ImplAAFMetaDefinition::description();
}

bool ImplAAFClassDef::isPredefined(void) const
{
  // to prevent ambiguity
  return ImplAAFMetaDefinition::isPredefined();
}

// Method is called after associated class has been added to MetaDictionary.
// If this method fails the class is removed from the MetaDictionary and the
// registration method will fail.
HRESULT ImplAAFClassDef::CompleteClassRegistration(void)
{
  AAFRESULT result = AAFRESULT_SUCCESS;

  // Make sure that all of the registration of properties can be
  // completed.
  ImplAAFPropertyDef *pProperty;
	OMStrongReferenceSetIterator<OMUniqueObjectIdentification, ImplAAFPropertyDef>propertyDefinitions(_Properties);
	while(++propertyDefinitions && AAFRESULT_SUCCEEDED(result))
	{
		pProperty = propertyDefinitions.value();
		ASSERTU(pProperty); // there should never be a NULL object in a set!
		if (pProperty)
		{
		  result = pProperty->CompleteClassRegistration();
		}
	}

  return result;
}

AAFRESULT ImplAAFClassDef::MergeTo(
    ImplAAFDictionary* pDestDictionary )
{
    ASSERTU( pDestDictionary );

    AAFRESULT hr = AAFRESULT_SUCCESS;


    aafUID_t parentClassID = {0};
    aafBoolean_t isRoot = kAAFFalse;
    IsRoot( &isRoot );
    if( isRoot == kAAFFalse )
    {
        ImplAAFClassDef* pSourceParentClassDef = NULL;
        GetParent( &pSourceParentClassDef );
        pSourceParentClassDef->GetAUID( &parentClassID );

        hr = pSourceParentClassDef->MergeTo( pDestDictionary );

        pSourceParentClassDef->ReleaseReference();
        pSourceParentClassDef = NULL;
    }


    aafUID_t classID;
    GetAUID( &classID );


    if( AAFRESULT_SUCCEEDED( hr ) )
    {
        ImplAAFClassDef* pDestClassDef = NULL;
        if( AAFRESULT_FAILED(
                pDestDictionary->LookupClassDef( classID, &pDestClassDef ) ) )
        {
            aafUInt32  nameBufLen = 0;
            GetNameBufLen( &nameBufLen );
            aafUInt8* pName = new aafUInt8[ nameBufLen ];
            GetName( (aafCharacter*)pName, nameBufLen );


            ImplAAFClassDef* pDestParentClassDef = NULL;
            if( isRoot == kAAFFalse )
            {
                pDestDictionary->LookupClassDef( parentClassID,
                                                 &pDestParentClassDef );
            }

            ImplAAFMetaDefinition* pMetaDefinition = NULL;
            hr = pDestDictionary->CreateMetaInstance( AUID_AAFClassDef,
                                                      &pMetaDefinition );
            if( AAFRESULT_SUCCEEDED( hr ) )
            {
                pDestClassDef = dynamic_cast<ImplAAFClassDef*>(pMetaDefinition);
                ASSERTU( pDestClassDef );
                pDestClassDef->AcquireReference();

                hr = pDestClassDef->Initialize( classID,
                                                pDestParentClassDef,
                                                (aafCharacter*)pName,
                                                _IsConcrete );

                if( AAFRESULT_SUCCEEDED( hr ) )
                {
                    hr = MergePropertyDefsTo( pDestClassDef );
                }

                if( AAFRESULT_SUCCEEDED( hr ) )
                {
                    hr = pDestDictionary->RegisterClassDef( pDestClassDef );
                }

                pMetaDefinition->ReleaseReference();
                pMetaDefinition = NULL;
            }


            delete[] pName;
            pName = NULL;

            if( pDestParentClassDef )
            {
                pDestParentClassDef->ReleaseReference();
                pDestParentClassDef = NULL;
            }
        }
        else
        {
            // The class definition exists in the destination
            // dictionary - merge missing property definitions.
            hr = MergePropertyDefsTo( pDestClassDef );
        }


        pDestClassDef->ReleaseReference();
        pDestClassDef = NULL;
    }


    return hr;
}

AAFRESULT ImplAAFClassDef::MergePropertyDefsTo(
    ImplAAFClassDef* pDestClassDef )
{
    ASSERTU( pDestClassDef );

    AAFRESULT hr = AAFRESULT_SUCCESS;
    ImplEnumAAFPropertyDefs* pEnumSourcePropertyDefs = NULL;
    GetPropertyDefs( &pEnumSourcePropertyDefs );
    ASSERTU( pEnumSourcePropertyDefs );

    ImplAAFPropertyDef* pSourcePropertyDef = NULL;
    while( AAFRESULT_SUCCEEDED(
               pEnumSourcePropertyDefs->NextOne(&pSourcePropertyDef)) )
    {
        aafUID_t propertyID;
        pSourcePropertyDef->GetAUID( &propertyID );

        ImplAAFPropertyDef* pDestPropertyDef = NULL;
        if( AAFRESULT_FAILED(
                pDestClassDef->LookupPropertyDef( propertyID,
                                                  &pDestPropertyDef ) ) )
        {
            hr = pSourcePropertyDef->MergeTo( pDestClassDef );
        }
        else
        {
            pDestPropertyDef->ReleaseReference();
            pDestPropertyDef = NULL;
        }

        pSourcePropertyDef->ReleaseReference();
        pSourcePropertyDef = NULL;

        if( AAFRESULT_FAILED(hr) )
            break;
    }

    pEnumSourcePropertyDefs->ReleaseReference();
    pEnumSourcePropertyDefs = NULL;


    return hr;
}

