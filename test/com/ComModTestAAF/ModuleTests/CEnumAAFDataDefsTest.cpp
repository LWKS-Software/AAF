// @doc INTERNAL
// @com This file implements the module test for CEnumAAFDataDef
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
 *  prior written permission of Avid Technology, Inc.
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

#include <iostream.h>
#include <stdio.h>

#include "AAFStoredObjectIDs.h"
#include "AAFResult.h"
#include "AAFDataDefs.h"
#include "AAFDefUIDs.h"

#define kNumComponents	5



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
						   IAAFFile**			ppFile,
						   IAAFHeader**			ppHeader)
{
	aafProductIdentification_t	ProductInfo;
	HRESULT						hr = AAFRESULT_SUCCESS;

	ProductInfo.companyName = L"AAF Developers Desk";
	ProductInfo.productName = L"EnumAAFDataDef Test";
	ProductInfo.productVersion.major = 1;
	ProductInfo.productVersion.minor = 0;
	ProductInfo.productVersion.tertiary = 0;
	ProductInfo.productVersion.patchLevel = 0;
	ProductInfo.productVersion.type = kVersionUnknown;
	ProductInfo.productVersionString = NULL;
	ProductInfo.productID = UnitTestProductID;
	ProductInfo.platform = NULL;

	*ppFile = NULL;

	switch (mode)
	{
	case kMediaOpenReadOnly:
		hr = AAFFileOpenExistingRead(pFileName, 0, ppFile);
		break;

	case kMediaOpenAppend:
		hr = AAFFileOpenNewModify(pFileName, 0, &ProductInfo, ppFile);
		break;

	default:
		hr = AAFRESULT_TEST_FAILED;
		break;
	}

	if (FAILED(hr))
	{
		if (*ppFile)
		{
			(*ppFile)->Release();
			*ppFile = NULL;
		}
		return hr;
	}
  
  	hr = (*ppFile)->GetHeader(ppHeader);
	if (FAILED(hr))
	{
		(*ppFile)->Release();
		*ppFile = NULL;
		return hr;
	}
 	
	return hr;
}

