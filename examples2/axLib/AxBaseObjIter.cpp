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
//=---------------------------------------------------------------------=

#include "AxBaseObjIter.h"

#include "AxObject.h"
#include "AxProperty.h"
#include "AxPropertyValue.h"
#include "AxMetaDef.h"
#include "AxIterator.h"

//=---------------------------------------------------------------------=

namespace {

class PropValToIter : public AxPropertyValueNoopPrtcl {
public:

	PropValToIter();

	virtual ~PropValToIter();

	virtual void process( IAAFPropertyValueSP&, IAAFTypeDefStrongObjRefSP& );

	virtual void process( IAAFPropertyValueSP&, IAAFTypeDefSetSP& );

	virtual void process( IAAFPropertyValueSP&, IAAFTypeDefVariableArraySP& );

	virtual void process( IAAFPropertyValueSP&, IAAFTypeDefRecordSP& );

	virtual void process( IAAFPropertyValueSP&, IAAFTypeDefFixedArraySP& );

	virtual void process( IAAFPropertyValueSP&, IAAFTypeDefIndirectSP& );

	virtual void process( IAAFPropertyValueSP&, IAAFTypeDefOpaqueSP& );

	virtual void process( IAAFPropertyValueSP&, IAAFTypeDefRenameSP& );

	bool ResultExists();

	std::shared_ptr<AxBaseObjIterPrtcl> GetResult();

private:

	inline void Post( std::shared_ptr<AxBaseObjIterPrtcl> iter ) {
		_isSet = true;
		_result= iter;
	}
	
	bool _isSet;
	std::shared_ptr<AxBaseObjIterPrtcl> _result;
};

PropValToIter::PropValToIter()
:	_isSet( false )
{}

PropValToIter::~PropValToIter()
{}

bool PropValToIter::ResultExists()
{
	return _isSet;
}

std::shared_ptr<AxBaseObjIterPrtcl> PropValToIter::GetResult()
{
	return _result;
}

void PropValToIter::process( IAAFPropertyValueSP& spIaafPropertyValue,
			     IAAFTypeDefStrongObjRefSP& spIaafTypeDefStrongObjRef )
{
	AxTypeDefStrongObjRef axStrongObjRef( spIaafTypeDefStrongObjRef );

	IUnknownSP spIUnknown;
	spIUnknown = axStrongObjRef.GetObject( spIaafPropertyValue, IID_IAAFObject );

	IAAFObjectSP spIaafObject;
	AxQueryInterface( spIUnknown, spIaafObject );

	AxObject axObj( spIaafObject );
	std::shared_ptr< AxBaseObjIterPrtcl > iter(
		new AxBaseSolitaryObjIter<AxObject>( spIaafObject ) );

	Post( iter );
}

void PropValToIter::process( IAAFPropertyValueSP& spIaafPropertyValue,
			     IAAFTypeDefSetSP& spIaafTypeDefSet )
{
	AxTypeDefSet axTypeDefSet( spIaafTypeDefSet );

	AxPropertyValueIter axPropValIter( axTypeDefSet.GetElements( spIaafPropertyValue ) );

	std::shared_ptr< AxBaseObjIterPrtcl > iter( 
		new AxBaseObjIter<AxPropertyValueIter, AxPropertyValue, IAAFPropertyValue>( axPropValIter ) );

	Post( iter );
}

void PropValToIter::process( IAAFPropertyValueSP& spIaafPropertyValue,
			     IAAFTypeDefVariableArraySP& spIaafTypeDefVariableArray )
{
	AxTypeDefVariableArray axTypeDefVarArray( spIaafTypeDefVariableArray );

	AxPropertyValueIter axPropValIter( axTypeDefVarArray.GetElements( spIaafPropertyValue ) );

	std::shared_ptr< AxBaseObjIterPrtcl > iter( 
		new AxBaseObjIter<AxPropertyValueIter, AxPropertyValue, IAAFPropertyValue>( axPropValIter ) );

	Post( iter );
}

void PropValToIter::process( IAAFPropertyValueSP& spIaafPropertyValue,
			     IAAFTypeDefRecordSP& spIaafTypeDefRecord )
{
	// Records are a bit weird, the are a set of named attribute/value
	// pairs, but are not IAAFObjects which are also a set of named
	// attribute/value (i.e. property/value) pairs.  In order to 
	// process the individual record fields using this framework,
	// a special iterator is required.

	std::shared_ptr<AxRecordIterator> axRecordIter( 
		new AxRecordIterator( spIaafPropertyValue, spIaafTypeDefRecord ) );

	std::shared_ptr<AxBaseObjIterPrtcl> iter(
		new AxBaseRecordObjIter( axRecordIter ) );

	Post( iter );
}

void PropValToIter::process( IAAFPropertyValueSP& spIaafPropertyValue,
			     IAAFTypeDefFixedArraySP& spIaafTypeDefFixedArray )
{
	AxTypeDefFixedArray axTypeDefFixedArray( spIaafTypeDefFixedArray );

	AxPropertyValueIter axPropValIter( axTypeDefFixedArray.GetElements( spIaafPropertyValue ) );

	std::shared_ptr< AxBaseObjIterPrtcl > iter( 
		new AxBaseObjIter<AxPropertyValueIter, AxPropertyValue, IAAFPropertyValue>( axPropValIter ) );

	Post( iter );
}

void PropValToIter::process( IAAFPropertyValueSP& spIaafPropertyValue,
			     IAAFTypeDefIndirectSP& spIaafTypeDefIndirect )
{
	AxTypeDefIndirect axIndirect( spIaafTypeDefIndirect );

	AxPropertyValue axActualPropVal( axIndirect.GetActualValue(spIaafPropertyValue) );

	std::shared_ptr< AxBaseObjIterPrtcl > iter(
		new AxBaseSolitaryObjIter<AxPropertyValue>( axActualPropVal ) );

	Post( iter );
}

void PropValToIter::process( IAAFPropertyValueSP& spIaafPropertyValue,
			     IAAFTypeDefOpaqueSP& spIaafTypeDefOpaque )
{
	AxTypeDefOpaque axOpaque( spIaafTypeDefOpaque );
	
	AxPropertyValue axActualPropVal( axOpaque.GetActualValue( spIaafPropertyValue ) );

	std::shared_ptr< AxBaseObjIterPrtcl > iter(
		new AxBaseSolitaryObjIter<AxPropertyValue>( axActualPropVal ) );

	Post( iter );
}

void PropValToIter::process( IAAFPropertyValueSP& spIaafPropertyValue,
			     IAAFTypeDefRenameSP& spIaafTypeDefRename )
{
	AxTypeDefRename axTypeDefRename( spIaafTypeDefRename );
	AxPropertyValue axBasePropVal( axTypeDefRename.GetBaseValue( spIaafPropertyValue ) );

	std::shared_ptr< AxBaseObjIterPrtcl > iter(
		new AxBaseSolitaryObjIter<AxPropertyValue>( axBasePropVal ) );

	Post( iter );

}

} // end of namespace

