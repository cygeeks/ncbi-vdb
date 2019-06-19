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

struct AWS;
#define CLOUD_IMPL struct AWS

#include <cloud/extern.h>
#include <cloud/impl.h>
#include <cloud/aws.h>

#include <klib/rc.h>
#include <klib/status.h>
#include <klib/text.h>
#include <kns/http.h>

#include <assert.h>

/*--------------------------------------------------------------------------
 * AWS
 */
struct AWS
{
    Cloud dad;
};

/* Destroy
 */
static
rc_t CC AWSDestroy ( AWS * self )
{
    free ( self );
    return 0;
}

/* MakeComputeEnvironmentToken
 *  contact cloud provider to get proof of execution environment in form of a token
 */
static
rc_t CC AWSMakeComputeEnvironmentToken ( const AWS * self, const String ** ce_token )
{
    return 0; //TODO
}

/* AddComputeEnvironmentTokenForSigner
 *  prepare a request object with a compute environment token
 *  for use by an SDL-associated "signer" service
 */
static
rc_t CC AWSAddComputeEnvironmentTokenForSigner ( const AWS * self, KClientHttpRequest * req )
{
    return 0; //TODO
}

/* AddUserPaysCredentials
 *  prepare a request object with credentials for user-pays
 */
static
rc_t CC AWSAddUserPaysCredentials ( const AWS * self, KClientHttpRequest * req )
{
    return 0; //TODO
}

static Cloud_vt_v1 AWS_vt_v1 =
{
    1, 0,

    AWSDestroy,
    AWSMakeComputeEnvironmentToken,
    AWSAddComputeEnvironmentTokenForSigner,
    AWSAddUserPaysCredentials
};

/* MakeAWS
 *  make an instance of an AWS cloud interface
 */
LIB_EXPORT rc_t CC CloudMgrMakeAWS ( const CloudMgr * self, AWS ** p_aws )
{
    rc_t rc;
//TODO: check self, aws
    AWS * aws = calloc ( 1, sizeof * aws );
    if ( aws == NULL )
    {
        rc = RC ( rcNS, rcMgr, rcAllocating, rcMemory, rcExhausted );
    }
    else
    {
        rc = CloudInit ( & aws -> dad, ( const Cloud_vt * ) & AWS_vt_v1, "AWS" );
        if ( rc == 0 )
        {
            * p_aws = aws;
        }
        else
        {
            free ( aws );
        }

    }

    return rc;
}

/* AddRef
 * Release
 */
LIB_EXPORT rc_t CC AWSAddRef ( const AWS * self )
{
    return CloudAddRef ( & self -> dad );
}

LIB_EXPORT rc_t CC AWSRelease ( const AWS * self )
{
    return CloudRelease ( & self -> dad );
}

/* Cast
 *  cast from a Cloud to an AWS type or vice versa
 *  allows us to apply cloud-specific interface to cloud object
 *
 *  returns a new reference, meaning the "self" must still be released
 */
LIB_EXPORT rc_t CC AWSToCloud ( const AWS * cself, Cloud ** cloud )
{
    rc_t rc;
    AWS * self = ( AWS * ) cself;

    if ( self == NULL )
        rc = RC ( rcCloud, rcProvider, rcCasting, rcSelf, rcNull );
    else if ( cloud == NULL )
        rc = RC ( rcCloud, rcProvider, rcCasting, rcParam, rcNull );
    else
    {
        rc = CloudAddRef ( & self -> dad );
        if ( rc == 0 )
        {
            * cloud = & self -> dad;
            return 0;
        }

        * cloud = NULL;
    }

    return rc;
}

LIB_EXPORT rc_t CC CloudToAWS ( const Cloud * self, AWS ** aws )
{
    rc_t rc;

    if ( self == NULL )
        rc = RC ( rcCloud, rcProvider, rcCasting, rcSelf, rcNull );
    else if ( aws == NULL )
        rc = RC ( rcCloud, rcProvider, rcCasting, rcParam, rcNull );
    else
    {
        if ( self == NULL )
            rc = 0;
        else if ( self -> vt != ( const Cloud_vt * ) & AWS_vt_v1 )
            rc = RC ( rcCloud, rcProvider, rcCasting, rcType, rcIncorrect );
        else
        {
            rc = CloudAddRef ( self );
            if ( rc == 0 )
            {
                * aws = ( AWS * ) self;
                return 0;
            }
        }

        * aws = NULL;
    }

    return rc;
}

#if 0

static rc_t aws_KConfigNodeUpdateChild (
    KConfigNode *self, String *name, String *value )
{
    KConfigNode *child;

    rc_t rc = KConfigNodeOpenNodeUpdate ( self, &child, "%S", name );
    if ( rc == 0 ) {
        rc = KConfigNodeWrite ( child, value->addr, value->size );
        KConfigNodeRelease ( child );
    }

    return rc;
}

