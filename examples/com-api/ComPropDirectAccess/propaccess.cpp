/***********************************************************************
 *
 *              Copyright (c) 1998-1999 Avid Technology, Inc.
 *
 * Permission to use, copy and modify this software and accompanying 
 * documentation, and to distribute and sublicense application software
 * incorporating this software for any purpose is hereby granted, 
 * provided that (i) the above copyright notice and this permission
 * notice appear in all copies of the software and related documentation,
 * and (ii) the name Avid Technology, Inc. may not be used in any
 * advertising or publicity relating to the software without the specific,
 * prior written permission of Avid Technology, Inc.
 *
 * THE SOFTWARE IS PROVIDED AS-IS AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL AVID TECHNOLOGY, INC. BE LIABLE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, PUNITIVE, INDIRECT, ECONOMIC, CONSEQUENTIAL OR
 * OTHER DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE AND
 * ACCOMPANYING DOCUMENTATION, INCLUDING, WITHOUT LIMITATION, DAMAGES
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, AND WHETHER OR NOT
 * ADVISED OF THE POSSIBILITY OF DAMAGE, REGARDLESS OF THE THEORY OF
 * LIABILITY.
 *
 ************************************************************************/

#include "AAF.h"

#include "CAAFBuiltinDefs.h"

//
// This example code is intended to show the following:
//
// - Create a new type.  We'll create a record (struct) type called
//   Rational16, which is a rational number composed of an aafInt16
//   numerator and an aafInt16 denominator.
//
// - Define a compile-type C struct (called rational16_t) which can be
//   used by the client to pass 16-bit-rational values through the
//   API.  Register it with the ref impl so the ref impl knows what
//   the struct field offsets are when reading and writing
//   rational16_t values.
//
// - Add a new optional property to an existing class (Component).
//   The optional property will be called "Odor", and its type will be
//   our new Rational16.
//
// - Create an object of a class derived from Component (which will
//   then be able to contain the new optional property).
//
// - Set and get values for the new optional property.
//


//
// This code Contains the following functions:
//

//
// Creates a Rational 16 type (a rational made up of int16 numerator
// and denominator) and inserts it in the dictionary.  Once it is put
// into the dictionary it is not necessary to do so again for this AAF
// file.
//
HRESULT createRational16Type (IAAFDictionary * pDict);

//
// Code to register the local compile-time C struct which can
// represent a Rational16 property.  This registration will allow the
// client of the ref impl to to use the rational16_t C struct to pass
// Rat16 values in and out of the property direct interfaces.
//
// This code must be called at runtime by every client who wishes to
// use C structs to represent client-defined record property types.
// It has to be called once per dictionary instantiation lifetime.
// Iin other words, if it is called on a dictionary and then saved to
// a file, an application which reads the file will need to
// re-register since the in-memory copy of the dictionary was
// re-constituted since the offsets were registered.
//
HRESULT registerRational16StructOffsets (IAAFDictionary * pDict);

//
// Adds an optional "Odor" property to AAFComponent, whose type is
// Rational16.  Once this code is called, the version of AAFComponent
// in the given dictionary will be so augmented.
//
HRESULT addRational16ToComponent (IAAFDictionary * pDict);

//
// Code to create an augmented AAFComponent, and set the "Odor"
// property therein.
//
HRESULT createStinkyFiller (IAAFDictionary * pDict,
							IAAFFiller ** ppCreatedFiller);

//
// Code to check the presence of "Odor" property in an augmented
// AAFComponent, and make sure its value is as set.
//
HRESULT checkStinkyFiller (IAAFDictionary * pDict,
						   IAAFFiller * pFiller);


//**************************************
//
// Implementations of the test functions
//
//**************************************

#include <stddef.h>
#include <assert.h>

