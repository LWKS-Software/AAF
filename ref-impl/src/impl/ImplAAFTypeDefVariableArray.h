#ifndef __ImplAAFTypeDefVariableArray_h__
#define __ImplAAFTypeDefVariableArray_h__

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

class ImplAAFPropertyValue;

#ifndef __ImplAAFTypeDefArray_h__
#include "ImplAAFTypeDefArray.h"
#endif

#include "OMWeakRefProperty.h"

class ImplAAFTypeDefVariableArray : public ImplAAFTypeDefArray
{
public:
  //
  // Constructor/destructor
  //
  //********
  ImplAAFTypeDefVariableArray ();

protected:
  virtual ~ImplAAFTypeDefVariableArray ();

public:

  // override from ImplAAFTypeDefArray
  virtual AAFRESULT STDMETHODCALLTYPE
    GetType
        // @parm [out] type of elements in this array
        (ImplAAFTypeDef ** ppTypeDef) const;


  //****************
  // Initialize()
  //
  virtual AAFRESULT STDMETHODCALLTYPE
    Initialize
        (// @parm [in] auid to be used to identify this type
         const aafUID_t & id,

         // @parm [in] type of each element to be contained in this array
         ImplAAFTypeDef * pTypeDef,

         // @parm [in] friendly name of this type definition
         const aafCharacter * pTypeName);


  //****************
  // GetCount()
  //
  virtual AAFRESULT STDMETHODCALLTYPE
    GetCount
        (// @parm [in] property value of array
         ImplAAFPropertyValue * pPropVal,

         // @parm [out] count of elements in the specified array property value
         aafUInt32 *  pCount) const;


  //****************
  // AppendElement()
  //
  virtual AAFRESULT STDMETHODCALLTYPE
    AppendElement
        (// @parm [in] property value corresponding to array to which element is appended
         ImplAAFPropertyValue * pInPropVal,

         // @parm [in] value to be appended to this array
         ImplAAFPropertyValue * pMemberPropVal);


  //****************
  // CreateEmptyValue()
  //
  virtual AAFRESULT STDMETHODCALLTYPE
    CreateEmptyValue
        (// @parm [out] newly-created property empty value,
         ImplAAFPropertyValue ** ppPropVal);
			 
  //****************
  // CreateValueFromValues() 
  //
  virtual AAFRESULT STDMETHODCALLTYPE
    CreateValueFromValues
        (// @parm [in, size_is(numElements)] array of property values for elements of array value which
    // is to be created.
         ImplAAFPropertyValue ** ppElementValues,

         // @parm [in] size of pElementValues array.
         aafUInt32  numElements,

         // @parm [out] newly-created property value
         ImplAAFPropertyValue ** ppPropVal);


  // Override from AAFTypeDef
  virtual AAFRESULT STDMETHODCALLTYPE
    GetTypeCategory (/*[out]*/ eAAFTypeCategory_t *  pTid);


  //*************************************************************
  //
  // Overrides from OMType, via inheritace through ImplAAFTypeDef
  //
  //*************************************************************


  virtual void reorder(OMByte* externalBytes,
                       size_t externalBytesSize) const;

  virtual size_t externalSize(OMByte* internalBytes,
							  size_t internalBytesSize) const;

  virtual void externalize(OMByte* internalBytes,
                           size_t internalBytesSize,
                           OMByte* externalBytes,
                           size_t externalBytesSize,
                           OMByteOrder byteOrder) const;

  virtual size_t internalSize(OMByte* externalBytes,
							  size_t externalBytesSize) const;

  virtual void internalize(OMByte* externalBytes,
                           size_t externalBytesSize,
                           OMByte* internalBytes,
                           size_t internalBytesSize,
                           OMByteOrder byteOrder) const;


  //****************
  // pvtInitialize()
  //
  virtual AAFRESULT STDMETHODCALLTYPE
    pvtInitialize
        (// @parm [in] auid to be used to identify this type
         const aafUID_t & id,

         // @parm [in] type of each element to be contained in this array
         const ImplAAFTypeDef *pType,

         // @parm [in] friendly name of this type definition
         const aafCharacter * pTypeName);


protected:
  // override from ImplAAFTypeDefArray
  virtual aafUInt32 pvtCount (ImplAAFPropertyValue * pInPropVal) const;


public:

  // overrides from ImplAAFTypeDef
  //
  virtual aafBool IsFixedSize (void) const;
  virtual size_t PropValSize (void) const;
  virtual aafBool IsRegistered (void) const;
  virtual size_t NativeSize (void) const;

  virtual OMProperty * 
    pvtCreateOMProperty (OMPropertyId pid,
							const wchar_t * name) const;

  virtual AAFRESULT STDMETHODCALLTYPE
    RawAccessType
        (ImplAAFTypeDef ** ppRawTypeDef);

public:
  // Overrides from ImplAAFTypeDef
  virtual bool IsAggregatable () const;
  virtual bool IsStreamable () const;
  virtual bool IsFixedArrayable () const;
  virtual bool IsVariableArrayable () const;
  virtual bool IsStringable () const;


  // override from OMStorable.
  virtual const OMClassId& classId(void) const;

  // Override callbacks from OMStorable
  virtual void onSave(void* clientContext) const;
  virtual void onRestore(void* clientContext) const;

private:
   OMWeakReferenceProperty<ImplAAFTypeDef> _ElementType;

  ImplAAFTypeDefSP BaseType (void) const;
};

//
// smart pointer
//

#ifndef __ImplAAFSmartPointer_h__
// caution! includes assert.h
#include "ImplAAFSmartPointer.h"
#endif

typedef ImplAAFSmartPointer<ImplAAFTypeDefVariableArray> ImplAAFTypeDefVariableArraySP;

#endif // ! __ImplAAFTypeDefVariableArray_h__
