#ifndef __AAFFileSignatures_h__
#define __AAFFileSignatures_h__
/***********************************************************************
 *
 *              Copyright (c) 1998-2000 Avid Technology, Inc.
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

#ifndef __AAFTypes_h__
#include "AAFTypes.h"
#endif

// AAF (and MXF) file signatures as byte streams.

const aafInt8 aafObjectModelDevelopmentVersion = 0;

// Development versions start at -1 and count down. Version 0
// indicates a released version.

// If you change aafObjectModelDevelopmentVersion above you must also
// change the signatures below.

// The aafObjectModelDevelopmentVersion appears in the signatures as
// follows
//   - Byte 13 of the binary signatures.
//   - Bytes 73 (most significant) and 74 (least significant) of
//     the text signatures.

// The signature for structured storage binary AAF files. This
// includes the structured storage file signature.
//
const aafUInt8 aafFileSignatureAafSSBinary[] = {
  0xd0, 0xcf, 0x11, 0xe0, 0xa1, 0xb1, 0x1a, 0xe1,
  0x41, 0x41, 0x46, 0x42, 0x0d, 0x00, 0x4f, 0x4d,
  0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0xff
};

const aafUID_t aafFileSignatureAafSSBinaryGUID = 
{0x42464141, 0x000d, 0x4d4f, {0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0xff}};

// The signature for structured storage binary MXF files. This includes
// the structured storage file signature.
//
const aafUInt8 aafFileSignatureMxfSSBinary[] = {
  0xd0, 0xcf, 0x11, 0xe0, 0xa1, 0xb1, 0x1a, 0xe1,
  0x4d, 0x58, 0x46, 0x42, 0x0d, 0x00, 0x4f, 0x4d,
  0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0xff
};


// The signature for XML text AAF files.
//
const aafUInt8 aafFileSignatureAafXmlText[] = {
  0x3c, 0x3f, 0x78, 0x6d, 0x6c, 0x20, 0x76, 0x65,
  0x72, 0x73, 0x69, 0x6f, 0x6e, 0x3d, 0x22, 0x31,
  0x2e, 0x30, 0x22, 0x3f, 0x3e, 0x3c, 0x3f, 0x41,
  0x41, 0x46, 0x20, 0x73, 0x69, 0x67, 0x6e, 0x61,
  0x74, 0x75, 0x72, 0x65, 0x3d, 0x22, 0x7b, 0x35,
  0x38, 0x34, 0x36, 0x34, 0x31, 0x34, 0x31, 0x2d,
  0x46, 0x46, 0x30, 0x44, 0x2d, 0x34, 0x44, 0x34,
  0x46, 0x2d, 0x30, 0x36, 0x30, 0x45, 0x2d, 0x32,
  0x42, 0x33, 0x34, 0x30, 0x31, 0x30, 0x31, 0x30,
  0x31, 0x30, 0x30, 0x7d, 0x22, 0x3f, 0x3e
};

// The signature for XML text MXF files.
//
const aafUInt8 aafFileSignatureMxfXmlText[] = {
  0x3c, 0x3f, 0x78, 0x6d, 0x6c, 0x20, 0x76, 0x65,
  0x72, 0x73, 0x69, 0x6f, 0x6e, 0x3d, 0x22, 0x31,
  0x2e, 0x30, 0x22, 0x3f, 0x3e, 0x3c, 0x3f, 0x41,
  0x41, 0x46, 0x20, 0x73, 0x69, 0x67, 0x6e, 0x61,
  0x74, 0x75, 0x72, 0x65, 0x3d, 0x22, 0x7b, 0x35,
  0x38, 0x34, 0x36, 0x35, 0x38, 0x34, 0x44, 0x2d,
  0x46, 0x46, 0x30, 0x44, 0x2d, 0x34, 0x44, 0x34,
  0x46, 0x2d, 0x30, 0x36, 0x30, 0x45, 0x2d, 0x32,
  0x42, 0x33, 0x34, 0x30, 0x31, 0x30, 0x31, 0x30,
  0x31, 0x30, 0x30, 0x7d, 0x22, 0x3f, 0x3e
};

#endif // !__AAFFileSignatures_h__