//
// Create handy typedefs for some smart pointers
//
#include "AAFSmartPointer.h"
typedef IAAFSmartPointer<IAAFClassDef>          IAAFClassDefSP;
typedef IAAFSmartPointer<IAAFDictionary>        IAAFDictionarySP;
typedef IAAFSmartPointer<IAAFEssenceDescriptor> IAAFEssenceDescriptorSP;
typedef IAAFSmartPointer<IAAFFile>              IAAFFileSP;
typedef IAAFSmartPointer<IAAFHTMLDescriptor>    IAAFHTMLDescriptorSP;
typedef IAAFSmartPointer<IAAFFiller>            IAAFFillerSP;
typedef IAAFSmartPointer<IAAFHeader>            IAAFHeaderSP;
typedef IAAFSmartPointer<IAAFMob>               IAAFMobSP;
typedef IAAFSmartPointer<IAAFMobSlot>           IAAFMobSlotSP;
typedef IAAFSmartPointer<IAAFObject>            IAAFObjectSP;
typedef IAAFSmartPointer<IAAFPropertyDef>       IAAFPropertyDefSP;
typedef IAAFSmartPointer<IAAFPropertyValue>     IAAFPropertyValueSP;
typedef IAAFSmartPointer<IAAFSegment>           IAAFSegmentSP;
typedef IAAFSmartPointer<IAAFSourceMob>         IAAFSourceMobSP;
typedef IAAFSmartPointer<IAAFTimelineMobSlot>   IAAFTimelineMobSlotSP;
typedef IAAFSmartPointer<IAAFTypeDef>           IAAFTypeDefSP;
typedef IAAFSmartPointer<IAAFTypeDefRecord>     IAAFTypeDefRecordSP;
typedef IAAFSmartPointer<IAAFTypeDefRename>     IAAFTypeDefRenameSP;
typedef IAAFSmartPointer<IEnumAAFMobSlots>      IEnumAAFMobSlotsSP;

// This is the compile-time C struct which can be used to represent
// the Rational16 type.
struct rational16_t
{
  aafInt16 numerator;
  aafInt16 denominator;
};



//
// AUIDs used by this module
//

//
// AAF-supplied and -published AUIDs
//
#include "AAFStoredObjectIDs.h"
#include "AAFTypeDefUIDs.h"
#include "AAFDataDefs.h"


//
// client-supplied and -published AUIDs
//
// type def for Rational16
// {36847471-D263-11d2-8429-00600832ACB8}
const aafUID_t AUID_TypeRational16 = 
{ 0x36847471, 0xd263, 0x11d2, { 0x84, 0x29, 0x0, 0x60, 0x8, 0x32, 0xac, 0xb8 } };
//
// type def for "RenamedRational16" (renamed Rational16)
// {7F116246-4B60-11d3-8443-00600832ACB8}
const aafUID_t AUID_TypeRenamedRational16 = 
{ 0x7f116246, 0x4b60, 0x11d3, { 0x84, 0x43, 0x0, 0x60, 0x8, 0x32, 0xac, 0xb8 } };
//
// property def for "Odor" property in Component
// {36847472-D263-11d2-8429-00600832ACB8}
const aafUID_t AUID_PropertyComponentOdor = 
{ 0x36847472, 0xd263, 0x11d2, { 0x84, 0x29, 0x0, 0x60, 0x8, 0x32, 0xac, 0xb8 } };



#define PROPAGATE_RESULT(condition) \
  if (! SUCCEEDED(condition)) return (condition);


