// @doc INTERNAL
// @com This file implements the module test for CAAFDefinitionObject
/******************************************\
*                                          *
* Advanced Authoring Format                *
*                                          *
* Copyright (c) 1998 Avid Technology, Inc. *
* Copyright (c) 1998 Microsoft Corporation *
*                                          *
\******************************************/

#ifndef __CEnumAAFSegments_h__
#include "CEnumAAFSegments.h"
#endif

#include "AAFResult.h"
#include "AAFDefUIDs.h"
#include <iostream.h>
#include <stdio.h>

#include "AAFStoredObjectIDs.h"

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


static HRESULT OpenAAFFile(aafWChar*			pFileName,
						   aafMediaOpenMode_t	mode,
						   // IAAFSession**		ppSession,
						   IAAFFile**			ppFile,
						   IAAFHeader**			ppHeader)
{
	aafProductIdentification_t	ProductInfo;
	HRESULT						hr = AAFRESULT_SUCCESS;

	ProductInfo.companyName = L"AAF Developers Desk";
	ProductInfo.productName = L"AAFSelector Test";
	ProductInfo.productVersion.major = 1;
	ProductInfo.productVersion.minor = 0;
	ProductInfo.productVersion.tertiary = 0;
	ProductInfo.productVersion.patchLevel = 0;
	ProductInfo.productVersion.type = kVersionUnknown;
	ProductInfo.productVersionString = NULL;
	ProductInfo.productID = -1;
	ProductInfo.platform = NULL;

	/*
	hr = CoCreateInstance(CLSID_AAFSession,
						   NULL, 
						   CLSCTX_INPROC_SERVER, 
						   IID_IAAFSession, 
						   (void **)ppSession);
	*/
	hr = CoCreateInstance(CLSID_AAFFile,
						   NULL, 
						   CLSCTX_INPROC_SERVER, 
						   IID_IAAFFile, 
						   (void **)ppFile);
	if (AAFRESULT_SUCCESS != hr)
		return hr;
    hr = (*ppFile)->Initialize();
	if (AAFRESULT_SUCCESS != hr)
		return hr;

	// hr = (*ppSession)->SetDefaultIdentification(&ProductInfo);
	// if (AAFRESULT_SUCCESS != hr)
	// 	return hr;

	switch (mode)
	{
	case kMediaOpenReadOnly:
		// hr = (*ppSession)->OpenReadFile(pFileName, ppFile);
		hr = (*ppFile)->OpenExistingRead(pFileName, 0);
		break;

	case kMediaOpenAppend:
		// hr = (*ppSession)->CreateFile(pFileName, kAAFRev1, ppFile);
		hr = (*ppFile)->OpenNewModify(pFileName, 0, &ProductInfo);
		break;

	default:
		hr = AAFRESULT_TEST_FAILED;
		break;
	}

	if (FAILED(hr))
	{
		// (*ppSession)->Release();
		// *ppSession = NULL;
		(*ppFile)->Release();
		*ppFile = NULL;
		return hr;
	}
  
  	hr = (*ppFile)->GetHeader(ppHeader);
	if (FAILED(hr))
	{
		// (*ppSession)->Release();
		// *ppSession = NULL;
		(*ppFile)->Release();
		*ppFile = NULL;
		return hr;
	}
 	
	return hr;
}

