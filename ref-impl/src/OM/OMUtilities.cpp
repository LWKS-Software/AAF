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
// @author Tim Bingham | tjb | Avid Technology, Inc. | OMUtilities

#include "OMUtilities.h"

#include "OMAssertions.h"
#include "OMPortability.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "utf8.h"

OMByteOrder hostByteOrder(void)
{
  TRACE("hostByteOrder");

  OMUInt32 longWord = 0x12345678;
  OMUInt8  byte = *((OMInt8*)&longWord);
  OMByteOrder result;

  ASSERT("Valid byte order", ((byte == 0x12) || (byte == 0x78)));

  if (byte == 0x12) {
    result = bigEndian;
  } else {
    result = littleEndian;
  }
  return result;
}

size_t lengthOfWideString(const wchar_t* string)
{
  TRACE("lengthOfWideString");

  PRECONDITION("Valid string", validWideString(string));
  const wchar_t* p = string;
  size_t length = 0;
  while (*p != 0) {
    ++length;
    ++p;
  }

  return length;
}

wchar_t* copyWideString(wchar_t* destination,
                        const wchar_t* source,
                        size_t length)
{
  TRACE("copyWideString");

  PRECONDITION("Valid destination", destination != 0);
  PRECONDITION("Valid source string", validWideString(source));
  PRECONDITION("Valid length", length > 0);

  wchar_t* d = destination;
  const wchar_t* s = source;
  size_t i = 0;

  for (i = 0; ((i < length) && (*s != 0)); i++) {
    *d++ = *s++;
  }
  for (i = i; i < length; i++) {
    *d++ = 0;
  }

  return destination;
}

wchar_t* copyWideString(wchar_t* destination,
                        const wchar_t* source)
{
  TRACE("copyWideString");

  PRECONDITION("Valid destination", destination != 0);
  PRECONDITION("Valid source string", validWideString(source));

  wchar_t* d = destination;
  const wchar_t* s = source;

  while(*s != 0) {
    *d++ = *s++;
  }
  *d = 0;

  return destination;
}

wchar_t* concatenateWideString(wchar_t* destination,
                               const wchar_t* source,
                               size_t length)
{
  TRACE("concatenateWideString");

  PRECONDITION("Valid destination", validWideString(destination));
  PRECONDITION("Valid source", validWideString(source));
  PRECONDITION("Valid length", length > 0); // Different to strncat()

  wchar_t* d = destination;
  while (*d != 0) {
    ++d;
  }

  const wchar_t* s = source;
  for (size_t i = 0; ((i < length) && (*s != 0)); i++) {
    *d++ = *s++;
  }
  *d = 0;
  return destination;
}

wchar_t* concatenateWideString(wchar_t* destination,
                               const wchar_t* source)
{
  TRACE("concatenateWideString");

  PRECONDITION("Valid destination", validWideString(destination));
  PRECONDITION("Valid source", validWideString(source));

  wchar_t* d = destination;
  while (*d != 0) {
    ++d;
  }

  const wchar_t* s = source;
  while(*s != 0) {
    *d++ = *s++;
  }
  *d = 0;
  return destination;
}

wchar_t* saveWideString(const wchar_t* string)
{
  TRACE("saveWideString");

  PRECONDITION("Valid string", validWideString(string));
  size_t length = lengthOfWideString(string) + 1;
  wchar_t* result = new wchar_t[length];
  ASSERT("Valid heap pointer", result != 0);
  copyWideString(result, string, length);

  return result;
}

// if string1 >  string2 then result =  1
// if string1 == string2 then result =  0
// if string1 <  string2 then result = -1
int compareWideString(const wchar_t* string1,
                      const wchar_t* string2,
                      size_t length)
{
  TRACE("compareWideString");

  PRECONDITION("Valid string", validWideString(string1));
  PRECONDITION("Valid string", validWideString(string2));
  int result = 0;
  const wchar_t* s1 = string1;
  const wchar_t* s2 = string2;
  for (size_t i = 0; i < length; i++) {
    if (*s1 != *s2) {
      if (*s1 > *s2) {
        result = 1;
      } else {
        result = -1;
      }
      break;
    } else if (*s1 == 0) {
      result = 0;
      break;
    }
    ++s1;
    ++s2;
  }
  return result;
}

