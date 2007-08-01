//=---------------------------------------------------------------------=
//
// $Id$ $Name$
//
// The contents of this file are subject to the AAF SDK Public
// Source License Agreement (the "License"); You may not use this file
// except in compliance with the License.  The License is available in
// AAFSDKPSL.TXT, or you may obtain a copy of the License from the AAF
// Association or its successor.
//
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See
// the License for the specific language governing rights and limitations
// under the License.
//
// The Original Code of this file is Copyright 1998-2005, Licensor of the
// AAF Association.
//
// The Initial Developer of the Original Code of this file and the
// Licensor of the AAF Association is Avid Technology.
// All rights reserved.
//
//=---------------------------------------------------------------------=


#include "AAF.h"


#include <iostream>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "AAFStoredObjectIDs.h"
#include "AAFResult.h"
#include "ModuleTest.h"
#include "AAFDefUIDs.h"

#include "CAAFBuiltinDefs.h"

static const aafWChar *slotNames[5] = { L"SLOT1", L"SLOT2", L"SLOT3", L"SLOT4", L"SLOT5" };
static const aafWChar* TagNames[3] = { L"TAG01", L"TAG02", L"TAG03" };
static const aafWChar* Comments[3] = { L"Comment 1", L"Comment 2", L"Comment 3"};	

static const 	aafMobID_t	TEST_MobID =
{{0x06, 0x0c, 0x2b, 0x34, 0x02, 0x05, 0x11, 0x01, 0x01, 0x00, 0x10, 0x00},
0x13, 0x00, 0x00, 0x00,
{0xa5691d12, 0x0406, 0x11d4, {0x8e, 0x3d, 0x00, 0x90, 0x27, 0xdf, 0xca, 0x7c}}};


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