static rc_t aws_extract_key_value_pair (
    const String *source, String *key, String *val )
{
    String k, v;
    const char *start = source->addr;
    const char *end = start + source->size;

    char *eql = string_chr ( start, source->size, '=' );
    if ( eql == NULL )
        return RC ( rcKFG, rcChar, rcSearching, rcFormat, rcInvalid );

    /* key */
    StringInit ( &k, start, eql - start, string_len ( start, eql - start ) );
    StringTrim ( &k, key );

    start = eql + 1;

    /* value */
    StringInit ( &v, start, end - start, string_len ( start, end - start ) );
    StringTrim ( &v, val );

    return 0;
}

static void aws_parse_file (
    const KFile *cred_file, KConfigNode *aws_node, const String *profile )
{
    size_t buf_size = 0;
    size_t num_read = 0;
    rc_t rc = 0;

    rc = KFileSize ( cred_file, &buf_size );
    if ( rc ) return;

    char *buffer = malloc ( buf_size );
    rc = KFileReadAll ( cred_file, 0, buffer, (size_t)buf_size, &num_read );

    if ( rc ) {
        free ( buffer );
        return;
    }
    String bracket;
    CONST_STRING ( &bracket, "[" );

    const String *temp1;
    StringConcat ( &temp1, &bracket, profile );
    CONST_STRING ( &bracket, "]" );
    const String *brack_profile;
    StringConcat ( &brack_profile, temp1, &bracket );

    const char *start = buffer;
    const char *end = start + buf_size;
    const char *sep = start;
    --sep;
    bool in_profile = false;


    for ( ; start < end; start = sep + 1 ) {
        rc_t rc;
        String string, trim;
        String key, value;
        sep = string_chr ( start, end - start, '\n' );
        if ( sep == NULL ) sep = (char *)end;

        StringInit (
            &string, start, sep - start, string_len ( start, sep - start ) );

        StringTrim ( &string, &trim );
        /* check for empty line and skip */
        if ( StringLength ( &trim ) == 0 ) continue;
        /*
                {
                    char *p = string_dup ( trim.addr, StringLength ( &trim ) );
                    fprintf ( stderr, "line: %s\n", p );
                    free ( p );
                }
        */
        /* check for comment line and skip */
        if ( trim.addr[0] == '#' ) continue;

        /* check for [profile] line */
        if ( trim.addr[0] == '[' ) {
            if ( StringEqual ( &trim, brack_profile ) ) { in_profile = true; }
            continue;
        }

        if ( !in_profile ) continue;

        /* check for key/value pairs and skip if none found */
        rc = aws_extract_key_value_pair ( &trim, &key, &value );
        if ( rc != 0 ) continue;

        /* now check keys we are looking for and populate the node*/

        String access_key_id, secret_access_key;
        CONST_STRING ( &access_key_id, "aws_access_key_id" );
        CONST_STRING ( &secret_access_key, "aws_secret_access_key" );

        if ( StringCaseEqual ( &key, &access_key_id ) ) {
            rc = aws_KConfigNodeUpdateChild ( aws_node, &key, &value );
            if ( rc != 0 ) return;
        }

        if ( StringCaseEqual ( &key, &secret_access_key ) ) {
            rc = aws_KConfigNodeUpdateChild ( aws_node, &key, &value );
            if ( rc != 0 ) return;
        }

        String region, output;
        CONST_STRING ( &region, "region" );
        CONST_STRING ( &output, "output" );

        if ( StringCaseEqual ( &key, &region ) ) {
            rc = aws_KConfigNodeUpdateChild ( aws_node, &key, &value );
            if ( rc != 0 ) return;
        }
        if ( StringCaseEqual ( &key, &output ) ) {
            rc = aws_KConfigNodeUpdateChild ( aws_node, &key, &value );
            if ( rc != 0 ) return;
        }
    }
    StringWhack ( temp1 );
    StringWhack ( brack_profile );
    free ( buffer );
}

