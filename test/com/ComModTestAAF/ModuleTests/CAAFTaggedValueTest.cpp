// @doc INTERNAL
// @com This file implements the module test for CAAFTaggedValue
/******************************************\
*                                          *
* Advanced Authoring Format                *
*                                          *
* Copyright (c) 1998 Avid Technology, Inc. *
* Copyright (c) 1998 Microsoft Corporation *
*                                          *
\******************************************/


#include "AAF.h"

#include <iostream.h>
#include <stdio.h>


#include "AAFStoredObjectIDs.h"
#include "AAFResult.h"
#include "AAFDataDefs.h"

static aafWChar *slotNames[5] = { L"SLOT1", L"SLOT2", L"SLOT3", L"SLOT4", L"SLOT5" };
static const aafUID_t *	slotDDefs[5] = {&DDEF_Picture, &DDEF_Sound, &DDEF_Sound, &DDEF_Picture, &DDEF_Picture};
static aafLength_t	slotsLength[5] = { 297, 44100, 44100, 44100, 44100};

static aafInt32 fadeInLen  = 1000;
static aafInt32 fadeOutLen = 2000;
static aafFadeType_t fadeInType = kFadeLinearAmp;
static aafFadeType_t fadeOutType = kFadeLinearPower;
static aafSourceRef_t sourceRef; 
static aafWChar* TagNames =  L"TAG01";
static aafWChar* Comments =  L"Comment 1";	
static aafWChar* AltComment = L"Alternate Comment";
// Cross-platform utility to delete a file.
static void RemoveTestFile(const wchar_t* pFileName)
{
  const size_t kMaxFileName = 512;
  char cFileName[kMaxFileName];

  size_t status = wcstombs(cFileName, pFileName, kMaxFileName);
  if (status != (size_t)-1)
  { // delete the file.
    remove(cFileName);
  }
}

// convenient error handlers.
inline void checkResult(HRESULT r)
{
  if (FAILED(r))
    throw r;
}
inline void checkExpression(bool expression, HRESULT r)
{
  if (!expression)
    throw r;
}