int compareWideString(const wchar_t* string1, const wchar_t* string2)
{
  TRACE("compareWideString");

  PRECONDITION("Valid string", validWideString(string1));
  PRECONDITION("Valid string", validWideString(string2));
  int result = 0;
  const wchar_t* s1 = string1;
  const wchar_t* s2 = string2;
  while (*s1 == *s2) {
    if (*s1 == 0) {
      break;
    }
    ++s1;
    ++s2;
  }

  if ((*s1 == 0) && (*s2 == 0)) {
    result = 0;
  } else if (*s1 > *s2) {
    result = 1;
  } else if (*s1 < *s2) {
    result = -1;
  }
  return result;
}

void convertWideStringToString(char*  result,
                               const wchar_t*  string ,
                               size_t  resultSize)
{
  TRACE("convertWideStringToString");
  PRECONDITION("Valid string", validWideString(string));
  PRECONDITION("Valid output buffer", result != 0);
  PRECONDITION("Valid output buffer size", resultSize > 0);

  size_t length = wcsu8slen(string);
  if (length > (resultSize - 1)) {
    length = resultSize - 1;
  }
#if defined(OM_PARAMETER_VALIDATION)
  size_t status =
#endif
  wcstou8s(result, string, resultSize);
  ASSERT("Successful conversion", status != (size_t)-1);
}

void convertStringToWideString(wchar_t* result,
                               const char* string,
                               size_t resultSize)
{
  TRACE("convertStringToWideString");

  PRECONDITION("Valid string", string != 0);
  size_t length = strlen(string);
  if (length > (resultSize - 1)) {
    length = resultSize - 1;
  }
#if defined(OM_PARAMETER_VALIDATION)
  size_t status =
#endif
  u8stowcs(result, string, resultSize);
  ASSERT("Successful conversion", status != (size_t)-1);
}

char* convertWideString(const wchar_t* string)
{
  TRACE("convertWideString");

  PRECONDITION("Valid string", validWideString(string));
  size_t length = lengthOfWideString(string);
  char* result = new char[length + 1];
  ASSERT("Valid heap pointer", result != 0);
#if defined(OM_PARAMETER_VALIDATION)
  size_t status =
#endif
  wcstou8s(result, string, length + 1);
  ASSERT("Successful conversion", status != (size_t)-1);
  ASSERT("Successful conversion", status != length + 1);
  return result;
}

wchar_t* convertString(const char* string)
{
  TRACE("convertString");

  PRECONDITION("Valid string", validString(string));
  size_t length = strlen(string);
  wchar_t* result = new wchar_t[length + 1];
  ASSERT("Valid heap pointer", result != 0);
#if defined(OM_PARAMETER_VALIDATION)
  size_t status =
#endif
  u8stowcs(result, string, length + 1);
  ASSERT("Successful conversion", status != (size_t)-1);
  return result;
}

wchar_t* findWideCharacter(const wchar_t* string, wchar_t character)
{
  const wchar_t* p = string;
  while(*p != 0) {
    if (*p == character) {
      break;
    }
    p++;
  }
  wchar_t* result;
  if (*p == 0) {
    result = 0;
  } else {
    result = const_cast<wchar_t*>(p);
  }
  return result;
}

size_t countWideCharacter(const wchar_t* string, wchar_t character)
{
  size_t result = 0;
  wchar_t* element = const_cast<wchar_t*>(string);
  wchar_t* end = findWideCharacter(element, character);
  while (end != 0) {
    result = result + 1;
    element = ++end;
    end = findWideCharacter(element, character);
  }
  return result;
}

void copyCToPString(unsigned char* pString, size_t size, const char* string)
{
  size_t maxLength = size - 1;
  if (maxLength > 255) {
    maxLength = 255;
  }
  size_t length = strlen(string);
  if (length > maxLength) {
    length = maxLength;
  }
  memcpy(&pString[1], string, length);
  pString[0] = static_cast<unsigned char>(length);
}

void copyPToCString(char* string, size_t size, const unsigned char* pString)
{
  size_t maxLength = size - 1;
  size_t length = pString[0];
  if (length > maxLength) {
    length = maxLength;
  }
  memcpy(string, &pString[1], length);
  string[length] = 0;
}

char* saveString(const char* string)
{
  TRACE("saveString");

  PRECONDITION("Valid string", validString(string));
  size_t length = strlen(string) + 1;
  char* result = new char[length];
  ASSERT("Valid heap pointer", result != 0);
  strcpy(result, string);
  return result;
}

