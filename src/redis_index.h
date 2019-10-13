#ifndef __REDIS_INDEX__
#define __REDIS_INDEX__

#include "document.h"
#include "index.h"
#include "inverted_index.h"
#include "search_ctx.h"
#include "concurrent_ctx.h"
#include "spec.h"

/* Open an inverted index reader on a redis DMA string, for a specific term.
 * If singleWordMode is set to 1, we do not load the skip index, only the score index
 */
IndexReader *Redis_OpenReader(RedisSearchCtx *ctx, RSQueryTerm *term, DocTable *dt,
                              int singleWordMode, t_fieldMask fieldMask, ConcurrentSearchCtx *csx,
                              double weight);

InvertedIndex *Redis_OpenInvertedIndexEx(RedisSearchCtx *ctx, const char *term, size_t len,
                                         int write, RedisModuleKey **keyp);
#define Redis_OpenInvertedIndex(ctx, term, len, isWrite) \
  Redis_OpenInvertedIndexEx(ctx, term, len, isWrite, NULL)
void Redis_CloseReader(IndexReader *r);

#define TERM_KEY_FORMAT "ft:%s/%.*s"
#define TERM_KEY_PREFIX "ft:"
#define SKIPINDEX_KEY_FORMAT "si:%s/%.*s"
#define SCOREINDEX_KEY_FORMAT "ss:%s/%.*s"

#define INVERTED_INDEX_ENCVER 1
#define INVERTED_INDEX_NOFREQFLAG_VER 0

typedef int (*ScanFunc)(RedisModuleCtx *ctx, RedisModuleString *keyName, void *opaque);

/* Optimize the buffers of a speicif term hit */
int Redis_OptimizeScanHandler(RedisModuleCtx *ctx, RedisModuleString *kn, void *opaque);

/* Drop the index and all the associated keys.
 *
 *  If deleteDocuments is non zero, we will delete the saved documents (if they exist).
 *  Only set this if there are no other indexes in the same redis instance.
 */
int Redis_DropIndex(RedisSearchCtx *ctx, int deleteDocuments, int deleteSpecKey);

/* Drop all the index's internal keys using this scan handler */
int Redis_DropScanHandler(RedisModuleCtx *ctx, RedisModuleString *kn, void *opaque);

/* Collect memory stas on the index */
int Redis_StatsScanHandler(RedisModuleCtx *ctx, RedisModuleString *kn, void *opaque);
/**
 * Format redis key for a term.
 * TODO: Add index name to it
 */
RedisModuleString *fmtRedisTermKey(RedisSearchCtx *ctx, const char *term, size_t len);
RedisModuleString *fmtRedisSkipIndexKey(RedisSearchCtx *ctx, const char *term, size_t len);
RedisModuleString *fmtRedisNumericIndexKey(RedisSearchCtx *ctx, const char *field);

extern RedisModuleType *InvertedIndexType;

void InvertedIndex_Free(void *idx);
void *InvertedIndex_RdbLoad(RedisModuleIO *rdb, int encver);
void InvertedIndex_RdbSave(RedisModuleIO *rdb, void *value);
void InvertedIndex_Digest(RedisModuleDigest *digest, void *value);
int InvertedIndex_RegisterType(RedisModuleCtx *ctx);
unsigned long InvertedIndex_MemUsage(const void *value);

#endif