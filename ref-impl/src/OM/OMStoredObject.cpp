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

// @doc OMEXTERNAL
// @author Tim Bingham | tjb | Avid Technology, Inc. | OMStoredObject
#include "OMStoredObject.h"

#include "OMUtilities.h"
#include "OMDataStream.h"

#include <stdio.h>

  // @mfunc Destructor.
OMStoredObject::~OMStoredObject(void)
{
  TRACE("OMStoredObject::~OMStoredObject");
}

void OMStoredObject::save(OMFile& file, bool /*finalize*/)
{
  TRACE("OMStoredObject::save");

  save(file);
}

wchar_t* OMStoredObject::streamName(const wchar_t* propertyName,
                                    OMPropertyId pid)
{
  return referenceName(propertyName, pid);
}

wchar_t* OMStoredObject::referenceName(const wchar_t* propertyName,
                                       OMPropertyId pid)
{
  TRACE("OMStoredObject::referenceName");

  wchar_t* result = new wchar_t[32];
  ASSERT("Valid heap pointer", result != 0);

  mangleName(propertyName, pid, result, 32);

  return result;
}

wchar_t* OMStoredObject::collectionName(const wchar_t* propertyName,
                                        OMPropertyId pid)
{
  TRACE("OMStoredObject::collectionName");

  wchar_t* result = new wchar_t[32];
  ASSERT("Valid heap pointer", result != 0);

  mangleName(propertyName, pid, result, 32 - 10);

  return result;
}

wchar_t* OMStoredObject::elementName(const wchar_t* propertyName,
                                     OMPropertyId pid,
                                     OMUInt32 localKey)
{
  TRACE("OMStoredObject::elementName");

  wchar_t* result = new wchar_t[32];
  ASSERT("Valid heap pointer", result != 0);

  mangleName(propertyName, pid, result, 32 - 10);

  size_t newSize = lengthOfWideString(result);
  concatenateWideString(result, L"{", 1);
  size_t keySize;
  if (localKey != 0) {
    keySize = stringSize(localKey);
    toWideString(localKey, &result[newSize + 1], keySize);
  } else {
    keySize = 1;
    concatenateWideString(result, L"0", 1);
  }
  concatenateWideString(result, L"}", 1);

  return result;
}

void OMStoredObject::mapCharacters(wchar_t* /* name */,
                                   size_t /* nameLength */)
{
  TRACE("OMStoredObject::mapCharacters");

#if 0
  for (size_t i = 0; i < nameLength; i++) {
    name[i] = map[name[i] & 0x7f];
  }
#endif
}

void OMStoredObject::mangleName(const wchar_t* clearName,
                                OMPropertyId pid,
                                wchar_t* mangledName,
                                size_t mangledNameSize)
{
  TRACE("OMStoredObject::mangleName");

  PRECONDITION("Valid pid", pid > 0);

  // Squeeze name to accommodate the pid as "-<pid>"
  //
  size_t maxSize = mangledNameSize - stringSize(pid) - 1 - 1;
  size_t newSize = squeezeWideString(clearName,
                                     lengthOfWideString(clearName),
                                     mangledName,
                                     maxSize);
  ASSERT("Consistent length", newSize == lengthOfWideString(mangledName));
  ASSERT("Consistent length",
                            lengthOfWideString(mangledName) < mangledNameSize);

  // Map out any illegal characters
  //
  mapCharacters(mangledName, newSize);

  // Append "-<pid>"
  //
  mangledName[newSize] = L'-';
  toWideString(pid, &mangledName[newSize+1], stringSize(pid));
}

#ifdef HACK_SKIP_PRIMARY_MOB_DEF
bool isPrimaryMobPresent(const OMFile* file)
{
  static const OMPropertyId PID_Root_Header       = 0x0002;
  static const OMPropertyId PID_Header_PrimaryMob = 0x3b08;
  static const OMPropertyId propertyPath[] = {
    PID_Root_Header, PID_Header_PrimaryMob, 0};

  const OMProperty* primaryMobProperty =
                           file->findProperty(propertyPath);
  const bool present = primaryMobProperty && primaryMobProperty->isPresent();
  return present;
}

#include "OMContainerElement.h"
#include "OMStrongReferenceSet.h"
bool elementHasPrimaryMobDefinition(const OMStrongReferenceSet& set,
                                    const OMStrongReferenceSetElement& element)
{
  static const OMPropertyId PID_ClassDefinition_Properties = 0x0009;
  static const OMUniqueObjectIdentification Property_PrimaryMob =
  {0x06010104, 0x0108, 0x0000, {0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x04}};

  bool result = false;
  const bool isPropertyDefinitionsSet =
	                        (set.propertyId() == PID_ClassDefinition_Properties);
  if (isPropertyDefinitionsSet) {
    const OMUniqueObjectIdentification* id =
      reinterpret_cast<const OMUniqueObjectIdentification*>(
                                                     element.identification());
    result = (id && *id == Property_PrimaryMob);
  }

  return result;
}

bool setHasPrimaryMobDefinition(const OMStrongReferenceSet& set)
{
  static const OMPropertyId PID_ClassDefinition_Properties = 0x0009;
  static const OMUniqueObjectIdentification Property_PrimaryMob =
  {0x06010104, 0x0108, 0x0000, {0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x04}};

  bool result = false;
  const bool isPropertyDefinitionsSet =
	                        (set.propertyId() == PID_ClassDefinition_Properties);
  if (isPropertyDefinitionsSet) {
	  result = set.contains(const_cast<OMUniqueObjectIdentification*>(&Property_PrimaryMob));
  }

  return result;
}
#endif