static HRESULT CreateAAFFile(
    aafWChar * pFileName,
    aafUID_constref fileKind,
    testRawStorageType_t rawStorageType,
    aafProductIdentification_constref productID)
{
	IAAFFile *					pFile = NULL;
  bool bFileOpen = false;
  IAAFHeader *        pHeader = NULL;
  IAAFDictionary*  pDictionary = NULL;
	IAAFMob						*pMob = NULL;
	IAAFTimelineMobSlot	*newSlot = NULL;
	IAAFSegment		*seg = NULL;
	IAAFSourceClip	*sclp = NULL;
	IAAFComponent	*pComponent = NULL;
	HRESULT						hr = S_OK;

	try
	{
	  // Remove the previous test file if any.
	  RemoveTestFile(pFileName);

	  // Create the file.
	  checkResult(CreateTestFile( pFileName, fileKind, rawStorageType, productID, &pFile ));
	  bFileOpen = true;
 
	  // We can't really do anthing in AAF without the header.
	  checkResult(pFile->GetHeader(&pHeader));

	  // Get the AAF Dictionary so that we can create valid AAF objects.
	  checkResult(pHeader->GetDictionary(&pDictionary));
	  CAAFBuiltinDefs defs (pDictionary);
 		
	  //Make the first mob
	  long	test;

	  // Create a concrete subclass of Mob
	  checkResult(defs.cdMasterMob()->
				  CreateInstance(IID_IAAFMob, 
								 (IUnknown **)&pMob));

	  checkResult(pMob->SetMobID(TEST_MobID));
	  checkResult(pMob->SetName(L"EnumAAFTaggedValuesTest"));
		// append some comments to this mob !!
	  for (test = 0; test < 3; test++)
	  {
		  checkResult(pMob->AppendComment(const_cast<aafWChar*>(TagNames[test]), Comments[test]));
	  }
	  // Add some slots
	  for(test = 0; test < 5; test++)
	  {
 		  checkResult(defs.cdSourceClip()->
					  CreateInstance(IID_IAAFSourceClip, 
									 (IUnknown **)&sclp));		
		 checkResult(sclp->QueryInterface(IID_IAAFComponent, (void **)&pComponent));
		 checkResult(pComponent->SetDataDef(defs.ddkAAFPicture()));
		pComponent->Release();
		pComponent = NULL;

		  checkResult(sclp->QueryInterface (IID_IAAFSegment, (void **)&seg));

		  aafRational_t editRate = { 0, 1};
		  checkResult(pMob->AppendNewTimelineSlot (editRate,
												   seg,
												   test+1,
												   slotNames[test],
												   0,
												   &newSlot));

		  newSlot->Release();
		  newSlot = NULL;

		  seg->Release();
		  seg = NULL;

		  sclp->Release();
		  sclp = NULL;
	  }

    // Add the mob to the file.
	  checkResult(pHeader->AddMob(pMob));
  }
  catch (HRESULT& rResult)
  {
    hr = rResult;
  }


  // Cleanup and return
  if (newSlot)
    newSlot->Release();

	if (pComponent)
		pComponent->Release();

  if (seg)
    seg->Release();

  if (sclp)
    sclp->Release();

  if (pMob)
    pMob->Release();

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
  IEnumAAFTaggedValues* pCloneIterator = NULL;
  IAAFTaggedValue*		pComment = NULL;

  IAAFMobSlot		*slot = NULL;
  aafNumSlots_t	numMobs, n, slt;
  aafUInt32		numComments, bytesRead, com;
  HRESULT						hr = S_OK;
  aafWChar		tag[64];
  aafWChar		Value[64];


  try
  {
    // Open the file
		checkResult(AAFFileOpenExistingRead(pFileName, 0, &pFile));
		bFileOpen = true;

    // We can't really do anthing in AAF without the header.
  	checkResult(pFile->GetHeader(&pHeader));


	  checkResult(pHeader->CountMobs(kAAFAllMob, &numMobs));
	  checkExpression(1 == numMobs, AAFRESULT_TEST_FAILED);


	  aafSearchCrit_t		criteria;
	  criteria.searchTag = kAAFNoSearch;
    checkResult(pHeader->GetMobs (&criteria, &mobIter));

    for(n = 0; n < numMobs; n++)
	  {
		  aafWChar		name[500], slotName[500];
		  aafNumSlots_t	numSlots;
		  aafMobID_t		mobID;
		  aafSlotID_t		trackID;

		  checkResult(mobIter->NextOne (&aMob));
		  checkResult(aMob->GetName (name, sizeof(name)));
		  checkResult(aMob->GetMobID (&mobID));

		  // Check for comments
		  checkResult(aMob->CountComments(&numComments));
		  checkExpression(3 == numComments, AAFRESULT_TEST_FAILED);
		  checkResult(aMob->GetComments(&pCommentIterator));
		  for(com = 0; com < numComments; com++)
		  {
			  checkResult(pCommentIterator->NextOne(&pComment));
			  checkResult(pComment->GetName(tag, sizeof(tag)));
			  checkResult(pComment->GetValue( sizeof(Value), (unsigned char *)Value, &bytesRead));
			  checkExpression(wcscmp(tag, TagNames[com])== 0, AAFRESULT_TEST_FAILED);
			  checkExpression(wcscmp(Value, Comments[com])== 0, AAFRESULT_TEST_FAILED);
			  pComment->Release();
        pComment = NULL;
		  }
		  // now check reset
		  checkResult(pCommentIterator->Reset());
		  checkResult(pCommentIterator->NextOne(&pComment));
		  checkResult(pComment->GetName(tag, sizeof(tag)));
		  checkResult(pComment->GetValue( sizeof(Value), (unsigned char *)Value, &bytesRead));
		  checkExpression(wcscmp(tag, TagNames[0])== 0, AAFRESULT_TEST_FAILED);
		  checkExpression(wcscmp(Value, Comments[0])== 0, AAFRESULT_TEST_FAILED);
		  pComment->Release();
      pComment = NULL;
		  // test skip
		  checkResult(pCommentIterator->Skip(1));
		  checkResult(pCommentIterator->NextOne(&pComment));
		  checkResult(pComment->GetName(tag, sizeof(tag)));
		  checkResult(pComment->GetValue( sizeof(Value), (unsigned char *)Value, &bytesRead));
		  checkExpression(wcscmp(tag, TagNames[2])== 0, AAFRESULT_TEST_FAILED);
		  checkExpression(wcscmp(Value, Comments[2])== 0, AAFRESULT_TEST_FAILED);
		  pComment->Release();
      pComment = NULL;
		  // test clone
		  checkResult(pCommentIterator->Clone(&pCloneIterator));
		  checkResult(pCloneIterator->Reset());
		  checkResult(pCloneIterator->NextOne(&pComment));
		  checkResult(pComment->GetName(tag, sizeof(tag)));
		  checkResult(pComment->GetValue( sizeof(Value), (unsigned char *)Value, &bytesRead));
		  checkExpression(wcscmp(tag, TagNames[0])== 0, AAFRESULT_TEST_FAILED);
		  checkExpression(wcscmp(Value, Comments[0])== 0, AAFRESULT_TEST_FAILED);
		  pComment->Release();
		  pComment = NULL;
		
		  pCommentIterator->Release();
      pCommentIterator = NULL;
		  pCloneIterator->Release();
      pCloneIterator = NULL;
			
		  checkResult(aMob->CountSlots (&numSlots));
		  checkExpression(5 == numSlots, AAFRESULT_TEST_FAILED);

			checkResult(aMob->GetSlots(&slotIter));

			for(slt = 0; slt < numSlots; slt++)
			{
				checkResult(slotIter->NextOne (&slot));
				checkResult(slot->GetName (slotName, sizeof(slotName)));
				checkResult(slot->GetSlotID(&trackID));
				checkExpression (wcscmp(slotName, slotNames[slt]) == 0, AAFRESULT_TEST_FAILED);

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

  if (pComment)
    pComment->Release();

  if (pCloneIterator)
    pCloneIterator->Release();

  if (pCommentIterator)
    pCommentIterator->Release();

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
 
	    
extern "C" HRESULT CEnumAAFTaggedValues_test(
    testMode_t mode,
    aafUID_t fileKind,
    testRawStorageType_t rawStorageType,
    aafProductIdentification_t productID);
extern "C" HRESULT CEnumAAFTaggedValues_test(
    testMode_t mode,
    aafUID_t fileKind,
    testRawStorageType_t rawStorageType,
    aafProductIdentification_t productID)
{
	HRESULT hr = AAFRESULT_NOT_IMPLEMENTED;
	const size_t fileNameBufLen = 128;
	aafWChar pFileName[ fileNameBufLen ] = L"";
	GenerateTestFileName( productID.productName, fileKind, fileNameBufLen, pFileName );

	try
	{
		if(mode == kAAFUnitTestReadWrite)
			hr = CreateAAFFile(pFileName, fileKind, rawStorageType, productID);
		else
			hr = AAFRESULT_SUCCESS;
		if(hr == AAFRESULT_SUCCESS)
			hr = ReadAAFFile( pFileName );
	}
	catch (...)
	{
	  cerr << "CEnumAAFTaggedValues_test...Caught general C++"
		   << " exception!" << endl; 
	  hr = AAFRESULT_TEST_FAILED;
	}

  // Cleanup our object if it exists.

  	// When all of the functionality of this class is tested, we can return success

  return hr;
}






























































////////////////////////////////////////////////////////////////////////////////