size_t stringSize(OMUInt32 i)
{
  TRACE("stringSize");

  PRECONDITION("Valid integer", i > 0);

  size_t result = 8;
  while (((i & 0xf0000000) >> 28) == 0) {
    ASSERT("Valid result", result != 0);
    result = result - 1;
    i = i << 4;
  }
  return result;
}

void toWideString(OMUInt32 i, wchar_t* result, size_t ANAME(resultSize))
{
  TRACE("toWideString");

  PRECONDITION("Valid integer", i > 0);
  PRECONDITION("Valid result buffer", result != 0);
  PRECONDITION("Output string large enough", resultSize >= stringSize(i));

  // Skip leading zeros
  //
  size_t digits = 8;
  int nibble;
  while ((nibble = ((i & 0xf0000000) >> 28)) == 0) {
    ASSERT("Valid digits", digits > 0);
    digits = digits - 1;
    i = i << 4;
  }

  // Convert remaining digits to hex characters
  //
  const wchar_t* hexDigits = L"0123456789abcdef";
  wchar_t* p = result;
  while (digits > 0) {
    ASSERT("Valid nibble", ((nibble >= 0) && (nibble <= 15)));
    *p++ = hexDigits[nibble];
    i = i << 4;
    nibble = ((i & 0xf0000000) >> 28);
    ASSERT("Valid digits", digits > 0);
    digits = digits - 1;
  }
  *p = 0;
}

size_t squeezeWideString(const wchar_t* clearName,
                         size_t clearNameSize,
                         wchar_t* squeezedName,
                         size_t squeezedNameSize)
{
  TRACE("squeezeWideString");

  PRECONDITION("Valid input name", validWideString(clearName));
  PRECONDITION("Valid input name size", clearNameSize > 0);
  PRECONDITION("Valid result buffer", squeezedName != 0);
  PRECONDITION("Valid result buffer size", squeezedNameSize > 0);

  size_t result;
  if (clearNameSize <= squeezedNameSize) {
    // It fits, just copy
    copyWideString(squeezedName, clearName, clearNameSize + 1);
    result = clearNameSize;
  } else {
    // Too big, squeeze it
    wchar_t ch;
    size_t half = squeezedNameSize / 2;
    for (size_t i = 0; i < squeezedNameSize; i++) {
      if (i < half) {         // Copy from bottom half
        ch = clearName[i];
      } else if (i == half) { // Show characters omitted
        ch = L'-';
      } else {                // Copy from top half
        ch = clearName[clearNameSize - (squeezedNameSize - i)];
      }
      ASSERT("Valid character", ch != 0);
      squeezedName[i] = ch;
    }
    squeezedName[squeezedNameSize] = 0;
    result = squeezedNameSize;
  }
  return result;
}

bool validString(const char* string)
{
  bool result = true;

  if (string == 0) {
    // bad pointer
    result = false;
  } else {
    size_t length = strlen(string);
    if (length == 0) {
      // bad length
      result = false;
    } else {
      for (size_t i = 0; i < length; i++) {
        int c = (unsigned char)string[i];
        if (!isprint(c)) {
          // bad character
          result = false;
          break;
        }
      }
    }
  }

  return result;
}

bool validWideString(const wchar_t* string)
{
  return (string != 0) /* && (lengthOfWideString(string) > 0) */;
}

static const char digits[] = "0123456789ABCDEF";

void toString(const OMUInt8& i, char* is)
{
  TRACE("toString");
  PRECONDITION("Valid buffer", is != 0);

  char* op = is;
  *op++ = digits[ (i & 0xf0) >> 4 ];
  *op++ = digits[ (i & 0x0f) ];
  *op++ = '\0';
}

void toString(const OMUInt16& i, char* is)
{
  TRACE("toString");
  PRECONDITION("Valid buffer", is != 0);

  const OMUInt16 mask1 = 0xff;
  const OMUInt16 mask0 = mask1 << 8;
  char* op = is;
  OMUInt8 b0 = (OMUInt8)((i & mask0) >> 8);
  OMUInt8 b1 = (OMUInt8)((i & mask1));
  toString(b0, op); op = op + 2;
  toString(b1, op); op = op + 2;
}