//=---------------------------------------------------------------------=

AxBaseRecordObjIter::AxBaseRecordObjIter( std::shared_ptr<AxRecordIterator> axRecordIter )
:	_axRecordIter( axRecordIter )
{}

AxBaseRecordObjIter::~AxBaseRecordObjIter()
{}

bool AxBaseRecordObjIter::NextOne( std::shared_ptr<AxBaseObj>& ret )
{
	AxRecordIterator::Pair nextRecordPair;

	if ( _axRecordIter->NextOne( nextRecordPair ) ) {
		std::shared_ptr<AxBaseObj>
			obj( new AxBaseObjAny<AxRecordIterator::Pair>( nextRecordPair ) );

		ret = obj;
		return true;
	}

	return false;
}

void AxBaseRecordObjIter::Reset()
{
	_axRecordIter->Reset();
}

std::shared_ptr<AxBaseObjIterPrtcl> AxBaseRecordObjIter::Clone()
{
	std::shared_ptr<AxBaseObjIterPrtcl> clone( 
		new AxBaseRecordObjIter( _axRecordIter->Clone() ) );

	return clone;
}

//=---------------------------------------------------------------------=

template <class TypeDef>
AxBaseArrayObjIter<TypeDef>::
AxBaseArrayObjIter( std::shared_ptr<AxArrayIterator<TypeDef> > axArrayIter )
:	_axArrayIter( axArrayIter )
{}

template <class TypeDef>
AxBaseArrayObjIter<TypeDef>::~AxBaseArrayObjIter()
{}

template <class TypeDef>
bool AxBaseArrayObjIter<TypeDef>::NextOne( std::shared_ptr<AxBaseObj>& ret )
{
        IAAFPropertyValueSP next;
        bool rc;
	rc = _axArrayIter->NextOne( next );
	if ( rc ) {
		std::shared_ptr<AxBaseObj>
			obj( new AxPropertyValue( next ) );

		ret = obj;
		return true;
	}

	return false;
}

template <class TypeDef>
void AxBaseArrayObjIter<TypeDef>::Reset()
{
	_axArrayIter->Reset();
}