static HRESULT CreateAAFFile(aafWChar * pFileName)
{
	IAAFFile *			pFile = NULL;
	bool				bFileOpen = false;
	IAAFHeader *        pHeader = NULL;
	IAAFDictionary*		pDictionary = NULL;
	IAAFMob				*pMob = NULL;
	IAAFMob*			pRMob = NULL;
	IAAFMasterMob*		pReferencedMob = NULL;
	IAAFCompositionMob* pCompMob = NULL;
	IAAFMobSlot			*newSlot = NULL;
	IAAFSegment			*seg = NULL;
	IAAFSourceClip		*sclp = NULL;
	aafProductIdentification_t	ProductInfo;
	aafUID_t			newUID, referencedMobID;
	HRESULT				hr = S_OK;

	ProductInfo.companyName = L"AAF Developers Desk";
	ProductInfo.productName = L"AAFTaggedValues Test";
	ProductInfo.productVersion.major = 1;
	ProductInfo.productVersion.minor = 0;
	ProductInfo.productVersion.tertiary = 0;
	ProductInfo.productVersion.patchLevel = 0;
	ProductInfo.productVersion.type = kVersionUnknown;
	ProductInfo.productVersionString = NULL;
	ProductInfo.productID = -1;
	ProductInfo.platform = NULL;


	try
	{
		// Remove the previous test file if any.
		RemoveTestFile(pFileName);

		// Create the file.
		checkResult(AAFFileOpenNewModify(pFileName, 0, &ProductInfo, &pFile));
		bFileOpen = true;
 
		// We can't really do anthing in AAF without the header.
		checkResult(pFile->GetHeader(&pHeader));

		// Get the AAF Dictionary so that we can create valid AAF objects.
		checkResult(pHeader->GetDictionary(&pDictionary));
 		
		//Make the first mob
		long	test;
		aafRational_t	audioRate = { 44100, 1 };

		// Create a  Composition Mob
		checkResult(pDictionary->CreateInstance(&AUID_AAFCompositionMob,
								  IID_IAAFCompositionMob, 
								  (IUnknown **)&pCompMob));

		checkResult(pCompMob->QueryInterface(IID_IAAFMob, (void **)&pMob));
		checkResult(CoCreateGuid((GUID *)&newUID));
		checkResult(pMob->SetMobID(&newUID));
		checkResult(pMob->SetName(L"AAFTaggedValuesTest"));

		// append a comment to this mob !!
		checkResult(pMob->AppendComment(TagNames, Comments));
	  
		checkResult(pMob->AppendComment(TagNames, AltComment));

		// Create a master mob to be referenced
		checkResult(pDictionary->CreateInstance(&AUID_AAFMasterMob,
								 IID_IAAFMasterMob, 
								 (IUnknown **)&pReferencedMob));

		checkResult(pReferencedMob->QueryInterface(IID_IAAFMob, (void **)&pRMob));
		checkResult(CoCreateGuid((GUID *)&referencedMobID));
		checkResult(pRMob->SetMobID(&referencedMobID));
		checkResult(pRMob->SetName(L"AAFTaggedValueTest::ReferencedMob"));

		// Add some slots
		for(test = 0; test < 5; test++)
		{
 			checkResult(pDictionary->CreateInstance(&AUID_AAFSourceClip,
								     IID_IAAFSourceClip, 
								     (IUnknown **)&sclp));		
			// Set the properties for the SourceClip
			sourceRef.sourceID = referencedMobID;
			sourceRef.sourceSlotID = 0;
			sourceRef.startTime = 0;
			checkResult(sclp->Initialize((aafUID_t *)slotDDefs[test], &slotsLength[test], sourceRef));
			checkResult(sclp->SetFade( fadeInLen, fadeInType, fadeOutLen, fadeOutType));

			checkResult(sclp->QueryInterface (IID_IAAFSegment, (void **)&seg));
			
			checkResult(pMob->AppendNewSlot (seg, test+1, slotNames[test], &newSlot));

			newSlot->Release();
			newSlot = NULL;

			seg->Release();
			seg = NULL;

			sclp->Release();
			sclp = NULL;
		}

		// Add the mob to the file.
		checkResult(pHeader->AppendMob(pMob));
		checkResult(pHeader->AppendMob(pRMob));
	}
	catch (HRESULT& rResult)
	{
		hr = rResult;
	}


  // Cleanup and return
  if (newSlot)
    newSlot->Release();

  if (seg)
    seg->Release();

  if (sclp)
    sclp->Release();

  if (pCompMob)
	  pCompMob->Release();

  if (pMob)
    pMob->Release();

  if (pReferencedMob)
	  pReferencedMob->Release();

  if (pRMob)
	  pRMob->Release();

  if (pDictionary)
    pDictionary->Release();

  if (pHeader)
    pHeader->Release();
      
  if (pFile)
  {  // Close file
    if (bFileOpen)
	  {
		  pFile->Save();
		  pFile->Close();
	  }
    pFile->Release();
  }

  return hr;
}

