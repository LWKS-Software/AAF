//=---------------------------------------------------------------------=
//
// $Id$
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

//AAF files
#include <AAFPlatform.h>
#include <AAFTypes.h>

//Boost files
#include <boost/shared_ptr.hpp>

//STL files
#include <map>
#include <set>

#ifndef __AllowedEditRateTable_h_
#define __AllowedEditRateTable_h_

namespace aafanalyzer {

using namespace boost;
using namespace std;

class AllowedEditRateTable
{
 public:

    typedef map<aafInt32, aafInt32> NumeratorMap;
    typedef map<double, aafInt32> RoundedMap;
    typedef map<aafInt32, shared_ptr<NumeratorMap> > DenominatorMap;
    typedef set<aafInt32> TrackSet;
    
    AllowedEditRateTable();
    virtual ~AllowedEditRateTable();
    
    bool IsInTable( aafRational_t editRate, bool isVideo );
    double Round( double num );

 private:
 
    DenominatorMap _editRateTable;
    RoundedMap _roundedEditRates;
    TrackSet _nominalVideoRates;
    
    void InitializeTable();

    // prohibited
    AllowedEditRateTable( const AllowedEditRateTable& );
    AllowedEditRateTable& operator=( const AllowedEditRateTable& );

};
    
} // end of namespace aafanalyzer

#endif /*__AllowedEditRateTable_h_*/