//
// Creates a Rational 16 type (a rational made up of int16 numerator
// and denominator) and inserts it in the dictionary.  Once it is put
// into the dictionary it is not necessary to do so again for this AAF
// file.
//
HRESULT createRational16Type (IAAFDictionary * pDict)
{
  IAAFTypeDefSP pTDInt16;
  const aafUInt32 kNumMembers = 2;

  CAAFBuiltinDefs defs (pDict);

  // Get the pre-defined type for int16
  PROPAGATE_RESULT(pDict->LookupTypeDef(kAAFTypeID_Int16,
										&pTDInt16));

  // load up arrays to initialize our new typedef
  IAAFTypeDef * memberTypes[kNumMembers] =
  {
	pTDInt16,
	pTDInt16
  };
  aafString_t memberNames[kNumMembers] =
  {
	L"numerator",
	L"denominator"
  };

  // Allocate a new typedef which will represent a 16-bit rational
  IAAFTypeDefRecordSP pTDRational16;
  PROPAGATE_RESULT(pDict->CreateMetaInstance(AUID_AAFTypeDefRecord,
                                             IID_IAAFTypeDefRecord,
                                             (IUnknown **) &pTDRational16));
  PROPAGATE_RESULT(pTDRational16->Initialize(AUID_TypeRational16,
											 memberTypes,
											 memberNames,
											 kNumMembers,
											 L"Rational16"));
  
  // Add it to the dictionary
  IAAFTypeDefSP spTypeDef;
  PROPAGATE_RESULT(pTDRational16->QueryInterface(IID_IAAFTypeDef,
												 (void**)&spTypeDef));
  PROPAGATE_RESULT(pDict->RegisterTypeDef(spTypeDef));

  return S_OK;
}


HRESULT createRenamedRational16 (IAAFDictionary * pDict)
{
  IAAFTypeDefRenameSP pRenamedRational16;
  CAAFBuiltinDefs defs (pDict);

  // look up existing type
  IAAFTypeDefSP pTDRational16;
  PROPAGATE_RESULT(pDict->LookupTypeDef(AUID_TypeRational16,
										&pTDRational16));

  // create new (rename) type
  PROPAGATE_RESULT(pDict->CreateMetaInstance(AUID_AAFTypeDefRename,
                                             IID_IAAFTypeDefRename,
                                             (IUnknown **) &pRenamedRational16));

  // connect 'em up
  PROPAGATE_RESULT(pRenamedRational16->Initialize(AUID_TypeRenamedRational16,
												  pTDRational16,
												  L"RenamedRational16"));

  return S_OK;
}



//
// Code to register the local compile-time C struct which can
// represent a Rational16 property.  This registration will allow the
// client of the ref impl to to use the rational16_t C struct to pass
// Rat16 values in and out of the property direct interfaces.
//
// This code must be called at runtime by every client who wishes to
// use C structs to represent client-defined Record property types.
//
HRESULT registerRational16StructOffsets (IAAFDictionary * pDict)
{
  // Get the Rational16 type out of the dictionary
  IAAFTypeDefSP spTypeDef;
  PROPAGATE_RESULT(pDict->LookupTypeDef(AUID_TypeRational16,
										&spTypeDef));

  IAAFTypeDefRecordSP spTypeDefRecord;
  PROPAGATE_RESULT(spTypeDef->QueryInterface(IID_IAAFTypeDefRecord,
											 (void**)&spTypeDefRecord));

  aafUInt32 offsets[2] =
	{
	  offsetof (rational16_t, numerator),
	  offsetof (rational16_t, denominator)
	};

  PROPAGATE_RESULT(spTypeDefRecord->
				   RegisterMembers (offsets,
									sizeof (offsets) / sizeof (offsets[0]),
									sizeof (rational16_t)));

  return S_OK;
}



//
// Adds an optional "Odor" property to AAFComponent, whose type is
// Rational16.  Once this code is called, the version of AAFComponent
// in the given dictionary will be so augmented.
//
HRESULT addRational16ToComponent (IAAFDictionary * pDict)
{
  IAAFClassDefSP pCDComponent;
  CAAFBuiltinDefs defs (pDict);

  // Get the class def for AAFComponent
  PROPAGATE_RESULT(pDict->LookupClassDef(AUID_AAFComponent,
										 &pCDComponent));

  // Get the Rational16 type out of the dictionary
  IAAFTypeDefSP pTDRational16;
  PROPAGATE_RESULT(pDict->LookupTypeDef(AUID_TypeRational16,
										&pTDRational16));

  IAAFPropertyDefSP pJunk;
  PROPAGATE_RESULT(pCDComponent->
				   RegisterOptionalPropertyDef(AUID_PropertyComponentOdor,
											   L"Odor",       // prop name
											   pTDRational16, // prop ID
											   &pJunk));

  return S_OK;
}


