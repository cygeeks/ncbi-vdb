/*===========================================================================
*
*                            PUBLIC DOMAIN NOTICE
*               National Center for Biotechnology Information
*
*  This software/database is a "United States Government Work" under the
*  terms of the United States Copyright Act.  It was written as part of
*  the author's official duties as a United States Government employee and
*  thus cannot be copyrighted.  This software/database is freely available
*  to the public for use. The National Library of Medicine and the U.S.
*  Government have not placed any restriction on its use or reproduction.
*
*  Although all reasonable efforts have been taken to ensure the accuracy
*  and reliability of the software and data, the NLM and the U.S.
*  Government do not and cannot warrant the performance or results that
*  may be obtained by using this software or data. The NLM and the U.S.
*  Government disclaim all warranties, express or implied, including
*  warranties of performance, merchantability or fitness for any particular
*  purpose.
*
*  Please cite the author in any work or product based on this material.
*
* ===========================================================================
*
*/
 
/**
* Integration tests for libmagic, version 5.04
*/
 
#include <ktst/unit_test.hpp>
 
#include <magic.h>
 
using namespace std;
 
TEST_SUITE(MagicTestSuite);
 
TEST_CASE(openClose)
{
    magic_t cookie = magic_open(0);
    REQUIRE_NOT_NULL(cookie);
    magic_close(cookie);
}
 
TEST_CASE(ASCII)
{
    magic_t cookie = magic_open(0);
    REQUIRE_NOT_NULL(cookie);
    
    REQUIRE_EQ(0, magic_load(cookie, NULL));
    REQUIRE_EQ(string("ASCII English text"), string(magic_file(cookie, "./Makefile"))); 
    
    magic_close(cookie);
}
 
//////////////////////////////////////////// Main
extern "C"
{
 
#include <kapp/args.h>
 
ver_t CC KAppVersion ( void )
{
    return 0x1000000;
}
rc_t CC UsageSummary (const char * progname)
{
    return 0;
}
 
rc_t CC Usage ( const Args * args )
{
    return 0;
}
 
const char UsageDefaultName[] = "test-magic";
 
rc_t CC KMain ( int argc, char *argv [] )
{
    rc_t rc=MagicTestSuite(argc, argv);
    return rc;
}
 
}