static HRESULT CreateAAFFile(aafWChar * pFileName)
{
	IAAFFile*			pFile = NULL;
	IAAFHeader*			pHeader = NULL;
	IAAFDictionary*		pDictionary = NULL;
	IAAFMob*			pMob = NULL;
	IAAFMobSlot*		pMobSlot = NULL;
	IAAFMob*			pReferencedMob = NULL;
	IAAFSourceClip*		pSourceClip = NULL;
	IAAFFiller*			pFiller = NULL;
	IAAFSegment*		pSegment = NULL;
	IAAFSelector*		pSelector = NULL;
	IAAFCompositionMob*	pCompMob = NULL;
	aafUID_t			NewMobID, referencedMobID;
	aafInt32			fadeInLen  = 1000;
	aafInt32			fadeOutLen = 2000;
	aafFadeType_t		fadeInType = kFadeLinearAmp;
	aafFadeType_t		fadeOutType = kFadeLinearPower;
	aafSourceRef_t		sourceRef; 
	aafUID_t			fillerUID = DDEF_Video;
	aafLength_t			fillerLength = 3200;

	HRESULT				hr = AAFRESULT_SUCCESS;

	try
	{  
		// Remove the previous test file if any.
		RemoveTestFile(pFileName);


		// Create the AAF file
		checkResult(OpenAAFFile(pFileName, kMediaOpenAppend, &pFile, &pHeader));

		// Get the AAF Dictionary so that we can create valid AAF objects.
		checkResult(pHeader->GetDictionary(&pDictionary));
 		
		// Create a mob to be referenced by the source clip
		checkResult(pDictionary->CreateInstance(&AUID_AAFMasterMob,
								 IID_IAAFMob, 
								 (IUnknown **)&pReferencedMob));
		checkResult(CoCreateGuid((GUID *)&referencedMobID));
		checkResult(pReferencedMob->SetMobID(&referencedMobID));
		checkResult(pReferencedMob->SetName(L"AAFSourceClipTest::ReferencedMob"));

		// Create a Composition Mob
		checkResult(pDictionary->CreateInstance(&AUID_AAFCompositionMob,
											  IID_IAAFCompositionMob, 
											  (IUnknown **)&pCompMob));

	    // get a IAAFMob interface
		checkResult(pCompMob->QueryInterface(IID_IAAFMob, (void **)&pMob));
		checkResult(CoCreateGuid((GUID *)&NewMobID));
		checkResult(pMob->SetMobID(&NewMobID));
		checkResult(pMob->SetName(L"AAFSelectorTest"));
	  
		// Create a Source clip 
 		checkResult(pDictionary->CreateInstance(&AUID_AAFSourceClip,
						     IID_IAAFSourceClip, 
						     (IUnknown **)&pSourceClip));		

		// Set the properties for the SourceClip
		checkResult(pSourceClip->SetFade( fadeInLen, fadeInType, fadeOutLen, fadeOutType));
		sourceRef.sourceID = referencedMobID;
		sourceRef.sourceSlotID = 0;
		sourceRef.startTime = 0;
		checkResult(pSourceClip->SetSourceReference(sourceRef));

		// create a filler 
	    checkResult(pDictionary->CreateInstance(&AUID_AAFFiller,
												IID_IAAFFiller, 
												(IUnknown **)&pFiller));
		// Set its properties.
	    checkResult(pFiller->Initialize( &fillerUID, fillerLength));

		// Now create a selector 
	    checkResult(pDictionary->CreateInstance(&AUID_AAFSelector,
												IID_IAAFSelector, 
												(IUnknown **)&pSelector));

		// Get a segment interface from the source clip
		checkResult(pSourceClip->QueryInterface (IID_IAAFSegment, (void **)&pSegment));
		// -----------------------------------------------------------------
		// Set all properties on the Selector
		//	Set the selected segment on the Selector
		checkResult(pSelector->SetSelectedSegment(pSegment));
		// Release the intreface so we can reuse the pointer
		pSegment->Release();
		checkResult(pFiller->QueryInterface(IID_IAAFSegment, (void **)&pSegment));
		checkResult(pSelector->AppendAlternateSegment(pSegment));
		// Release the intreface so we can reuse the pointer
		pSegment->Release();
		checkResult(pSelector->QueryInterface(IID_IAAFSegment, (void **)&pSegment));
	    // append the Selector to the MOB tree
		checkResult(pMob->AppendNewSlot(pSegment, 1, L"SelectorSlot", &pMobSlot)); 
		
		// Add the composition mob to the file
		pHeader->AppendMob(pMob);

	}
	catch (HRESULT& rResult)
	{
		hr = rResult;
	}

	// Cleanup and return
	if (pMobSlot)
		pMobSlot->Release();

	if (pSourceClip)
		pSourceClip->Release();

	if (pFiller)
		pFiller->Release();

	if (pSegment)
		pSegment->Release();

	if (pSelector)
		pSelector->Release();

	if (pCompMob)
		pCompMob->Release();

	if (pMob)
		pMob->Release();

	if (pDictionary)
		pDictionary->Release();

	if (pHeader)
		pHeader->Release();

	if (pFile)
	{
		pFile->Save();
		pFile->Close();
		pFile->Release();
	}

	return hr;
}