void toString(const OMUInt32& i, char* is)
{
  TRACE("toString");
  PRECONDITION("Valid buffer", is != 0);

  const OMUInt32 mask1 = 0xffff;
  const OMUInt32 mask0 = mask1 << 16;
  char* op = is;
  OMUInt16 w0 = (OMUInt16)((i & mask0) >> 16);
  OMUInt16 w1 = (OMUInt16)((i & mask1));
  toString(w0, op); op = op + 4;
  toString(w1, op); op = op + 4;
}

void toString(const OMUInt64& i, char* is)
{
  TRACE("toString");
  PRECONDITION("Valid buffer", is != 0);

  const OMUInt64 mask1 = 0xffffffff;
  const OMUInt64 mask0 = mask1 << 32;
  char* op = is;
  OMUInt32 l0 = (OMUInt32)((i & mask0) >> 32);
  OMUInt32 l1 = (OMUInt32)((i & mask1));
  toString(l0, op); op = op + 8;
  toString(l1, op); op = op + 8;
}

void toString(const OMObjectIdentification& id, char* idString)
{
  TRACE("toString");
  PRECONDITION("Valid buffer", idString != 0);

  char* op = idString;

  *op++ = '{';

  OMUInt32* lp = (OMUInt32 *)&id;

  // long word
  toString(*lp++, op); op = op + 8;

  OMUInt16* wp = (OMUInt16 *)lp;

  // word
  *op++ = '-';
  toString(*wp++, op); op = op + 4;
  // word
  *op++ = '-';
  toString(*wp++, op); op = op + 4;

  OMByte* bp = (OMByte *)wp;

  // pseudo-word
  *op++ = '-';
  toString(*bp++, op); op = op + 2;
  toString(*bp++, op); op = op + 2;

  // bytes
  *op++ = '-';
  toString(*bp++, op); op = op + 2;
  toString(*bp++, op); op = op + 2;
  toString(*bp++, op); op = op + 2;
  toString(*bp++, op); op = op + 2;
  toString(*bp++, op); op = op + 2;
  toString(*bp++, op); op = op + 2;

  *op++ = '}';
  *op = '\0';
}

void toString(const OMKLVKey& key, char* keyString)
{
  TRACE("toString");
  PRECONDITION("Valid buffer", keyString != 0);

  char* op = keyString;
  OMUInt8* bp = (OMUInt8*)&key;
  for (size_t i = 0; i < sizeof(OMKLVKey); i++) {
    toString(*bp++, op); op = op + 2;
    if (i < sizeof(OMKLVKey) - 1) {
      *op = '.'; op = op + 1;
    }
  }
  *op = '\0';
}

void fromString(OMUInt8& i, const char* is)
{
  TRACE("fromString");
  PRECONDITION("Valid string", is != 0);
  PRECONDITION("Valid string", strlen(is) >= (OMUInt8StringBufferSize - 1));
  OMByte b = *is++;
  b = b - 0x30;
  if (b > 9) b = b - 7;
  i = b;
  i = i << 4;
  b = *is;
  b = b - 0x30;
  if (b > 9) b = b - 7;
  i = i + b;
}

void fromString(OMUInt16& i, const char* is)
{
  TRACE("fromString");
  PRECONDITION("Valid string", is != 0);
  PRECONDITION("Valid string", strlen(is) >= (OMUInt16StringBufferSize - 1));
  const char* p = is;
  OMUInt8 b;
  fromString(b, p); p = p + 2;
  i = b;
  i = i << 8;
  fromString(b, p);
  i = i + b;
}

void fromString(OMUInt32& i, const char* is)
{
  TRACE("fromString");
  PRECONDITION("Valid string", is != 0);
  PRECONDITION("Valid string", strlen(is) >= (OMUInt32StringBufferSize - 1));
  const char* p = is;
  OMUInt16 w;
  fromString(w, p); p = p + 4;
  i = w;
  i = i << 16;
  fromString(w, p);
  i = i + w;
}

void fromString(OMUInt64& i, const char* is)
{
  TRACE("fromString");
  PRECONDITION("Valid string", is != 0);
  PRECONDITION("Valid string", strlen(is) >= (OMUInt64StringBufferSize - 1));
  const char* p = is;
  OMUInt32 l;
  fromString(l, p); p = p + 8;
  i = l;
  i = i << 32;
  fromString(l, p);
  i = i + l;
}

