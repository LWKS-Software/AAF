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
 * prior written permission of Avid Technology, Inc.
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

#include "AAFStoredObjectIDs.h"
#include "AAFPropertyIDs.h"

#ifndef __ImplAAFFilmDescriptor_h__
#include "ImplAAFFilmDescriptor.h"
#endif

#include <assert.h>
#include <string.h>
#include "aafErr.h"

ImplAAFFilmDescriptor::ImplAAFFilmDescriptor ()
: _manufacturer(	PID_FilmDescriptor_Manufacturer,		"Manufacturer"),
  _model(			PID_FilmDescriptor_Model,				"Model"),
  _format(			PID_FilmDescriptor_FilmFormat,			"FilmFormat"),
  _frameRate(		PID_FilmDescriptor_FrameRate,			"FrameRate"),
  _perfPerFrame(	PID_FilmDescriptor_PerforationsPerFrame,"PerforationsPerFrame"),
  _aspectRatio(		PID_FilmDescriptor_FilmAspectRatio,		"FilmAspectRatio")
{
	_persistentProperties.put(_manufacturer.address());
	_persistentProperties.put(_model.address());
	_persistentProperties.put(_format.address());
	_persistentProperties.put(_frameRate.address());
	_persistentProperties.put(_perfPerFrame.address());
	_persistentProperties.put(_aspectRatio.address());
}

ImplAAFFilmDescriptor::~ImplAAFFilmDescriptor ()
{
}



AAFRESULT STDMETHODCALLTYPE
    ImplAAFFilmDescriptor::SetFilmManufacturer (
      const aafCharacter *name)
{
	if(name == NULL)
		return(AAFRESULT_NULL_PARAM);

	_manufacturer = name;

	return(AAFRESULT_SUCCESS); 
}



AAFRESULT STDMETHODCALLTYPE
    ImplAAFFilmDescriptor::GetFilmManufacturer (
      aafCharacter *name, aafUInt32 bufSize)
{
	if(name == NULL)
		return(AAFRESULT_NULL_PARAM);

	if(!_manufacturer.isPresent())
		return AAFRESULT_PROP_NOT_PRESENT;

	bool stat;

	stat = _manufacturer.copyToBuffer(name, bufSize);
	if (! stat)
	{
	  return AAFRESULT_SMALLBUF;	// Shouldn't the API have a length parm?
	}

	return(AAFRESULT_SUCCESS); 
}


AAFRESULT STDMETHODCALLTYPE
    ImplAAFFilmDescriptor::GetFilmManufacturerBufLen (
      aafUInt32 *bufSize)
{
	if(bufSize == NULL)
		return(AAFRESULT_NULL_PARAM);
	
	if(!_manufacturer.isPresent())
		return AAFRESULT_PROP_NOT_PRESENT;
	
	*bufSize = _manufacturer.size();
	return(AAFRESULT_SUCCESS); 
}
	

AAFRESULT STDMETHODCALLTYPE
    ImplAAFFilmDescriptor::SetFilmModel (
      const aafCharacter *name)
{
	if(name == NULL)
		return(AAFRESULT_NULL_PARAM);

	_model = name;

	return(AAFRESULT_SUCCESS); 
}



AAFRESULT STDMETHODCALLTYPE
    ImplAAFFilmDescriptor::GetFilmModel (
      aafCharacter *name, aafUInt32 bufSize)
{
	if(name == NULL)
		return(AAFRESULT_NULL_PARAM);

	if(!_model.isPresent())
		return AAFRESULT_PROP_NOT_PRESENT;

	bool stat;

	stat = _model.copyToBuffer(name, bufSize);
	if (! stat)
	{
	  return AAFRESULT_SMALLBUF;	// Shouldn't the API have a length parm?
	}

	return(AAFRESULT_SUCCESS); 
}


AAFRESULT STDMETHODCALLTYPE
    ImplAAFFilmDescriptor::GetFilmModelBufLen (
      aafUInt32 *bufSize)
{		
	if(bufSize == NULL)
		return(AAFRESULT_NULL_PARAM);

	if(!_model.isPresent())
		return AAFRESULT_PROP_NOT_PRESENT;
	
	*bufSize = _model.size();
	return(AAFRESULT_SUCCESS); 
}


AAFRESULT STDMETHODCALLTYPE
    ImplAAFFilmDescriptor::GetFilmFormat (
      aafFilmType_t*filmFormat)
{
	aafAssert(filmFormat != NULL, NULL, AAFRESULT_NULL_PARAM);

	if(!_format.isPresent())
		return AAFRESULT_PROP_NOT_PRESENT;
	
	*filmFormat = _format;
	return(AAFRESULT_SUCCESS); 
}


AAFRESULT STDMETHODCALLTYPE
    ImplAAFFilmDescriptor::GetFrameRate (
      aafUInt32*  rate)
{
	aafAssert(rate != NULL, NULL, AAFRESULT_NULL_PARAM);

	if(!_frameRate.isPresent())
		return AAFRESULT_PROP_NOT_PRESENT;
	
	*rate = _frameRate;
	return(AAFRESULT_SUCCESS); 
}



AAFRESULT STDMETHODCALLTYPE
    ImplAAFFilmDescriptor::GetPerfPerFrame (
      aafUInt8* perfPerFrame)
{
	aafAssert(perfPerFrame != NULL, NULL, AAFRESULT_NULL_PARAM);
	
	if(!_perfPerFrame.isPresent())
		return AAFRESULT_PROP_NOT_PRESENT;
	
	*perfPerFrame = _perfPerFrame;
	return(AAFRESULT_SUCCESS); 
}



AAFRESULT STDMETHODCALLTYPE
    ImplAAFFilmDescriptor::GetFilmAspectRatio (
      aafRational_t*aspectRatio)
{
	aafAssert(aspectRatio != NULL, NULL, AAFRESULT_NULL_PARAM);
	
	if(!_aspectRatio.isPresent())
		return AAFRESULT_PROP_NOT_PRESENT;
	
	*aspectRatio = _aspectRatio;
	return(AAFRESULT_SUCCESS); 
}



AAFRESULT STDMETHODCALLTYPE
    ImplAAFFilmDescriptor::SetFilmFormat (
      aafFilmType_t  filmFormat)
{
	_format = filmFormat;
	return(AAFRESULT_SUCCESS); 
}


AAFRESULT STDMETHODCALLTYPE
    ImplAAFFilmDescriptor::SetFrameRate (
      aafUInt32 rate)
{
	_frameRate = rate;
	return(AAFRESULT_SUCCESS); 
}



AAFRESULT STDMETHODCALLTYPE
    ImplAAFFilmDescriptor::SetPerfPerFrame (
      aafUInt8 perfPerFrame)
{
	_perfPerFrame = perfPerFrame;
	return(AAFRESULT_SUCCESS); 
}



AAFRESULT STDMETHODCALLTYPE
    ImplAAFFilmDescriptor::SetFilmAspectRatio (
      aafRational_t  aspectRatio)
{
	_aspectRatio = aspectRatio;
	return(AAFRESULT_SUCCESS); 
}



AAFRESULT STDMETHODCALLTYPE
    ImplAAFFilmDescriptor::GetOwningMobKind (aafMobKind_t *pMobKind)
{
	*pMobKind = kFilmMob;
	return(AAFRESULT_SUCCESS);
}