template <class TypeDef>
std::shared_ptr<AxBaseObjIterPrtcl> AxBaseArrayObjIter<TypeDef>::Clone()
{
	std::shared_ptr<AxBaseObjIterPrtcl> clone( 
		new AxBaseArrayObjIter( _axArrayIter->Clone() ) );

	return clone;
}
//=---------------------------------------------------------------------=

AxBaseObjIterPrtcl::~AxBaseObjIterPrtcl()
{}


inline void AxBaseObjRecIter::Push( std::shared_ptr< AxBaseObjIterPrtcl > iter )
{
	_deque.push_back( iter.get() );
}

inline void AxBaseObjRecIter::Pop()
{
        delete _deque.back();
        _deque.pop_back();
}

inline AxBaseObjIterPrtcl& AxBaseObjRecIter::Top()
{
	return *_deque.back();
}

inline bool AxBaseObjRecIter::Empty()
{
	return _deque.empty();
}

inline int AxBaseObjRecIter::Size()
{
	return _deque.size();
}

AxBaseObjRecIter::AxBaseObjRecIter( std::shared_ptr< AxBaseObjIterPrtcl >  root )
:	_root( root )
{
	AxBaseObjRecIter::Push( _root->Clone() );
}


AxBaseObjRecIter::~AxBaseObjRecIter()
{}

bool AxBaseObjRecIter::NextOne( std::shared_ptr<AxBaseObj>& objRet, int& level )
{
	if ( Empty() ) {
	  return false;
	}

	AxBaseObjIterPrtcl& iter = Top();

	bool rc = iter.NextOne( objRet );

	if ( !rc ) {
		Pop();
		return NextOne( objRet, level );
	}

	level = GetLevel();

	// Peek at the pointer, use it, but don't take ownership!
	try {
		if ( dynamic_cast< AxObject* >( objRet.get() ) ) {

			AxObject& obj = dynamic_cast< AxObject& >( *objRet.get() );

			HandleObjectRecursion( obj );
		}
		else if ( dynamic_cast< AxProperty* >( objRet.get() ) ) {

			AxProperty& prop = dynamic_cast< AxProperty& >( *objRet.get() );

			HandlePropertyRecursion( prop );
		}	
		else if ( dynamic_cast< AxPropertyValue* >( objRet.get() ) ) {

			AxPropertyValue& propVal = dynamic_cast< AxPropertyValue& >( *objRet.get() );

			HandlePropertyValueRecursion( propVal );
		}                      
		else if ( dynamic_cast< AxBaseObjAny<AxRecordIterator::Pair>* >( objRet.get() ) ) {

			AxBaseObjAny<AxRecordIterator::Pair>& recPair =
				dynamic_cast< AxBaseObjAny<AxRecordIterator::Pair>& >( *objRet.get() );

			HandleRecordPropertyValueRecursion(	recPair.get() );
		
		}
	}
	catch ( const AxExHResult& ex ) {
		std::shared_ptr< AxBaseObjIterPrtcl > iter(
			new AxBaseSolitaryObjIter<AxBaseObjAny<AxExHResult> >(
				AxBaseObjAny<AxExHResult>( ex ) ) );
		Push( iter );
	}
	
	return true;
}

void AxBaseObjRecIter::PopStack()
{
	Pop();
}

int AxBaseObjRecIter::GetLevel()
{
	return Size();
}

void AxBaseObjRecIter::HandleObjectRecursion( AxObject& obj )
{
      	std::shared_ptr< AxBaseObjIterPrtcl > iter( 
		new AxBaseObjIter<AxPropertyIter, AxProperty, IAAFProperty>( obj.GetProperties() ) );

	Push( iter );
}

void AxBaseObjRecIter::HandlePropertyRecursion( AxProperty& prop )
{
	AxPropertyValue propVal( prop.GetValue() );
		
	std::shared_ptr< AxBaseObjIterPrtcl > iter(
		new AxBaseSolitaryObjIter<AxPropertyValue>( propVal ) );

	Push( iter );
}

void AxBaseObjRecIter::HandlePropertyValueRecursion( AxPropertyValue& propVal )
{
	PropValToIter valuePrtcl;

	propVal.Process( valuePrtcl );

	if ( valuePrtcl.ResultExists() ) {
		Push( valuePrtcl.GetResult() );
	}
}

void AxBaseObjRecIter::HandleRecordPropertyValueRecursion( AxRecordIterator::Pair& recPair )
{
	std::shared_ptr< AxBaseObjIterPrtcl > iter(
		new AxBaseSolitaryObjIter<AxPropertyValue>( AxPropertyValue(recPair.second) ) );

	Push( iter );
}

//=---------------------------------------------------------------------=
