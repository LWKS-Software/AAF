
/******************************************\
*                                          *
* Advanced Authoring Format                *
*                                          *
* Copyright (c) 1998 Avid Technology, Inc. *
* Copyright (c) 1998 Microsoft Corporation *
*                                          *
\******************************************/






#ifndef __ImplAAFIdentification_h__
#include "ImplAAFIdentification.h"
#endif

#include "AAFTypes.h"
#include "AAFResult.h"

#include <assert.h>


ImplAAFIdentification::~ImplAAFIdentification ()
{}

ImplAAFIdentification::ImplAAFIdentification():
_companyName(         PID_IDENTIFICATION_COMPANYNAME,          "companyName"),
_productName(         PID_IDENTIFICATION_PRODUCTNAME,          "productName"),
// _productVersion(   PID_IDENTIFICATION_PRODUCTVERSION,       "productVersion")
_productVersionString(PID_IDENTIFICATION_PRODUCTVERSIONSTRING, "productVersionString"),
// _productId(        PID_IDENTIFICATION_PRODUCTID,            "productId"),
_date(                PID_IDENTIFICATION_DATE,                 "date"),
// _toolKitVersion(   PID_IDENTIFICATION_TOOLKITVERSION,       "toolkitVersion"),
_platform(            PID_IDENTIFICATION_PLATFORM,             "platform")
// _generation(       PID_IDENTIFICATION_GENERATION,           "generation")
{
  // Insert the properties into the persistent property set.
  //
  _persistentProperties.put(   _companyName.address());
  _persistentProperties.put(   _productName.address());
  // _persistentProperties.put(_productVersion.address());
  _persistentProperties.put(   _productVersionString.address());
  // _persistentProperties.put(_productId.address());
  _persistentProperties.put(   _date.address());
  // _persistentProperties.put(_toolKitVersion.address());
  _persistentProperties.put(   _platform.address());
  // _persistentProperties.put(_generation.address());
}

ImplAAFIdentification::ImplAAFIdentification(
                                     const char* companyName,
                                     const char* productName,
                                     const aafProductVersion_t* productVersion,
                                     const char* productVersionString,
                                     // const AUID* productId,
                                     const aafTimeStamp_t date,
                                     const aafProductVersion_t* toolKitVersion,
                                     const char* platform
                                     // const AUID* generation
                                     ):
_companyName(         PID_IDENTIFICATION_COMPANYNAME,          "companyName"),
_productName(         PID_IDENTIFICATION_PRODUCTNAME,          "productName"),
// _productVersion(   PID_IDENTIFICATION_PRODUCTVERSION,       "productVersion")
_productVersionString(PID_IDENTIFICATION_PRODUCTVERSIONSTRING, "productVersionString"),
// _productId(        PID_IDENTIFICATION_PRODUCTID,            "productId"),
_date(                PID_IDENTIFICATION_DATE,                 "date"),
// _toolKitVersion(   PID_IDENTIFICATION_TOOLKITVERSION,       "toolkitVersion"),
_platform(            PID_IDENTIFICATION_PLATFORM,             "platform")
// _generation(       PID_IDENTIFICATION_GENERATION,           "generation")
{
  TRACE("AAFIdentification::AAFIdentification");
  PRECONDITION("Valid companyName",          validString(companyName));
  PRECONDITION("Valid productName",          validString(productName));
  PRECONDITION("Valid productVersionString", validString(productVersionString));
  PRECONDITION("Valid platform",             validString(platform));

  // Insert the properties into the persistent property set.
  //
  _persistentProperties.put(   _companyName.address());
  _persistentProperties.put(   _productName.address());
  // _persistentProperties.put(_productVersion.address());
  _persistentProperties.put(   _productVersionString.address());
  // _persistentProperties.put(_productId.address());
  _persistentProperties.put(   _date.address());
  // _persistentProperties.put(_toolKitVersion.address());
  _persistentProperties.put(   _platform.address());
  // _persistentProperties.put(_generation.address());

  // Initialize members.
  //
  _companyName = companyName;
  _productName = productName;
  // _productVersion.???;
  _productVersionString = productVersionString;
  // _productId.???;
  _date = date;
  // _toolKitVersion.???;
  _platform = platform;
  // _generation.???;

};


extern "C" const aafClassID_t CLSID_AAFIdentification;

OMDEFINE_STORABLE(ImplAAFIdentification, CLSID_AAFIdentification);

/* returns true if string fits into buffer and does the conversion.
   Returns false otherwise. */
static bool stringPropertyToWCharString(aafWChar *pWString,
										aafInt32 buflen,
										OMStringProperty& stringProperty)
{
  const char* string = stringProperty;
  int slen = stringProperty.length();
  if (buflen < slen)
	{
	  return false;
	}
  mbstowcs(pWString, string, slen);
  pWString[slen] = L'\0';
  return true;
}

AAFRESULT STDMETHODCALLTYPE
    ImplAAFIdentification::GetCompanyName (aafWChar *  pName,
										   aafInt32 bufSize)
{
  bool stat;
  if (! pName)
	{
	  return AAFRESULT_NULL_PARAM;
	}
  stat = stringPropertyToWCharString(pName, bufSize, _companyName);
  if (! stat)
	{
	  return AAFRESULT_SMALLBUF;
	}
  return AAFRESULT_SUCCESS;
}