void fromString(OMObjectIdentification& id, const char* idString)
{
  TRACE("fromString");
  PRECONDITION("Valid id string", isValidObjectIdentificationString(idString));

  const char* p = idString;
  ++p; // {
  // long word
  fromString(id.Data1, p); p = p + 8;
  ++p; // -
  // word
  fromString(id.Data2, p); p = p + 4;
  ++p; // -
  // word
  fromString(id.Data3, p); p = p + 4;
  ++p; // -
  // pseudo-word
  OMUInt16 x;
  fromString(x, p); p = p + 4;
  id.Data4[0] = (OMUInt8)((x & 0xff00) >> 8);
  id.Data4[1] = (OMUInt8)(x & 0x00ff);
  ++p; // -
  // bytes
  fromString(id.Data4[2], p); p = p + 2;
  fromString(id.Data4[3], p); p = p + 2;
  fromString(id.Data4[4], p); p = p + 2;
  fromString(id.Data4[5], p); p = p + 2;
  fromString(id.Data4[6], p); p = p + 2;
  fromString(id.Data4[7], p); p = p + 2;
  ++p; // }
  POSTCONDITION("End of string", *p == 0);
}

bool isValidObjectIdentificationString(const char* idString)
{
  TRACE("isValidObjectIdentificationString");
  PRECONDITION("Non-void string", idString != 0);

  bool result = true;
  if (!validString(idString)) {
    result = false;
  } else if (strlen(idString) != OMObjectIdentificationStringBufferSize - 1) {
    result = false;
  } else {
    const char* pattern = "{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}";
    ASSERT("Valid pattern length", strlen(pattern) == strlen(idString));

    const char* idChar = idString;
    const char* patternChar = pattern;
    while (*patternChar != 0) {
      if (*patternChar == 'x') {
        if (!isxdigit(*idChar)) {
          result = false;
          break;
        }
      } else {
        if (*idChar != *patternChar) {
          result = false;
          break;
        }
      }
      ++idChar;
      ++patternChar;
    }
  }

  return result;
}

void fromString(OMKLVKey& /* key */, const char* /* keyString */)
{
  TRACE("fromString");
  ASSERT("Unimplemented code not reached", false); // tjb TBS
}


bool isValidKLVKeyString(const char* /* keyString */)
{
  TRACE("isValidKLVKeyString(");
  ASSERT("Unimplemented code not reached", false); // tjb TBS
  return false;
}

void convert(OMKLVKey& key, const OMUniqueObjectIdentification& id)
{
  TRACE("convert");

  // Bottom half of key <- top half of id
  //
  key.octet0  = id.Data4[0];
  key.octet1  = id.Data4[1];
  key.octet2  = id.Data4[2];
  key.octet3  = id.Data4[3];
  key.octet4  = id.Data4[4];
  key.octet5  = id.Data4[5];
  key.octet6  = id.Data4[6];
  key.octet7  = id.Data4[7];

  // Top half of key <- bottom half of id
  //
  key.octet8  = (OMByte)((id.Data1 & 0xff000000) >> 24);
  key.octet9  = (OMByte)((id.Data1 & 0x00ff0000) >> 16);
  key.octet10 = (OMByte)((id.Data1 & 0x0000ff00) >>  8);
  key.octet11 = (OMByte)((id.Data1 & 0x000000ff));

  key.octet12 = (OMByte)((id.Data2 & 0xff00) >> 8);
  key.octet13 = (OMByte)((id.Data2 & 0x00ff));

  key.octet14 = (OMByte)((id.Data3 & 0xff00) >> 8);
  key.octet15 = (OMByte)((id.Data3 & 0x00ff));

  // If id is an AAF class AUID, map it to a SMPTE 336M local set key
  //
  OMByte classIdPrefix[] = {0x06, 0x0e, 0x2b, 0x34, 0x02, 0x06};
  if (memcmp(&key, &classIdPrefix, sizeof(classIdPrefix)) == 0) {
    key.octet5  = 0x53;
  }
}