static HRESULT ReadAAFFile(aafWChar * pFileName)
{
	IAAFFile*			pFile = NULL;
	IAAFHeader*			pHeader = NULL;
	IEnumAAFMobs*		pMobIter = NULL;
	IAAFMob*			pMob = NULL;
	IEnumAAFMobSlots*	pSlotIter = NULL;
	IAAFMobSlot*		pSlot = NULL;
	IAAFCompositionMob*	pCompMob = NULL;
	IAAFSegment*		pSegment = NULL;
	IAAFSegment*		pSelSegment = NULL;
	IAAFSegment*		pAltSegment = NULL;
	IAAFFiller*			pFiller = NULL;
	IAAFSelector*		pSelector = NULL;
	IAAFSourceClip*		pSourceClip = NULL;
	IEnumAAFSegments*	pAlternateSegIter = NULL;

	aafNumSlots_t		numMobs;
	aafInt32			numAlternateSegs;

	aafSearchCrit_t		criteria;
	HRESULT				hr = AAFRESULT_SUCCESS;


	try
	{
		// Open the AAF file
		checkResult(OpenAAFFile(pFileName, kMediaOpenReadOnly, &pFile, &pHeader));

		// Validate that there is only one composition mob.
		checkResult(pHeader->GetNumMobs(kCompMob, &numMobs));
		checkExpression(1 == numMobs, AAFRESULT_TEST_FAILED);

		// Enumerate over Composition MOBs
		criteria.searchTag = kByMobKind;
		criteria.tags.mobKind = kCompMob;
		checkResult(pHeader->EnumAAFAllMobs(&criteria, &pMobIter));
		while (pMobIter && pMobIter->NextOne(&pMob) == AAFRESULT_SUCCESS)
		{
			aafNumSlots_t		numSlots = 0;

			checkResult(pMob->GetNumSlots(&numSlots));
			checkExpression(1 == numSlots, AAFRESULT_TEST_FAILED);

			// Enumerate over all MOB slots for this MOB
			checkResult(pMob->EnumAAFAllMobSlots(&pSlotIter));
			while (pSlotIter && pSlotIter->NextOne(&pSlot) == AAFRESULT_SUCCESS)
			{
				checkResult(pSlot->GetSegment(&pSegment));
				// See if it is a Selector
				checkResult(pSegment->QueryInterface(IID_IAAFSelector, (void **) &pSelector));

				// -----------------------------------------------------------
				// Get Selector Properties
				checkResult(pSelector->GetNumAlternateSegments(&numAlternateSegs));
				checkExpression(1 == numAlternateSegs, AAFRESULT_TEST_FAILED);

				checkResult(pSelector->GetSelectedSegment(&pSelSegment));
				// Make sure it is a Source clip
				checkResult(pSelSegment->QueryInterface(IID_IAAFSourceClip, (void **)&pSourceClip));
				
				// Enumerate alternates
				checkResult(pSelector->EnumAlternateSegments(&pAlternateSegIter));
				while (pAlternateSegIter && pAlternateSegIter->NextOne(&pAltSegment) == AAFRESULT_SUCCESS)
				{
					// Make sure Alternate segment is a filler
					checkResult(pAltSegment->QueryInterface(IID_IAAFFiller, (void **)&pFiller));
					pFiller->Release();
					pFiller = NULL;
				}
				pAlternateSegIter->Release();
				pAlternateSegIter = NULL;

				pSourceClip->Release();
				pSourceClip = NULL;

				pSelSegment->Release();
				pSelSegment = NULL;

				pSegment->Release();
				pSegment = NULL;

				pSlot->Release();
				pSlot = NULL;
			}

			pSlotIter->Release();
			pSlotIter = NULL;
		}
		pMobIter->Release();
		pMobIter = NULL;
	}
	catch (HRESULT& rResult)
	{
	    hr = rResult;
	}

	// Cleanup object references
	if (pSourceClip)
		pSourceClip->Release();

	if (pCompMob)
		pCompMob->Release();

	if (pSegment)
		pSegment->Release();

	if (pSelSegment)
		pSelSegment->Release();

	if (pAltSegment)
		pAltSegment->Release();

	if (pSlot)
		pSlot->Release();

	if (pSlotIter)
		pSlotIter->Release();

	if (pMob)
		pMob->Release();

	if (pMobIter)
		pMobIter->Release();

	if (pAlternateSegIter)
		pAlternateSegIter->Release();

	if (pFiller)
		pFiller->Release();

	if (pHeader) pHeader->Release();

	if (pFile)
	{
		pFile->Close();
		pFile->Release();
	}


	return 	hr;
}

HRESULT CEnumAAFSegments::test()
{
	HRESULT hr = AAFRESULT_NOT_IMPLEMENTED;
 	aafWChar * pFileName = L"EnumAAFSegmentsTest.aaf";

	try
	{
		hr = CreateAAFFile(	pFileName );
		if(hr == AAFRESULT_SUCCESS)
			hr = ReadAAFFile( pFileName );
	}
	catch (...)
	{
	  cerr << "CEnumAAFSegments::test...Caught general C++"
		" exception!" << endl; 
	  hr = AAFRESULT_TEST_FAILED;
	}

	return hr;
}