//
// Code to create an augmented AAFComponent, and set the "Odor"
// property therein.
//
HRESULT createStinkyFiller (IAAFDictionary * pDict,
							IAAFFiller ** ppCreatedFiller)
{
  assert (pDict);
  assert (ppCreatedFiller);

  CAAFBuiltinDefs defs (pDict);

  //
  // Make a filler object.  Initialize it, then get the generic
  // IAAFObject interface.
  //
  IAAFFiller * pFill = 0;
  PROPAGATE_RESULT(defs.cdFiller()->
				   CreateInstance(IID_IAAFFiller,
								  (IUnknown **) &pFill));
  assert (pFill);
  PROPAGATE_RESULT (pFill->Initialize(defs.ddSound(), 10));

  IAAFObjectSP pObj;
  PROPAGATE_RESULT (pFill->QueryInterface(IID_IAAFObject,
										  (void**)&pObj));


  //
  // Get the property definition for Component::Odor
  //
  IAAFClassDefSP pCDComponent;
  PROPAGATE_RESULT(pDict->LookupClassDef(AUID_AAFComponent,
										 &pCDComponent));
  IAAFPropertyDefSP pPDComponentOdor;
  PROPAGATE_RESULT(pCDComponent->
				   LookupPropertyDef(AUID_PropertyComponentOdor,
									 &pPDComponentOdor));


  //
  // Verify that the optional property "Odor" is not *yet* present in
  // the object.
  //
  aafBool present;
  PROPAGATE_RESULT(pObj->IsPropertyPresent (pPDComponentOdor,
											&present));
  // make sure it's not there
  if (present)
	return E_FAIL;


  //
  // Set the odor value to this filler to "1/2".
  //
  rational16_t odor;
  odor.numerator = 1;
  odor.denominator = 2;

  //
  // Create a property value, through the type def, which contains our
  // odor value.  Note that the function
  // registerRational16StructOffsets() must have already been run so
  // the ref impl knows what the struct offsets are for rational16_t.
  //
  // First get the type def.
  //
  IAAFTypeDefSP spTypeDef;
  PROPAGATE_RESULT (pDict->
					LookupTypeDef (AUID_TypeRational16,
								   &spTypeDef));
  IAAFTypeDefRecordSP spTypeDefRat16;
  PROPAGATE_RESULT(spTypeDef->
				   QueryInterface(IID_IAAFTypeDefRecord,
								  (void**)&spTypeDefRat16));
  // Now create a property value object with that value.
  IAAFPropertyValueSP spOdorPropVal;
  PROPAGATE_RESULT (spTypeDefRat16->
					CreateValueFromStruct ((aafMemPtr_t) &odor,
										   sizeof (odor),
										   &spOdorPropVal));

  //
  // Set the value of the odor property to that property value.
  //
  PROPAGATE_RESULT(pObj->SetPropertyValue(pPDComponentOdor,
										  spOdorPropVal));


  //
  // The 'set' above should have added the optional property to the
  // object, and set its value.  Verify that the optional property is
  // now present in the object.
  //
  PROPAGATE_RESULT(pObj->IsPropertyPresent (pPDComponentOdor,
											&present));
  // make sure it's there
  assert (present);

  assert (ppCreatedFiller);
  assert (pFill);
  // pFill->Release() balances out (*ppCreatedFiller)->Acquire().
  *ppCreatedFiller = pFill;

  //
  // check it for good measure.
  //
  PROPAGATE_RESULT (checkStinkyFiller (pDict, pFill));

  return S_OK;
}