static HRESULT ReadAAFFile(aafWChar * pFileName)
{
	IAAFFile *					pFile = NULL;
	bool bFileOpen = false;
	IAAFHeader *				pHeader = NULL;
	IEnumAAFMobs *mobIter = NULL;
	IAAFMob			*aMob = NULL;
	IEnumAAFMobSlots	*slotIter = NULL;
	IEnumAAFTaggedValues* pCommentIterator = NULL;
	IAAFTaggedValue*		pComment = NULL;

	IAAFMobSlot		*slot = NULL;
	aafProductIdentification_t	ProductInfo;
	aafNumSlots_t	numMobs, n, s;
	aafUInt32		numComments, bytesRead;
	HRESULT						hr = S_OK;
	aafWChar		tag[64];
	aafWChar		Value[64];

	ProductInfo.companyName = L"AAF Developers Desk";
	ProductInfo.productName = L"AAFMob Test";
	ProductInfo.productVersion.major = 1;
	ProductInfo.productVersion.minor = 0;
	ProductInfo.productVersion.tertiary = 0;
	ProductInfo.productVersion.patchLevel = 0;
	ProductInfo.productVersion.type = kVersionUnknown;
	ProductInfo.productVersionString = NULL;
	ProductInfo.productID = -1;
	ProductInfo.platform = NULL;

	try
	{
		// Open the file
		checkResult(AAFFileOpenExistingRead(pFileName, 0, &pFile));
		bFileOpen = true;

		// We can't really do anthing in AAF without the header.
  		checkResult(pFile->GetHeader(&pHeader));


		checkResult(pHeader->GetNumMobs(kAllMob, &numMobs));
		checkExpression(1 == numMobs, AAFRESULT_TEST_FAILED);


		aafSearchCrit_t		criteria;
		criteria.searchTag = kNoSearch;
		checkResult(pHeader->EnumAAFAllMobs (&criteria, &mobIter));

		for(n = 0; n < numMobs; n++)
		{
			aafWChar		name[500], slotName[500];
			aafNumSlots_t	numSlots;
			aafUID_t		mobID;
			aafSlotID_t		trackID;

			checkResult(mobIter->NextOne (&aMob));
			checkResult(aMob->GetName (name, sizeof(name)));
			checkResult(aMob->GetMobID (&mobID));

			// Check for comments
			checkResult(aMob->GetNumComments(&numComments));
			checkExpression(1 == numComments, AAFRESULT_TEST_FAILED);
			checkResult(aMob->EnumAAFAllMobComments(&pCommentIterator));
			for(s = 0; s < numComments; s++)
			{
				checkResult(pCommentIterator->NextOne(&pComment));
				checkResult(pComment->GetName(tag, sizeof(tag)));
				checkResult(pComment->GetValue( sizeof(Value), (unsigned char *)Value, &bytesRead));
				checkExpression(wcscmp(tag, TagNames)== 0, AAFRESULT_TEST_FAILED);
				checkExpression(wcscmp(Value, AltComment)== 0, AAFRESULT_TEST_FAILED);
				pComment->Release();
			}
			pCommentIterator->Release();
			
			checkResult(aMob->GetNumSlots (&numSlots));
			checkExpression(5 == numSlots, AAFRESULT_TEST_FAILED);

			checkResult(aMob->EnumAAFAllMobSlots(&slotIter));

			for(s = 0; s < numSlots; s++)
			{
				checkResult(slotIter->NextOne (&slot));
				checkResult(slot->GetName (slotName, sizeof(slotName)));
				checkResult(slot->GetSlotID(&trackID));
				checkExpression (wcscmp(slotName, slotNames[s]) == 0, AAFRESULT_TEST_FAILED);

				slot->Release();
				slot = NULL;
			}

			aMob->Release();
			aMob = NULL;
		}
	}
	catch (HRESULT& rResult)
	{
    hr = rResult;
	}

	// Cleanup object references
  if (slot)
    slot->Release();

  if (slotIter)
    slotIter->Release();

  if (aMob)
    aMob->Release();

  if (mobIter)
    mobIter->Release();

  if (pHeader)
    pHeader->Release();
      
  if (pFile)
  {  // Close file
    if (bFileOpen)
      pFile->Close();
     pFile->Release();
  }

  return hr;
}
 

extern "C" HRESULT CAAFTaggedValue_test()
{
	HRESULT hr = AAFRESULT_NOT_IMPLEMENTED;
 	aafWChar * pFileName = L"TaggedValuesTest.aaf";

	try
	{
		hr = CreateAAFFile(	pFileName );
		if(hr == AAFRESULT_SUCCESS)
			hr = ReadAAFFile( pFileName );
	}
	catch (...)
	{
	  cerr << "CTaggedValues_test...Caught general C++"
		" exception!" << endl; 
	  hr = AAFRESULT_TEST_FAILED;
	}

  return hr;
}










  // 
  // Succeeds if all of the following are true:
  // - the ppTypeDef pointer is valid.
  // 
  // If this method fails nothing will be written to *ppTypeDef.
  // 
  // This method will return the following codes.  If more than one of
  // the listed errors is in effect\, it will return the first one
  // encountered in the order given below:
  //
  // AAFRESULT_SUCCESS
  //   - succeeded.  \(This is the only code indicating success.\)
  //
  // AAFRESULT_NOT_INITIALIZED
  //   - This object has not yet had Initialize\(\) called on it.
  //
  // AAFRESULT_PROP_NOT_PRESENT
  //   - This property does not exist in the file.
  //
  // AAFRESULT_NULL_PARAM
  //   - ppTypeDef arg is NULL.)






/****/