static HRESULT CreateAAFFile(aafWChar * pFileName)
{
	IAAFFile*		pFile = NULL;
	IAAFHeader*		pHeader = NULL;
	IAAFDictionary*  pDictionary = NULL;
	IAAFMob*		pMob = NULL;
	IAAFMobSlot*	pMobSlot = NULL;
	IAAFSequence*	pSequence = NULL;
	IAAFSegment*	pSegment = NULL;
	IAAFComponent*	pComponent = NULL;
	aafUID_t		NewMobID;
	int				i;
	HRESULT			hr = S_OK;
	
	
	try
	{  
		// Remove the previous test file if any.
		RemoveTestFile(pFileName);
		
		
		// Create the AAF file
		checkResult(OpenAAFFile(pFileName, kMediaOpenAppend, &pFile, &pHeader));
		
		// Get the AAF Dictionary so that we can create valid AAF objects.
		checkResult(pHeader->GetDictionary(&pDictionary));
		
		// Create a Composition Mob
		checkResult(pDictionary->CreateInstance(&AUID_AAFCompositionMob,
			IID_IAAFMob, 
			(IUnknown **)&pMob));
		
		checkResult(CoCreateGuid((GUID *)&NewMobID));
		checkResult(pMob->SetMobID(&NewMobID));
		checkResult(pMob->SetName(L"EnumAAFDataDefTest"));
		
		// Add mob slot w/ Sequence
		checkResult(pDictionary->CreateInstance(&AUID_AAFSequence,
			IID_IAAFSequence, 
			(IUnknown **)&pSequence));		
		checkResult(pSequence->Initialize((aafUID_t*)&DDEF_Picture));
		
		//
		//	Add some segments.  Need to test failure conditions
		//	(i.e. starting/ending w/ transition, two trans back
		//	to bacl).
		//
		for(i = 0; i < kNumComponents; i++)
		{
			aafLength_t		len = 10;
			
			checkResult(pDictionary->CreateInstance(&AUID_AAFFiller,
				IID_IAAFComponent, 
				(IUnknown **)&pComponent));
			
			if(i == 0)
			{
				checkResult(pComponent->SetDataDef((aafUID_t*)&DDEF_PictureWithMatte));
			}
			else
			{
				checkResult(pComponent->SetDataDef((aafUID_t*)&DDEF_Picture));
			}

			checkResult(pComponent->SetLength(&len));
			checkResult(pSequence->AppendComponent(pComponent));
			
			pComponent->Release();
			pComponent = NULL;
		}
		
		checkResult(pSequence->QueryInterface (IID_IAAFSegment, (void **)&pSegment));
		
		checkResult(pMob->AppendNewSlot(pSegment, 1, L"AAF Test Sequence", &pMobSlot));
		
		pMobSlot->Release();
		pMobSlot = NULL;
		
		pSegment->Release();
		pSegment = NULL;
		
		// Add the master mob to the file and cleanup
		pHeader->AppendMob(pMob);
		
	}
	catch (HRESULT& rResult)
	{
		hr = rResult;
	}
	
	
	// Cleanup and return
	if (pMobSlot)
		pMobSlot->Release();
	
	if (pSegment)
		pSegment->Release();
	
	if (pComponent)
		pComponent->Release();
	
	if (pSequence)
		pSequence->Release();
	
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

static HRESULT ReadAAFFile(aafWChar* pFileName)
{
	IAAFFile*		pFile = NULL;
	IAAFHeader*		pHeader = NULL;
	IEnumAAFMobs*	pMobIter = NULL;
	IAAFMob*		pMob;
	IEnumAAFMobSlots*	pSlotIter = NULL;
	IAAFMobSlot*		pSlot = NULL;
	IAAFComponent*		pComp = NULL;
	IAAFSegment*		pSegment = NULL;
	IAAFDataDef*		pDataDef = NULL;
	IAAFSequence*		pSequence = NULL;
	IAAFDictionary*		pDictionary = NULL;
	IEnumAAFDataDefs*	pEnumDataDef = NULL;
	IEnumAAFDataDefs*	pCloneEnum = NULL;
	IEnumAAFComponents*	pCompIter = NULL;
	IAAFDataDef*		pArray[2] = { NULL, NULL };
	aafNumSlots_t		numMobs;
	aafInt32			index;
	aafSearchCrit_t		criteria;
	HRESULT				hr = S_OK;
	aafBool				testBool;
	aafUInt32			resultCount;
	
	try
	{
		// Open the AAF file
		checkResult(OpenAAFFile(pFileName, kMediaOpenReadOnly, &pFile, &pHeader));
		
		// Validate that there is only one composition mob.
		checkResult(pHeader->GetNumMobs(kCompMob, &numMobs));
		checkExpression(1 == numMobs, AAFRESULT_TEST_FAILED);
		
		// Get the AAF Dictionary so that we can create valid AAF objects.
		checkResult(pHeader->GetDictionary(&pDictionary));
		
		checkResult(pDictionary->GetDataDefinitions(&pEnumDataDef));
		/* Read and check the first element */
		checkResult(pEnumDataDef->NextOne(&pDataDef));
		checkResult(pDataDef->IsPictureKind(&testBool));
		checkExpression(testBool == AAFTrue, AAFRESULT_TEST_FAILED);
		checkResult(pDataDef->IsSoundKind(&testBool));
		checkExpression(testBool == AAFFalse, AAFRESULT_TEST_FAILED);
		pDataDef->Release();
		pDataDef = NULL;
		
		/**/
		/* Read and check the second element */
		checkResult(pEnumDataDef->NextOne(&pDataDef));
		checkResult(pDataDef->IsSoundKind(&testBool));
		checkExpression(testBool == AAFTrue, AAFRESULT_TEST_FAILED);
		checkResult(pDataDef->IsPictureKind(&testBool));
		checkExpression(testBool == AAFFalse, AAFRESULT_TEST_FAILED);
		pDataDef->Release();
		pDataDef = NULL;
		/*****/
		
		/* Reset, and check the first element again*/
		checkResult(pEnumDataDef->Reset());
		checkResult(pEnumDataDef->NextOne(&pDataDef));
		checkResult(pDataDef->IsPictureKind(&testBool));
		checkExpression(testBool == AAFTrue, AAFRESULT_TEST_FAILED);
		checkResult(pDataDef->IsSoundKind(&testBool));
		checkExpression(testBool == AAFFalse, AAFRESULT_TEST_FAILED);
		pDataDef->Release();
		pDataDef = NULL;
		
		/* Reset, Skip, and check the second element again*/
		checkResult(pEnumDataDef->Reset());
		checkResult(pEnumDataDef->Skip(1));
		checkResult(pEnumDataDef->NextOne(&pDataDef));
		checkResult(pDataDef->IsSoundKind(&testBool));
		checkExpression(testBool == AAFTrue, AAFRESULT_TEST_FAILED);
		checkResult(pDataDef->IsPictureKind(&testBool));
		checkExpression(testBool == AAFFalse, AAFRESULT_TEST_FAILED);
		pDataDef->Release();
		pDataDef = NULL;
		
		/* Reset, and read both elements */
		checkResult(pEnumDataDef->Reset());
		checkResult(pEnumDataDef->Next (2, (IAAFDataDef **)&pArray, &resultCount));
		checkExpression (resultCount == 2, AAFRESULT_TEST_FAILED);
		checkResult(pArray[0]->IsPictureKind(&testBool));
		checkExpression(testBool == AAFTrue, AAFRESULT_TEST_FAILED);
		checkResult(pArray[0]->IsSoundKind(&testBool));
		checkExpression(testBool == AAFFalse, AAFRESULT_TEST_FAILED);
		pArray[0]->Release();
		pArray[0] = NULL;
		
		checkResult(pArray[1]->IsSoundKind(&testBool));
		checkExpression(testBool == AAFTrue, AAFRESULT_TEST_FAILED);
		checkResult(pArray[1]->IsPictureKind(&testBool));
		checkExpression(testBool == AAFFalse, AAFRESULT_TEST_FAILED);
		pArray[1]->Release();
		pArray[1] = NULL;
		
//		/* Read one past to make sure that it fails */
//		checkExpression(pEnumDataDef->NextOne(&pDataDef) != AAFRESULT_SUCCESS, AAFRESULT_TEST_FAILED);
		/* Clone the enumerator, and read one element */
		checkResult(pEnumDataDef->Clone(&pCloneEnum));
		checkResult(pCloneEnum->Reset());
		checkResult(pCloneEnum->NextOne(&pDataDef));
		checkResult(pDataDef->IsPictureKind(&testBool));
		checkExpression(testBool == AAFTrue, AAFRESULT_TEST_FAILED);
		checkResult(pDataDef->IsSoundKind(&testBool));
		checkExpression(testBool == AAFFalse, AAFRESULT_TEST_FAILED);
		pDataDef->Release();
		pDataDef = NULL;
		pCloneEnum->Release();
		pCloneEnum = NULL;
		
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
				aafInt32			numCpnts;
				
				checkResult(pSlot->GetSegment(&pSegment));
				checkResult(pSegment->QueryInterface(IID_IAAFSequence, (void **) &pSequence));
				
				checkResult(pSequence->GetNumComponents(&numCpnts));
				checkExpression(numCpnts == kNumComponents, AAFRESULT_TEST_FAILED);
				
				checkResult(pSequence->EnumComponents(&pCompIter));
				numCpnts = 0;
				index = 0;
				while (pCompIter && pCompIter->NextOne(&pComp) == AAFRESULT_SUCCESS)
				{
					aafUID_t	dataDef, pictureID = DDEF_Picture, pwmID = DDEF_PictureWithMatte;
					aafUID_t	soundID = DDEF_Sound;
					aafBool		testBool;
					
					numCpnts++;
					
					checkResult(pComp->GetDataDef(&dataDef));
					checkResult(pDictionary->LookupDataDefinition(&dataDef, &pDataDef));
					checkResult(pDataDef->IsSoundKind(&testBool));
					checkExpression(testBool == AAFFalse, AAFRESULT_TEST_FAILED);
					checkResult(pDataDef->IsMatteKind(&testBool));
					checkExpression(testBool == AAFFalse, AAFRESULT_TEST_FAILED);
					
					if(index == 0)	// First segment is Picture with Matte, converts to picture
					{
						checkResult(pDataDef->IsDataDefOf(&pwmID, &testBool));
						checkExpression(testBool == AAFTrue, AAFRESULT_TEST_FAILED);
						checkResult(pDataDef->IsPictureKind(&testBool));
						checkExpression(testBool == AAFFalse, AAFRESULT_TEST_FAILED);
						checkResult(pDataDef->IsPictureWithMatteKind(&testBool));
						checkExpression(testBool == AAFTrue, AAFRESULT_TEST_FAILED);
						checkResult(pDataDef->DoesDataDefConvertTo (&pictureID, &testBool));
						checkExpression(testBool == AAFTrue, AAFRESULT_TEST_FAILED);
					}
					else		// First segment is Picture, converts from picture with Matte
					{
						checkResult(pDataDef->IsDataDefOf(&pictureID, &testBool));
						checkExpression(testBool == AAFTrue, AAFRESULT_TEST_FAILED);
						checkResult(pDataDef->IsPictureKind(&testBool));
						checkExpression(testBool == AAFTrue, AAFRESULT_TEST_FAILED);
						checkResult(pDataDef->IsPictureWithMatteKind(&testBool));
						checkExpression(testBool == AAFFalse, AAFRESULT_TEST_FAILED);
						checkResult(pDataDef->DoesDataDefConvertFrom (&pwmID, &testBool));
						checkExpression(testBool == AAFTrue, AAFRESULT_TEST_FAILED);
					}
					checkResult(pDataDef->DoesDataDefConvertTo (&soundID, &testBool));
					checkExpression(testBool == AAFFalse, AAFRESULT_TEST_FAILED);
					checkResult(pDataDef->DoesDataDefConvertFrom (&soundID, &testBool));
					checkExpression(testBool == AAFFalse, AAFRESULT_TEST_FAILED);
					
					pComp->Release();
					pComp = NULL;
					pDataDef->Release();
					pDataDef = NULL;
					index++;
				}
				
				pCompIter->Release();
				pCompIter = NULL;
				
				pSequence->Release();
				pSequence = NULL;
				
				pSegment->Release();
				pSegment = NULL;
				
				pSlot->Release();
				pSlot = NULL;
			}
			
			pSlotIter->Release();
			pSlotIter = NULL;
			
			pMob->Release();
			pMob = NULL;
		}
		
		
	}
	catch (HRESULT& rResult)
	{
		hr = rResult;
	}
	
	// Cleanup object references
	if (pComp)
		pComp->Release();
	

	if (pEnumDataDef)
		pEnumDataDef->Release();
	if (pCloneEnum)
		pCloneEnum->Release();
	if (pCompIter)
		pCompIter->Release();
	if (pArray[0])
		pArray[0]->Release();
	if (pArray[1])
		pArray[1]->Release();
	
	if (pDataDef)
		pDataDef->Release();
	
	if (pSequence)
		pSequence->Release();
	
	if (pSegment)
		pSegment->Release();
	
	if (pSlot)
		pSlot->Release();
	
	if (pDictionary)
		pDictionary->Release();
	
	if (pSlotIter)
		pSlotIter->Release();
	
	if (pMob)
		pMob->Release();
	
	if (pMobIter)
		pMobIter->Release();
	
	if (pHeader) pHeader->Release();
	
	if (pFile)
	{
		pFile->Close();
		pFile->Release();
	}
	
	
	return 	hr;
}

extern "C" HRESULT CEnumAAFDataDefs_test()
{
	HRESULT hr = AAFRESULT_NOT_IMPLEMENTED;
	aafWChar * pFileName = L"EnumAAFDataDefTest.aaf";

	try
	{
		hr = CreateAAFFile(pFileName);
		if (SUCCEEDED(hr))
			hr = ReadAAFFile(pFileName);
	}
	catch (...)
	{
		cerr << "CEnumAAFDataDef_test...Caught general C++ exception!" << endl; 
	}

	// When all of the functionality of this class is tested, we can return success.
	// When a method and its unit test have been implemented, remove it from the list.
//	if (SUCCEEDED(hr))
//	{
//		cout << "The following EnumAAFDataDef methods have not been implemented:" << endl; 
//		cout << "     IsDataDefOf - needs unit test" << endl; 
//		hr = AAFRESULT_TEST_PARTIAL_SUCCESS;
//	}

	return hr;
}