//
// Code to access "Odor" component in an augmented AAFComponent.
//
HRESULT checkStinkyFiller (IAAFDictionary * pDict,
						   IAAFFiller * pFiller)
{
  assert (pFiller);

  CAAFBuiltinDefs defs (pDict);
  IAAFObjectSP pObj;
  PROPAGATE_RESULT(pFiller->QueryInterface(IID_IAAFObject,
										   (void**)&pObj));

  //
  // Get the property def for Component::Odor
  //
  IAAFClassDefSP pCDComponent;
  PROPAGATE_RESULT(pDict->LookupClassDef(AUID_AAFComponent,
										 &pCDComponent));
  IAAFPropertyDefSP pPDComponentOdor;
  PROPAGATE_RESULT(pCDComponent->
				   LookupPropertyDef(AUID_PropertyComponentOdor,
									 &pPDComponentOdor));

  //
  // Make sure the Odor property is there.
  //
  aafBool present;
  PROPAGATE_RESULT(pObj->IsPropertyPresent (pPDComponentOdor,
											&present));
  if (!present)
	return E_FAIL;

  //
  // Read the odor.
  //
  IAAFPropertyValueSP spPropVal;
  PROPAGATE_RESULT(pObj->GetPropertyValue(pPDComponentOdor,
										  &spPropVal));

  //
  // Get the type def from the dict with which to interpret this
  // property value.
  //
  IAAFTypeDefSP spTypeDef;
  PROPAGATE_RESULT (pDict->
					LookupTypeDef (AUID_TypeRational16,
								   &spTypeDef));
  IAAFTypeDefRecordSP spTypeDefRat16;
  PROPAGATE_RESULT(spTypeDef->
				   QueryInterface(IID_IAAFTypeDefRecord,
								  (void**)&spTypeDefRat16));
  // Now interpret the property value as a rational16_t.
  rational16_t aroma = { 0 };
  PROPAGATE_RESULT (spTypeDefRat16->
					GetStruct (spPropVal,
							   (aafMemPtr_t) &aroma,
							   sizeof (aroma)));

  if (1 != aroma.numerator)
	return E_FAIL;
  if (2 != aroma.denominator)
	return E_FAIL;

  return S_OK;
}



//*********************************************
//
// Infrastructure to make use of the test funcs
//
//*********************************************

#include <iostream.h>
#include <stdio.h>

static void     FatalErrorCode(HRESULT errcode, int line, char *file)
{
  cerr << "\nError \'" << errcode
	   << "\' returned at line " << line
	   << " in " << file << endl;
  exit(1);
}


static HRESULT moduleErrorTmp = S_OK; /* note usage in macro */
#define check(a)  \
{ moduleErrorTmp = a; \
  if (!SUCCEEDED(moduleErrorTmp)) \
     FatalErrorCode(moduleErrorTmp, __LINE__, __FILE__);\
}


static void convert(wchar_t* wcName, size_t length, const char* name)
{
  assert (name);
  assert (*name);
  assert (wcName);
  assert (length > 0);
  
  size_t status = mbstowcs(wcName, name, length);
  if (status == (size_t)-1) {
    cerr << "\nError : Failed to convert \'" << name
		 << "\' to a wide character string.\n" << endl;
    exit(1);  
  }
}

static void convert(char* cName, size_t length, const wchar_t* name)
{
  assert (name);
  assert (*name);
  assert (cName);
  assert (length > 0);

  size_t status = wcstombs(cName, name, length);
  if (status == (size_t)-1) {
    cerr << "\nError : Conversion failed.\n" << endl;
    exit(1);  
  }
}

static void convert(char* cName, size_t length, const char* name)
{
  assert (name);
  assert (*name);
  assert (cName);
  assert (length > 0);

  size_t sourceLength = strlen(name);
  if (sourceLength < length - 1) {
    strncpy(cName, name, length);
  } else {
	cerr << "\nError : Failed to copy \'" << name
		 << "\'.\n" << endl;
    exit(1);  
  }
}