static rc_t aws_find_nodes (
    KConfigNode *aws_node, const char *aws_path, const String *profile )
{
    bool done = false;

    KDirectory *wd = NULL;
    rc_t rc = KDirectoryNativeDir ( &wd );
    if ( rc ) return rc;
    const KFile *cred_file = NULL;

    const char *conf_env = getenv ( "AWS_CONFIG_FILE" );
    if ( conf_env ) {
        rc = KDirectoryOpenFileRead ( wd, &cred_file, "%s", conf_env );
        if ( rc ) return rc;
        aws_parse_file ( cred_file, aws_node, profile );
        KFileRelease ( cred_file );
        done = true;
    }
    const char *cred_env = getenv ( "AWS_SHARED_CREDENTIAL_FILE" );
    if ( cred_env ) {
        rc = KDirectoryOpenFileRead ( wd, &cred_file, "%s", cred_env );
        if ( rc ) return rc;

        aws_parse_file ( cred_file, aws_node, profile );
        KFileRelease ( cred_file );
        done = true;
    }
    if ( !done ) {
        rc = KDirectoryOpenFileRead (
            wd, &cred_file, "%s%s", aws_path, "/credentials" );
        if ( rc ) return rc;

        aws_parse_file ( cred_file, aws_node, profile );
        KFileRelease ( cred_file );

        rc = KDirectoryOpenFileRead (
            wd, &cred_file, "%s%s", aws_path, "/config" );
        if ( rc ) return rc;

        aws_parse_file ( cred_file, aws_node, profile );

        KFileRelease ( cred_file );
    }

    KDirectoryRelease ( wd );
    return 0;
}


static void make_home_node ( const KConfig *self, char *path, size_t path_size )
{
    size_t num_read;
    const char *home;

    const KConfigNode *home_node;

    /* Check to see if home node exists */
    rc_t rc = KConfigOpenNodeRead ( self, &home_node, "HOME" );
    if ( home_node == NULL ) {
        /* just grab the HOME env variable */
        home = getenv ( "HOME" );
        if ( home != NULL ) {
            num_read = string_copy_measure ( path, path_size, home );
            if ( num_read >= path_size ) path[0] = 0;
        }
    } else {
        /* if it exists check for a path */
        rc = KConfigNodeRead ( home_node, 0, path, path_size, &num_read, NULL );
        if ( rc != 0 ) {
            home = getenv ( "HOME" );
            if ( home != NULL ) {
                num_read = string_copy_measure ( path, path_size, home );
                if ( num_read >= path_size ) path[0] = 0;
            }
        }

        rc = KConfigNodeRelease ( home_node );
    }
}

KFG_EXTERN rc_t CC add_aws_nodes ( KConfig *self )
{
    rc_t rc = 0;

    /* Get Profile */
    String sprofile;
    char profile[4096];
    if ( getenv ( "AWS_PROFILE" ) != NULL ) {
        StringInitCString ( &sprofile, getenv ( "AWS_PROFILE" ) );
    } else {
        size_t num_writ = 0;
        rc = KConfig_Get_Aws_Profile (
            self, profile, sizeof ( profile ), &num_writ );
        if ( rc != 0 && num_writ == 0 ) {
            CONST_STRING ( &sprofile, "default" );
        } else {
            StringInitCString ( &sprofile, profile );
        }
    }

    /* Build config node */
    KConfigNode *aws_node = NULL;
    rc = KConfigOpenNodeUpdate ( self, &aws_node, "aws", NULL );
    if ( rc ) return rc;

    /* Check paths and parse */
    char home[4096] = "";
    make_home_node ( self, home, sizeof home );

    /* Check Environment first */
    const char *aws_access_key_id = getenv ( "AWS_ACCESS_KEY_ID" );
    const char *aws_secret_access_key = getenv ( "AWS_SECRET_ACCESS_KEY" );
    if ( aws_access_key_id != NULL && aws_secret_access_key != NULL
        && strlen ( aws_access_key_id ) > 0
        && strlen ( aws_secret_access_key ) > 0 ) {
        /* Use environment variables */
        String access_key_id, secret_access_key;
        CONST_STRING ( &access_key_id, "aws_access_key_id" );
        CONST_STRING ( &secret_access_key, "aws_secret_access_key" );
        String value;
        StringInitCString ( &value, aws_access_key_id );
        aws_KConfigNodeUpdateChild ( aws_node, &access_key_id, &value );
        StringInitCString ( &value, aws_secret_access_key );
        aws_KConfigNodeUpdateChild ( aws_node, &secret_access_key, &value );

        KConfigNodeRelease ( aws_node );
        return 0;
    }

    if ( home[0] != 0 ) {
        char path[4096] = "";
        size_t num_writ = 0;
        rc = string_printf ( path, sizeof path, &num_writ, "%s/.aws", home );
        if ( rc == 0 && num_writ != 0 ) {
            /* Use config files */
            if ( rc == 0 ) {
                rc = aws_find_nodes ( aws_node, path, &sprofile );
                if ( rc ) {
                    /* OK if no .aws available */
                    rc = 0;
                }
            }
        }
    }

    KConfigNodeRelease ( aws_node );

    return rc;
}


#endif
