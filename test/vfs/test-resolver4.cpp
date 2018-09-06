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
* ==============================================================================
*/

#include <klib/debug.h> /* KDbgSetString */

#include <ktst/unit_test.hpp> // TEST_SUITE

#include <vfs/path.h> /* VPathRelease */
#include <vfs/services-priv.h> /* KServiceNamesExecuteExt */

TEST_SUITE ( TestResolver4 )

TEST_CASE ( Test ) {
    KService * s = NULL;
    REQUIRE_RC ( KServiceMake ( & s ) );
    const KSrvResponse * r = NULL;
    REQUIRE_RC ( KServiceTestNamesExecuteExt ( s, 0, 0, "4.", & r,
      "{\"sequence\":[{\"status\":{\"code\":200},\"acc\":\"SRR000001\","
      "\"itemClass\":\"run\",\"format\":\"sra\",\"link\":\"http://h\"}]}" ) );
    REQUIRE_EQ ( KSrvResponseLength  ( r ), 1u );
    const KSrvRespObj * obj = NULL;

    REQUIRE_RC_FAIL ( KSrvResponseGetObjByIdx ( r, 1, & obj ) );
    REQUIRE_NULL    ( obj );
    REQUIRE_RC      ( KSrvResponseGetObjByIdx ( r, 0, & obj ) );
    REQUIRE_NOT_NULL( obj );
    REQUIRE_RC      ( KSrvRespObjRelease ( obj ) );

    REQUIRE_RC_FAIL ( KSrvResponseGetObjByAcc ( r, "SRR000000", & obj ) );
    REQUIRE_RC      ( KSrvResponseGetObjByAcc ( r, "SRR000001", & obj ) );

    REQUIRE_RC_FAIL ( KSrvRespObjMakeIterator ( NULL, NULL ) );
    REQUIRE_RC_FAIL ( KSrvRespObjMakeIterator ( obj , NULL ) );

    KSrvRespObjIterator * it = NULL;
    REQUIRE_RC      ( KSrvRespObjMakeIterator ( obj , & it ) );
    REQUIRE_NOT_NULL( it );

    REQUIRE_RC_FAIL ( KSrvRespObjIteratorNextFile ( NULL, NULL ) );
    REQUIRE_RC_FAIL ( KSrvRespObjIteratorNextFile ( it  , NULL ) );

    const KSrvRespFile * file = NULL;
    REQUIRE_RC_FAIL ( KSrvRespObjIteratorNextFile ( NULL, & file ) );
    REQUIRE_NULL    ( file );
    REQUIRE_RC      ( KSrvRespObjIteratorNextFile ( it  , & file ) );
    REQUIRE_NOT_NULL( file );

    REQUIRE_RC_FAIL ( KSrvRespFileMakeIterator ( NULL, 0, NULL ) );
    REQUIRE_RC_FAIL ( KSrvRespFileMakeIterator ( file, 0, NULL ) );

    KSrvRespFileIterator * fi = NULL;
    REQUIRE_RC_FAIL ( KSrvRespFileMakeIterator ( file, 0, & fi ) );
    REQUIRE_RC_FAIL ( KSrvRespFileMakeIterator ( file, eProtocolFasp , & fi ) );
    REQUIRE_RC_FAIL ( KSrvRespFileMakeIterator ( file, eProtocolHttps, & fi ) );
    REQUIRE_NULL    ( fi );
    REQUIRE_RC      ( KSrvRespFileMakeIterator ( file, eProtocolHttp , & fi ) );
    REQUIRE_NOT_NULL( fi );

    REQUIRE_RC_FAIL ( KSrvRespFileIteratorNextPath (  NULL, NULL ) );
    REQUIRE_RC_FAIL ( KSrvRespFileIteratorNextPath (  fi  , NULL ) );

    const VPath * path = NULL;
    REQUIRE_RC_FAIL ( KSrvRespFileIteratorNextPath (  NULL, & path ) );
    REQUIRE_NULL    ( path );

    REQUIRE_RC      ( KSrvRespFileIteratorNextPath (  fi  , & path ) );
    REQUIRE_NOT_NULL( path );

    const String * uri = NULL;
    REQUIRE_RC      ( VPathMakeUri ( path, & uri ) );
    REQUIRE_NOT_NULL( path );
    String e;
    CONST_STRING ( & e, "http://h" );
    REQUIRE ( StringEqual ( uri, & e ) );
    free ( const_cast < String * > ( uri ) );

    REQUIRE_RC      ( VPathRelease ( path ) );

    REQUIRE_RC      ( KSrvRespFileIteratorNextPath (  fi  , & path ) );
    REQUIRE_NULL    ( path );

    REQUIRE_RC      ( KSrvRespFileIteratorRelease ( fi ) );

    REQUIRE_RC      ( KSrvRespFileRelease ( file ) );

    REQUIRE_RC      ( KSrvRespObjIteratorRelease ( it ) );

    REQUIRE_RC      ( KSrvRespObjRelease ( obj ) );

    REQUIRE_RC      ( KSrvResponseRelease ( r ) );
    REQUIRE_RC      ( KServiceRelease ( s ) );
}

extern "C" {
    ver_t CC KAppVersion ( void ) { return 0; }
    rc_t CC KMain ( int argc, char * argv [] ) {
if (
0 ) assert ( ! KDbgSetString ( "VFS-JSON" ) );
        return TestResolver4 ( argc, argv );
    }
}