static void convert(wchar_t* wName, size_t length, const wchar_t* name)
{
  assert (name);
  assert (*name);
  assert (wName);
  assert (length > 0);

  size_t sourceLength = 0;
  while (*name)
    ++sourceLength;
  if (sourceLength < length - 1) {
    // Copy the string if there is enough room in the destinition buffer.
    while (*wName++ = *name++)
      ;
  } else {
	cerr << "\nError : Failed to copy \'" << name
		 << "\'.\n" << endl;
    exit(1);  
  }
}


#if defined(_MAC) || defined(macintosh)
// For some reason the CoCreateGuid() function is not implemented in the 
// Microsoft Component Library...so we define something that should be
// fairly unique on the mac.

#include <Events.h>
#include <time.h>

STDAPI CoCreateGuid(GUID  *pguid)
{
  // {1994bd00-69de-11d2-b6bc-fcab70ff7331}
  static GUID sTemplate = 
    { 0x1994bd00, 0x69de, 0x11d2, { 0xb6, 0xbc, 0xfc, 0xab, 0x70, 0xff, 0x73, 0x31 } };

  static bool sInitializedTemplate = false;
  
  if (NULL == pguid)
    return E_INVALIDARG;
    
  if (!sInitializedTemplate)
  {
    time_t timer = time(NULL);
    UInt32 ticks = TickCount();
   
    sTemplate.Data1 += timer + ticks;
    
    sInitializedTemplate = true;
  }
  
  // Just bump the first member of the guid to emulate GUIDGEN behavior.
  ++sTemplate.Data1;
  
  *pguid = sTemplate;
  
  return S_OK;
}

#endif


static void ReadAAFFile(aafWChar * pFileName,
						/*[in]*/ aafMobID_constref createdMobID)
{
  IAAFFileSP spFile;
  check (AAFFileOpenExistingRead(pFileName, 0, &spFile));

  IAAFHeaderSP spHeader;
  check (spFile->GetHeader(&spHeader));

  IAAFDictionarySP spDictionary;
  check (spHeader->GetDictionary(&spDictionary));
   
  // This registration needs to be done, even though offsets were
  // registered when file was written.  Note that if it is to be done
  // at all, it has to be done before any attempt to read any object
  // containing a property of this type is done.
  check (registerRational16StructOffsets (spDictionary));

  IAAFMobSP spMob;
  check (spHeader->LookupMob (createdMobID, &spMob));

  aafNumSlots_t numSlots = 0;
  check (spMob->CountSlots (&numSlots));
  // we only put one in
  assert (1 == numSlots);

  IEnumAAFMobSlotsSP spSlotEnum;
  check (spMob->GetSlots (&spSlotEnum));

  IAAFMobSlotSP spMobSlot;
  // Since we only put one in, just bother with the first one.
  check (spSlotEnum->NextOne (&spMobSlot));

  // Get the segment; it's got to be our filler.
  IAAFSegmentSP spSegment;
  check (spMobSlot->GetSegment (&spSegment));

  // Get filler interface
  IAAFFillerSP spFiller;
  check (spSegment->QueryInterface(IID_IAAFFiller,
								   (void**)&spFiller));

  // We do the checking in this function.
  check (checkStinkyFiller (spDictionary, spFiller));

  // All done...
  check (spFile->Close());
}

const aafUID_t NIL_UID = { 0 };

