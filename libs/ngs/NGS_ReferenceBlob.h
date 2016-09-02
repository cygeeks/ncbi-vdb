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

#ifndef _h_NGS_ReferenceBlob_
#define _h_NGS_ReferenceBlob_

typedef struct NGS_ReferenceBlob NGS_ReferenceBlob;
#ifndef _h_ngs_refcount_
#define NGS_REFCOUNT NGS_ReferenceBlob
#include "NGS_Refcount.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct NGS_ReferenceBlob;
struct NGS_Cursor;
struct NGS_String;

/*--------------------------------------------------------------------------
 * NGS_ReferenceBlob
 *  Access to blobs in a REFERENCE.READ column
 *  reference counted
 */

/* Make
 *  create a blob containing the given row in the reference
 */
struct NGS_ReferenceBlob * NGS_ReferenceBlobMake ( ctx_t ctx, const struct NGS_Cursor* curs, int64_t firstRowId );

/* Release
 *  release reference to the blob
 */
void NGS_ReferenceBlobRelease ( struct NGS_ReferenceBlob * self, ctx_t ctx );

/* Duplicate
 *  duplicate reference to the blob
 */
NGS_ReferenceBlob * NGS_ReferenceBlobDuplicate (  struct NGS_ReferenceBlob * self, ctx_t ctx );

/* RowRange
 */
void NGS_ReferenceBlobRowRange ( const struct NGS_ReferenceBlob * self, ctx_t ctx, int64_t * p_first, uint64_t * p_count );

/* Data
 *  returns the blob's data buffer
 */
const void* NGS_ReferenceBlobData ( const struct NGS_ReferenceBlob * self, ctx_t ctx );

/* Size
 *  returns the size the blob's data buffer
 */
uint64_t NGS_ReferenceBlobSize ( const struct NGS_ReferenceBlob * self, ctx_t ctx );

/* ResolveOffset
 *  resolve offset inside the blob into offset on the reference
 *  inBlob - an offset into the blob
 *  inReference [ OUT ] - offset into the reference, corresponding to inBlob
 *  uint32_t* repeatCount [ OUT, NULL OK ] - the number of repetitions of the reference chunk  the offset is inside of (1 if no repeats)
 *  uint64_t* increment [ OUT, NULL OK ] - size of the repeated chunk (0 if no repeats)
 */
void NGS_ReferenceBlobResolveOffset ( const struct NGS_ReferenceBlob * self, ctx_t ctx, uint64_t inBlob, uint64_t* inReference, uint32_t* repeatCount, uint64_t* increment );

/* Find repeated chunks
 *  startInBlob - an offset into the blob, start of the search for repated chunks
 *  nextInBlob [ OUT ] - offset into the blob corresponding to the found repeated chunk; use NGS_ReferenceBlobResolveOffset() to obtain details
 *  inReference [ OUT ] - offset into the reference, corresponding to nextInBlob
 *  uint32_t* repeatCount [ OUT, NULL OK ] - the number of repetitions of the reference chunk  the offset is inside of (1 if no repeats)
 *  uint64_t* increment [ OUT, NULL OK ] - size of the repeated chunk (0 if no repeats)
 *
 *  return false if no repeats are found
 */
bool NGS_ReferenceBlobFindRepeat ( const struct NGS_ReferenceBlob * self, ctx_t ctx, uint64_t p_startInBlob, uint64_t* p_nextInBlob, uint64_t* p_inReference, uint32_t* p_repeatCount, uint64_t* p_increment );

/*TEMPORARY*/
void NGS_ReferenceBlobPrintPageMap ( const struct NGS_ReferenceBlob * self, ctx_t ctx );

#ifdef __cplusplus
}
#endif

#endif /* _h_NGS_ReferenceBlob_ */