void convert(OMUniqueObjectIdentification& id, const OMKLVKey& key)
{
  TRACE("convert");

  // Bottom half of id <- top half of key
  //
  id.Data4[0] = key.octet0;
  id.Data4[1] = key.octet1;
  id.Data4[2] = key.octet2;
  id.Data4[3] = key.octet3;
  id.Data4[4] = key.octet4;
  id.Data4[5] = key.octet5;
  id.Data4[6] = key.octet6;
  id.Data4[7] = key.octet7;

  // Top half of id <- bottom half of key
  //
  OMUInt32 d1 = 0;
  d1 = d1 + (key.octet8  << 24);
  d1 = d1 + (key.octet9  << 16);
  d1 = d1 + (key.octet10 <<  8);
  d1 = d1 + (key.octet11);
  id.Data1 = d1;

  OMUInt16 d2 = 0;
  d2 = d2 + (key.octet12 << 8);
  d2 = d2 + (key.octet13);
  id.Data2 = d2;

  OMUInt16 d3 = 0;
  d3 = d3 + (key.octet14 << 8);
  d3 = d3 + (key.octet15);
  id.Data3 = d3;

  // If key is a SMPTE 336M local set key, map it to an AAF class AUID
  //
  OMByte localSetPrefix[] = {0x06, 0x0e, 0x2b, 0x34, 0x02, 0x53};
  if (memcmp(&key, &localSetPrefix, sizeof(localSetPrefix)) == 0) {
    id.Data4[5] = 0x06;
  }
}

void checkTypes(void)
{
  TRACE("checkTypes");

  ASSERT("Correct size for OMInt8",   sizeof(OMInt8)   == 1);
  ASSERT("Correct size for OMInt16",  sizeof(OMInt16)  == 2);
  ASSERT("Correct size for OMInt32",  sizeof(OMInt32)  == 4);
  ASSERT("Correct size for OMInt64",  sizeof(OMInt64)  == 8);

  ASSERT("Correct size for OMUInt8",  sizeof(OMUInt8)  == 1);
  ASSERT("Correct size for OMUInt16", sizeof(OMUInt16) == 2);
  ASSERT("Correct size for OMUInt32", sizeof(OMUInt32) == 4);
  ASSERT("Correct size for OMUInt64", sizeof(OMUInt64) == 8);

  ASSERT("Correct size for OMObjectIdentification",
                                         sizeof(OMObjectIdentification) == 16);
  ASSERT("Correct size for OMKLVKey", sizeof(OMKLVKey) == 16);

  ASSERT("Correct value for OMINT8_MIN",
                            OMINT8_MIN  == (-OMINT8_MAX  - 1));
  ASSERT("Correct value for OMINT8_MAX",
                            OMINT8_MAX  == (OMInt8) ~((OMInt8) 1 << ( 8 - 1)));
  ASSERT("Correct value for OMINT16_MIN",
                            OMINT16_MIN == (-OMINT16_MAX - 1));
  ASSERT("Correct value for OMINT16_MAX",
                            OMINT16_MAX == (OMInt16)~((OMInt16)1 << (16 - 1)));
  ASSERT("Correct value for OMINT32_MIN",
                            OMINT32_MIN == (-OMINT32_MAX - 1));
  ASSERT("Correct value for OMINT32_MAX",
                            OMINT32_MAX == (OMInt32)~((OMInt32)1 << (32 - 1)));
  ASSERT("Correct value for OMINT64_MIN",
                            OMINT64_MIN == (-OMINT64_MAX - 1));
  ASSERT("Correct value for OMINT64_MAX",
                            OMINT64_MAX == (OMInt64)~((OMInt64)1 << (64 - 1)));

  ASSERT("Correct value for OMUINT8_MAX",  OMUINT8_MAX   == (OMUInt8) ~0);
  ASSERT("Correct value for OMUINT16_MAX", OMUINT16_MAX  == (OMUInt16)~0);
  ASSERT("Correct value for OMUINT32_MAX", OMUINT32_MAX  == (OMUInt32)~0);
  ASSERT("Correct value for OMUINT64_MAX", OMUINT64_MAX  == (OMUInt64)~0);
}

  // Manipulation of property paths (eventually these will be
  // intantiations of templates shared with the wide character
  // functions).

bool validPropertyPath(const OMPropertyId* path)
{
  TRACE("validPropertyPath");

  bool result;
  if (path == 0) {
    result = false;
  } else {
    if (lengthOfPropertyPath(path) == 0) {
      result = false;
    } else {
      result = true;
    }
  }
  return result;
}