// Creates a file and adds a mob, containing a slot, containing a Fill
// clip augmented with our optional property.  The resulting generated
// mob ID is returned via the createdMobID argument.
static void CreateAAFFile(aafWChar * pFileName,
						  /*[out]*/ aafMobID_t & createdMobID)
{
  aafProductIdentification_t  ProductInfo;
  
  // delete any previous test file before continuing...
  char chFileName[1000];
  convert(chFileName, sizeof(chFileName), pFileName);
  remove(chFileName);

  // Create a new file...
  aafProductVersion_t v;
  v.major = 1;
  v.minor = 0;
  v.tertiary = 0;
  v.patchLevel = 0;
  v.type = kAAFVersionUnknown;
  ProductInfo.companyName = L"AAF Developers Desk";
  ProductInfo.productName = L"Property Access Example";
  ProductInfo.productVersion = &v;
  ProductInfo.productVersionString = NULL;
  ProductInfo.productID = NIL_UID;
  ProductInfo.platform = NULL;
  
  IAAFFileSP spFile;
  check (AAFFileOpenNewModify(pFileName, 0, &ProductInfo, &spFile));
  
  IAAFHeaderSP spHeader;
  check (spFile->GetHeader(&spHeader));

  // Get the AAF Dictionary so that we can create valid AAF objects.
  IAAFDictionarySP spDictionary;
  check (spHeader->GetDictionary(&spDictionary));
  CAAFBuiltinDefs defs (spDictionary);
   
  // Create and register all new things that have to go into the
  // dictionary
  check (createRational16Type (spDictionary));
  check (registerRational16StructOffsets (spDictionary));
  check (addRational16ToComponent (spDictionary));

  // Create a source Mob
  IAAFSourceMobSP  smob;
  check (defs.cdSourceMob()->
		 CreateInstance(IID_IAAFSourceMob, 
						(IUnknown **)&smob));

  IAAFMobSP spMob;
  check (smob->QueryInterface (IID_IAAFMob, (void **)&spMob));

  aafMobID_t newMobID;
  check (CoCreateGuid((GUID *)&newMobID)); // hack: we need a utility function.
  check (spMob->SetMobID(newMobID));
  check (spMob->SetName(L"a Source Mob"));

  IAAFHTMLDescriptorSP  spHTMLDesc;
  check (defs.cdHTMLDescriptor()->
		 CreateInstance(IID_IAAFHTMLDescriptor, 
						(IUnknown **) &spHTMLDesc));
  aafRational_t  audioRate = { 44100, 1 };

  IAAFEssenceDescriptorSP spEssenceDesc;
  check (spHTMLDesc->QueryInterface (IID_IAAFEssenceDescriptor,
								   (void **)&spEssenceDesc));
  check (smob->SetEssenceDescriptor (spEssenceDesc));

  // Add a slot; make it our "stinky filler".
  IAAFFillerSP spFill;
  check (createStinkyFiller (spDictionary, &spFill));
  IAAFSegmentSP seg;
  check (spFill->QueryInterface (IID_IAAFSegment, (void **)&seg));
  IAAFTimelineMobSlotSP newSlot;
  check (spMob->AppendNewTimelineSlot (audioRate,
									   seg, // segment to put into new slot
									   1,   // slot number
									   L"Slot 1",
									   0,
									   &newSlot));

  // Add the newly created and initialized Mob to the end of the mob index.
  check (spHeader->AddMob(spMob));
  
  // save and exit.
  check (spFile->Save());
  check (spFile->Close());

  // Return the created mob ID to the user.
  createdMobID = newMobID;
}


//
// simple helper class to initialize and cleanup COM library.
//
struct CComInitialize
{
  CComInitialize()
  {
    CoInitialize(NULL);
  }

  ~CComInitialize()
  {
    CoUninitialize();
  }
};

//
// simple helper class to initialize and cleanup AAF library.
//
struct CAAFInitialize
{
  CAAFInitialize(const char *dllname = NULL)
  {
  	cout << "Attempting to load the AAF dll..." << endl;
    check (AAFLoad(dllname));
    cout << "Loaded." << endl;
  }

  ~CAAFInitialize()
  {
    AAFUnload();
  }
};


main()
{
  CComInitialize comInit;
  CAAFInitialize aafInit;

  aafWChar * pwFileName = L"PropAccess.aaf";
  const char * pFileName = "PropAccess.aaf";

  aafMobID_t createdMobID;
  cout << "***Creating file " << pFileName << endl;
  CreateAAFFile(pwFileName, createdMobID);
  cout << "***Re-opening file " << pFileName << endl;
  ReadAAFFile(pwFileName, createdMobID);
  
  cout << "Done" << endl;;
  return(0);
}
