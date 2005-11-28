#ifndef __AxKLV_h__
#define __AxKLV_h__

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
// The Original Code of this file is Copyright 1998-2004, Licensor of the
// AAF Association.
//
//=---------------------------------------------------------------------=


#include "AxTypes.h"
#include "AxSmartPointer.h"
#include "AxObject.h"

//=---------------------------------------------------------------------=

class AxTaggedValue : public AxObject {

public:
	AxTaggedValue( IAAFTaggedValueSP spIaafTaggedValue );
	virtual ~AxTaggedValue();

    void Initialize( AxString name, IAAFTypeDefSP typeDef, aafUInt32 valueSize, aafDataBuffer_t pValue );
    AxString GetName();
    IAAFTypeDefSP GetTypeDefinition();
    void GetValue( aafUInt32 valueSize, aafDataBuffer_t pValue, aafUInt32* bytesRead);
    void SetValue ( aafUInt32 valueSize, aafDataBuffer_t pValue );

    template <typename ParamType>
    void GetValue( ParamType& val )
    {
        ParamType tmp;
        aafUInt32 bytesRead;
        GetValue( sizeof(tmp), reinterpret_cast<aafDataBuffer_t>(&tmp), &bytesRead );
        if ( sizeof(tmp) == bytesRead ) {
            val = tmp;
        }
        else {
            throw AxEx( L"size mismatch in AxTaggedValue::GetValue()" );      
        }
      }
      
    template <typename ParamType>
    void SetValue( ParamType val )
    {
        ParamType tmp;
        SetValue( sizeof(tmp), reinterpret_cast<aafDataBuffer_t>(&tmp) );
    }

	operator IAAFTaggedValueSP ()
	{ return _spIaafTaggedValue; }

private:
	AxTaggedValue();
	AxTaggedValue( const AxTaggedValue& );
	AxTaggedValue& operator=( const AxTaggedValue& );

	IAAFTaggedValueSP _spIaafTaggedValue;
};




#endif