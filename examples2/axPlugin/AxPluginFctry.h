#ifndef __AxPluginFctry_h__
#define __AxPluginFctry_h__

//=---------------------------------------------------------------------=
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
// The Original Code of this file is Copyright 1998-2001, Licensor of the
// AAF Association.
// 
//=---------------------------------------------------------------------=

#include "AxPluginFctryPrtcl.h"
#include "AxPluginUtil.h"

#if defined(OS_WINDOWS)
// Disable warning "int' : forcing value to bool 'true' or 'false' (performance warning)"
// generated by "(clsid == _clsid)".
#pragma warning(disable:4800)
#endif

//
// This is an implementation of AxPluginFctryPrtcl that can by
// used with (i.e., registered with) AxPluginRegistry.
//
// It is used by AxPluginRegistry to create an instance of a COM component's
// factory object in response to a call the library's DllGetClassObject()
// entry point.
//
// It is intended that the template be instandiated on the underlying COM
// object.  Note, that is the underlying COM object, *not* the underlying
// COM object's factory.
//
// An instance is CAxClassFactory<ComObjectType> is created by the
// CreateClassObject() method.  Hence, wherever the template is instantiated,
// CAxClassFactory.h must be included ahead of this header file.
//
//
// Please refer the UML.

template <class ComObjectType>
class AxPluginFctry : public AxPluginFctryPrtcl
{
public:

	AxPluginFctry( const CLSID& clsid )
		: _clsid(clsid)
	{}

	virtual ~AxPluginFctry()
	{}

	virtual CLSID GetClassID()
	{
		return _clsid;
	}

	virtual bool IsClassId( const CLSID& clsid )
	{
		return (clsid == _clsid);
	}

	virtual HRESULT CreateClassObject( IUnknown** ppv )
	{
		CAxClassFactory<ComObjectType>* factory = new CAxClassFactory<ComObjectType>;

		if ( !factory ) {
			return E_OUTOFMEMORY;
		}

		*ppv = static_cast<IUnknown*>( factory );

		return S_OK;
	}

private:
	CLSID _clsid;
};

#endif