OMUInt32 lengthOfPropertyPath(const OMPropertyId* path)
{
  TRACE("lengthOfPropertyPath");
  // PRECONDITION("Valid property path", validPropertyPath(path));
  PRECONDITION("Valid property path", path != 0);

  const OMPropertyId* p = path;
  OMUInt32 result = 0;
  while (*p != 0) {
    ++result;
    ++p;
  }
  return result;
}

OMPropertyId* savePropertyPath(const OMPropertyId* path)
{
  TRACE("savePropertyPath");
  PRECONDITION("Valid property path", validPropertyPath(path));

  size_t length = lengthOfPropertyPath(path) + 1;
  OMPropertyId* result = new OMPropertyId[length];
  ASSERT("Valid heap pointer", result != 0);
  for (size_t i = 0; i < length; i++) {
    result[i] = path[i];
  }

  return result;
}

int comparePropertyPath(const OMPropertyId* path1, const OMPropertyId* path2)
{
  TRACE("comparePropertyPath");

  PRECONDITION("Valid property path", validPropertyPath(path1));
  PRECONDITION("Valid property path", validPropertyPath(path2));
  int result = 0;
  const OMPropertyId* p1 = path1;
  const OMPropertyId* p2 = path2;
  while (*p1 == *p2) {
    if (*p1 == 0) {
      break;
    }
    ++p1;
    ++p2;
  }

  if ((*p1 == 0) && (*p2 == 0)) {
    result = 0;
  } else if (*p1 > *p2) {
    result = 1;
  } else if (*p1 < *p2) {
    result = -1;
  }
  return result;
}

#if defined(OM_OS_WINDOWS)

#include <windows.h>

typedef enum WindowsKind {
  wkError,       // error/unknown
  wk3_1,         // Win32s
  wkConsumer,    // Windows 95/98/Me
  wkProfessional // Windows NT/2000/XP
} WindowsKind;

static WindowsKind getWindowsKind(void)
{
  WindowsKind result = wkError;

  DWORD version = GetVersion();
  BYTE majorVersion = (BYTE)(version        & 0x000000ff);
  // If minorVersion is needed use:
  // minorVersion = (BYTE)((version >> 8) & 0x000000ff);

  if (version < 0x80000000) {
    result = wkProfessional;
  } else if (majorVersion < 4) {
    result = wk3_1;
  } else {
    result = wkConsumer;
  }
  return result;
}

#endif

// _wfopen() and _wremove() are in the W32 API on Windows 95, 98 and
// ME but with an implementation that always fails. So we only call
// them if getWindowsKind() == wkProfessional.

// Just like ISO fopen() except for wchar_t* file names and modes.
//
FILE* wfopen(const wchar_t* fileName, const wchar_t* mode)
{
  TRACE("wfopen");
  ASSERT("Valid file name", fileName != 0);
  ASSERT("Valid mode", mode != 0);

  FILE* result = 0;
#if defined(OM_OS_WINDOWS)
  if (getWindowsKind() == wkProfessional) {
    result = _wfopen(fileName, mode);
  } else {
#endif
    char cFileName[FILENAME_MAX + 1];
    size_t status = wcstou8s(cFileName, fileName, FILENAME_MAX + 1);
    ASSERT("Convert succeeded", status != (size_t)-1);
    ASSERT("Convert succeeded", status != FILENAME_MAX + 1);

    char cMode[FILENAME_MAX + 1];
    status = wcstou8s(cMode, mode, FILENAME_MAX + 1);
    ASSERT("Convert succeeded", status != (size_t)-1);
    ASSERT("Convert succeeded", status != FILENAME_MAX + 1);

    result = fopen(cFileName, cMode);
#if defined(OM_OS_WINDOWS)
  }
#endif
  return result;
}

// Just like ISO remove() except for wchar_t* file names.
int wremove(const wchar_t* fileName)
{
  TRACE("wremove");
  ASSERT("Valid file name", fileName != 0);

  int result = 0;
#if defined(OM_OS_WINDOWS)
  if (getWindowsKind() == wkProfessional) {
    result = _wremove(fileName);
  } else {
#endif
    char cFileName[FILENAME_MAX];
#if defined(OM_PARAMETER_VALIDATION)
    size_t status =
#endif
    wcstou8s(cFileName, fileName, FILENAME_MAX);
    ASSERT("Convert succeeded", status != (size_t)-1);
    ASSERT("Convert succeeded", status != FILENAME_MAX + 1);

    result = remove(cFileName);
#if defined(OM_OS_WINDOWS)
  }
#endif
  return result;
}

