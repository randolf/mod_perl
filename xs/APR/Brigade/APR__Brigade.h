static MP_INLINE
apr_bucket_brigade *mpxs_apr_brigade_create(pTHX_ SV *CLASS,
                                            apr_pool_t *p,
                                            apr_bucket_alloc_t *ba)
{
    return apr_brigade_create(p, ba);
}

#define get_brigade(brigade, fetch) \
(fetch(brigade) == APR_BRIGADE_SENTINEL(brigade) ? \
 NULL : fetch(brigade))

static MP_INLINE
apr_bucket *mpxs_APR__Brigade_first(apr_bucket_brigade *brigade)
{
    return get_brigade(brigade, APR_BRIGADE_FIRST);
}

static MP_INLINE
apr_bucket *mpxs_APR__Brigade_last(apr_bucket_brigade *brigade)
{
    return get_brigade(brigade, APR_BRIGADE_LAST);
}

#define get_bucket(brigade, bucket, fetch) \
(fetch(bucket) == APR_BRIGADE_SENTINEL(brigade) ? \
 NULL : fetch(bucket))

static MP_INLINE
apr_bucket *mpxs_APR__Brigade_next(apr_bucket_brigade *brigade,
                                    apr_bucket *bucket)
{
    return get_bucket(brigade, bucket, APR_BUCKET_NEXT);
}

static MP_INLINE
apr_bucket *mpxs_APR__Brigade_prev(apr_bucket_brigade *brigade,
                                   apr_bucket *bucket)
{
    return get_bucket(brigade, bucket, APR_BUCKET_PREV);
}

static MP_INLINE
void mpxs_APR__Brigade_insert_tail(apr_bucket_brigade *brigade,
                                   apr_bucket *bucket)
{
    APR_BRIGADE_INSERT_TAIL(brigade, bucket);
}

static MP_INLINE
void mpxs_APR__Brigade_insert_head(apr_bucket_brigade *brigade,
                                   apr_bucket *bucket)
{
    APR_BRIGADE_INSERT_HEAD(brigade, bucket);
}

static MP_INLINE
void mpxs_APR__Brigade_concat(apr_bucket_brigade *a,
                              apr_bucket_brigade *b)
{
    APR_BRIGADE_CONCAT(a, b);
}

static MP_INLINE
int mpxs_APR__Brigade_empty(apr_bucket_brigade *brigade)
{
    return APR_BRIGADE_EMPTY(brigade);
}

static MP_INLINE
SV *mpxs_APR__Brigade_length(pTHX_ apr_bucket_brigade *bb,
                             int read_all)
{
    apr_off_t length;

    apr_status_t rv = apr_brigade_length(bb, read_all, &length);

    /* XXX - we're deviating from the API here a bit in order to
     * make it more perlish - returning the length instead of
     * the return code.  maybe that's not such a good idea, though...
     */
    if (rv == APR_SUCCESS) {
        return newSViv((int)length);
    }

    return &PL_sv_undef;
}

static MP_INLINE
SV *mpxs_APR__Brigade_flatten(pTHX_ apr_bucket_brigade *bb,
                              apr_pool_t *pool, SV *sv_len)
{

    /* XXX we're deviating from the API here to try and make
     * the API more Perlish - nobody likes the idea of two
     * "in/out" arguments.  and we generally don't ever need
     * the length anyway...
     */

    apr_status_t status;
    char *buffer;
    apr_size_t length;

    if (SvTRUE(sv_len)) {
        /* APR::Brigade->flatten($p, $length);
         * use apr_brigade_flatten to get the first $length bytes
         *
         * note that $length must be non-zero to get here
         */

        length = mp_xs_sv2_apr_size_t(sv_len);

        /* since we always require a pool, we can allocate from it */
        buffer = apr_pcalloc(pool, length);

        status = apr_brigade_flatten(bb, buffer, &length);

    }
    else {
        /* APR::Brigade->flatten($p);
         * use apr_brigade_pflatten to slurp the entire brigade
         *
         * note that it doesn't matter what we pass in for length
         */

        status = apr_brigade_pflatten(bb, &buffer, &length, pool);

    }

    if (status != APR_SUCCESS) {
        /* XXX croak?
         * note that reading from an empty brigade will return
         * an empty string, not undef, so there is a difference
         */
        return &PL_sv_undef;
    }

    return newSVpvn(buffer, length);
}
