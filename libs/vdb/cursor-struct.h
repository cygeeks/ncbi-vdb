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

#ifndef _h_cursor_struct_
#define _h_cursor_struct_

#ifndef _h_vdb_extern_
#include <vdb/extern.h>
#endif

#ifndef _h_klib_vector_
#include <klib/vector.h>
#endif

#ifndef _h_klib_container_
#include <klib/container.h>
#endif

#ifndef _h_klib_refcount_
#include <klib/refcount.h>
#endif

#ifndef KONST
#define KONST
#endif

#ifndef SKONST
#define SKONST const
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------
 * forwards
 */
struct KLock;
struct KCondition;
struct KThread;
struct KNamelist;
struct KDlset;
struct VTable;
struct VCtxId;
struct VSchema;
struct SColumn;
struct VColumn;
struct VPhysical;

struct VCursor
{
    /* row id */
    int64_t row_id;

    /* half-closed page range */
    int64_t start_id, end_id;

    /* starting id for flush */
    volatile int64_t flush_id;

    /* attached reference to table */
    struct VTable KONST *tbl;

    /* cursor-specific schema and table */
    struct VSchema SKONST *schema;
    struct STable SKONST *stbl;

    /* background flush thread objects */
    int64_t launch_cnt;
    struct KThread *flush_thread;
    struct KLock *flush_lock;
    struct KCondition *flush_cond;

    /* background pagemap conversion objects */
    struct KThread *pagemap_thread;
    PageMapProcessRequest pmpr;

    /* user data */
    void *user;
    void ( CC * user_whack ) ( void *data );

    /* external named cursor parameters */
    BSTree named_params;

    /* linked cursors */
    BSTree linked_cursors;

    /* read-only blob cache */
    VBlobMRUCache *blob_mru_cache;

    /* external row of VColumn* by ord ( owned ) */
    Vector row;

    Vector v_cache_curs;
    Vector v_cache_cidx;
    /** trying to prevent forward prefetch on rows which are cached ***/
    bool    cache_col_active;
    int64_t cache_empty_start; /** first rowid where cache is detected to be empty **/
    int64_t cache_empty_end;   /** last  rowid  **/

    /* column objects by cid ( not-owned ) */
    VCursorCache col;

    /* physical columns by cid ( owned ) */
    VCursorCache phys;
    uint32_t phys_cnt;

    /* productions by cid ( not-owned ) */
    VCursorCache prod;

    /* intermediate productions ( owned ) */
    Vector owned;

    /* trigger productions ( not-owned ) */
    Vector trig;

    KRefcount refcount;

    volatile uint32_t flush_cnt;

    /* foreground state */
    uint8_t state;

    /* flush_state */
    volatile uint8_t flush_state;

    bool read_only;

    /* support for sradb-v1 API */
    bool permit_add_column;
    bool permit_post_open_add;
    /* support suspension of schema-declared triggers **/
    bool suspend_triggers;
    /* cursor used in sub-selects */
    bool is_sub_cursor;
    /* cursor for VDB columns located in separate db.tbl ***/
    const struct VCursor* cache_curs;
};

#ifdef __cplusplus
}
#endif

#endif /* _h_cursor_struct_ */