#if defined(OM_OS_WINDOWS)
#define OM_USE_COM_CREATEUUID
#elif defined(OM_OS_UNIX) && defined(__linux__)
#define OM_USE_LIBUUID_CREATEUUID
#elif defined(OM_OS_UNIX) && defined(__FreeBSD__)

/* See FreeBSD Porter's Handbook for how
   to differentiate OS versions. */
#if __FreeBSD__ >= 2
#  include <osreldate.h>
#  if __FreeBSD_version >= 500000
     /* UUID was introduced in the 5.0 release */
#    define OM_USE_LIBC_CREATEUUID 
#  else
#    define OM_USE_OM_CREATEUUID
#  endif
#else
#  define OM_USE_OM_CREATEUUID
#endif

#elif defined(OM_OS_UNIX) && defined(__MACH__)
#define OM_USE_CF_CREATEUUID
#elif defined(OM_OS_UNIX)
#define OM_USE_OM_CREATEUUID
#elif defined(OM_OS_MACOSX)
#define OM_USE_OM_CREATEUUID
#endif

#if defined(OM_USE_COM_CREATEUUID)

#include <objbase.h>

OMUniqueObjectIdentification createUniqueIdentifier(void)
{
  OMUniqueObjectIdentification result = nullOMUniqueObjectIdentification;
  GUID u;
  CoCreateGuid(&u);
  memcpy(&result, &u, sizeof(OMUniqueObjectIdentification));
  return result;
}

#elif defined(OM_USE_OM_CREATEUUID)

#include <time.h>

OMUniqueObjectIdentification createUniqueIdentifier(void)
{
  OMUniqueObjectIdentification result = nullOMUniqueObjectIdentification;
  // {FFFFFFFF-3B78-47ec-98DD-68AC60D2539E}
  static OMUniqueObjectIdentification id =
    {0xffffffff, 0x3b78, 0x47ec,
    {0x98, 0xdd, 0x68, 0xac, 0x60, 0xd2, 0x53, 0x9e}};

  if (id.Data1 == 0xffffffff) {
    OMUInt32 ticks = clock();
    time_t timer = time(0);
    id.Data1 += timer + ticks;
  }
  ++id.Data1;
  result = id;
  return result;
}

#elif defined(OM_USE_CF_CREATEUUID)

#include <CoreServices/CoreServices.h>

OMUniqueObjectIdentification createUniqueIdentifier(void)
{
  OMUniqueObjectIdentification result = nullOMUniqueObjectIdentification;
  CFUUIDRef ur = CFUUIDCreate(kCFAllocatorDefault);
  CFUUIDBytes ub = CFUUIDGetUUIDBytes(ur);
  memcpy(&result, &ub, sizeof(OMUniqueObjectIdentification));
  CFRelease(ur);
  return result;
}

#elif defined(OM_USE_LIBUUID_CREATEUUID)

extern "C" {
#include <uuid/uuid.h>
}

OMUniqueObjectIdentification createUniqueIdentifier(void)
{
  OMUniqueObjectIdentification result = nullOMUniqueObjectIdentification;
  uuid_t u;
  uuid_generate(u);
  memcpy(&result, &u, sizeof(OMUniqueObjectIdentification));
  return result;
}
#elif defined(OM_USE_LIBC_CREATEUUID)

#include <uuid.h>

OMUniqueObjectIdentification createUniqueIdentifier(void)
{
  TRACE("createUniqueIdentifier");

  OMUniqueObjectIdentification result = nullOMUniqueObjectIdentification;
  uint32_t status;
  uuid_t u;
  uuid_create(&u, &status);
  ASSERT("uuid_create() succeeded", status == uuid_s_ok);
  memcpy(&result, &u, sizeof(OMUniqueObjectIdentification));
  return result;
}

#endif

OMUInt64 ioVectorByteCount(const OMIOBufferDescriptor* buffers,
						   const OMUInt32 bufferCount)
{
  TRACE( "ioVectorByteCount" );
  PRECONDITION( "Valid buffers", buffers != 0 );

  OMUInt64 size = 0;
  for( OMUInt32 i=0; i<bufferCount; i++ )
  {
    size = size + buffers[i]._bufferSize;
  }

  return size;
}