AAFRESULT STDMETHODCALLTYPE
    ImplAAFIdentification::GetCompanyNameBufLen (aafInt32 *  pLen)
{
  if (! pLen)
	{
	  return AAFRESULT_NULL_PARAM;
	}
  *pLen = _companyName.length();
  return AAFRESULT_SUCCESS;
}



AAFRESULT STDMETHODCALLTYPE
    ImplAAFIdentification::GetProductName (aafWChar *  pProductName,
										   aafInt32 bufSize)
{
  bool stat;
  if (! pProductName)
	{
	  return AAFRESULT_NULL_PARAM;
	}
  stat = stringPropertyToWCharString(pProductName, bufSize, _productName);
  if (! stat)
	{
	  return AAFRESULT_SMALLBUF;
	}
  return AAFRESULT_SUCCESS;
}


AAFRESULT STDMETHODCALLTYPE
    ImplAAFIdentification::GetProductNameBufLen (aafInt32 *  pLen)
{
  if (! pLen)
	{
	  return AAFRESULT_NULL_PARAM;
	}
  *pLen = _productName.length();
  return AAFRESULT_SUCCESS;
}



AAFRESULT STDMETHODCALLTYPE
    ImplAAFIdentification::GetProductVersionString (aafWChar *  pVS,
													aafInt32 bufSize)
{
  bool stat;
  if (! pVS)
	{
	  return AAFRESULT_NULL_PARAM;
	}
  stat = stringPropertyToWCharString(pVS, bufSize, _productVersionString);
  if (! stat)
	{
	  return AAFRESULT_SMALLBUF;
	}
  return AAFRESULT_SUCCESS;
}


AAFRESULT STDMETHODCALLTYPE
    ImplAAFIdentification::GetProductVersionStringBufLen (aafInt32 *  pLen)
{
  if (! pLen)
	{
	  return AAFRESULT_NULL_PARAM;
	}
  *pLen = _productVersionString.length();
  return AAFRESULT_SUCCESS;
}



AAFRESULT STDMETHODCALLTYPE
    ImplAAFIdentification::GetProductID (aafUID_t * pPID)
{
  if (! pPID)
	{
	  return AAFRESULT_NULL_PARAM;
	}
  return AAFRESULT_NOT_IMPLEMENTED;
}


AAFRESULT STDMETHODCALLTYPE
    ImplAAFIdentification::GetDate (aafTimeStamp_t * pTS)
{
  if (! pTS)
	{
	  return AAFRESULT_NULL_PARAM;
	}
  return AAFRESULT_NOT_IMPLEMENTED;
}


AAFRESULT STDMETHODCALLTYPE
    ImplAAFIdentification::GetRefImplVersion (aafProductVersion_t * pV)
{
  if (! pV)
	{
	  return AAFRESULT_NULL_PARAM;
	}
  return AAFRESULT_NOT_IMPLEMENTED;
}


AAFRESULT STDMETHODCALLTYPE
    ImplAAFIdentification::GetPlatform (aafWChar *  pPlatform,
										aafInt32 bufSize)
{
  bool stat;
  if (! pPlatform)
	{
	  return AAFRESULT_NULL_PARAM;
	}
  stat = stringPropertyToWCharString(pPlatform, bufSize, _platform);
  if (! stat)
	{
	  return AAFRESULT_SMALLBUF;
	}
  return AAFRESULT_SUCCESS;
}


AAFRESULT STDMETHODCALLTYPE
    ImplAAFIdentification::GetPlatformBufLen (aafInt32 *  pLen)
{
  if (! pLen)
	{
	  return AAFRESULT_NULL_PARAM;
	}
  *pLen = _platform.length();
  return AAFRESULT_SUCCESS;
}


AAFRESULT STDMETHODCALLTYPE
    ImplAAFIdentification::GetGeneration (aafUID_t *  pGen)
{
  if (! pGen)
	{
	  return AAFRESULT_NULL_PARAM;
	}
  return AAFRESULT_NOT_IMPLEMENTED;
}


AAFRESULT STDMETHODCALLTYPE
    ImplAAFIdentification::SetCompanyName (aafWChar *  pName)
{
  if (! pName)
	{
	  return AAFRESULT_NULL_PARAM;
	}
  return AAFRESULT_NOT_IMPLEMENTED;
}


AAFRESULT STDMETHODCALLTYPE
    ImplAAFIdentification::SetProductName (aafWChar *  pName)
{
  if (! pName)
	{
	  return AAFRESULT_NULL_PARAM;
	}
  return AAFRESULT_NOT_IMPLEMENTED;
}


AAFRESULT STDMETHODCALLTYPE
    ImplAAFIdentification::SetProductVersion (aafProductVersion_t * pV)
{
  if (! pV)
	{
	  return AAFRESULT_NULL_PARAM;
	}
  return AAFRESULT_NOT_IMPLEMENTED;
}


AAFRESULT STDMETHODCALLTYPE
    ImplAAFIdentification::SetProductVersionString (aafWChar * pVS)
{
  if (! pVS)
	{
	  return AAFRESULT_NULL_PARAM;
	}
  return AAFRESULT_NOT_IMPLEMENTED;
}


AAFRESULT STDMETHODCALLTYPE
    ImplAAFIdentification::SetProductID (aafUID_t * pPID)
{
  if (! pPID)
	{
	  return AAFRESULT_NULL_PARAM;
	}
  return AAFRESULT_NOT_IMPLEMENTED;
}
