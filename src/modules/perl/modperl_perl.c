#include "mod_perl.h"

/* this module contains mod_perl small tweaks to the Perl runtime
 * others (larger tweaks) are in their own modules, e.g. modperl_env.c
 */

typedef struct {
    const char *name;
    const char *sub_name;
    const char *core_name;
} modperl_perl_core_global_t;

#define MP_PERL_CORE_GLOBAL_ENT(name) \
{ name, "ModPerl::Util::" name, "CORE::GLOBAL::" name }

static modperl_perl_core_global_t MP_perl_core_global_entries[] = {
    MP_PERL_CORE_GLOBAL_ENT("exit"),
    { NULL },
};

void modperl_perl_core_global_init(pTHX)
{
    modperl_perl_core_global_t *cglobals = MP_perl_core_global_entries;

    while (cglobals->name) {
        GV *gv = gv_fetchpv(cglobals->core_name, TRUE, SVt_PVCV);
        GvCV(gv) = get_cv(cglobals->sub_name, TRUE);
        GvIMPORTED_CV_on(gv);
        cglobals++;
    }
}

static void modperl_perl_ids_get(modperl_perl_ids_t *ids)
{
    ids->pid  = (I32)getpid();
#ifndef WIN32
    ids->uid  = getuid();
    ids->euid = geteuid(); 
    ids->gid  = getgid(); 
    ids->gid  = getegid(); 

    MP_TRACE_g(MP_FUNC, 
               "uid=%d, euid=%d, gid=%d, egid=%d\n",
               (int)ids->uid, (int)ids->euid,
               (int)ids->gid, (int)ids->egid);
#endif
}

static void modperl_perl_init_ids(pTHX_ modperl_perl_ids_t *ids)
{
    sv_setiv(GvSV(gv_fetchpv("$", TRUE, SVt_PV)), ids->pid);

#ifndef WIN32
    PL_uid  = ids->uid;
    PL_euid = ids->euid;
    PL_gid  = ids->gid;
    PL_egid = ids->egid;
#endif
}


#ifdef USE_ITHREADS

static apr_status_t modperl_perl_init_ids_mip(pTHX_ modperl_interp_pool_t *mip,
                                              void *data)
{
    modperl_perl_init_ids(aTHX_ (modperl_perl_ids_t *)data);
    return APR_SUCCESS;
}

#endif /* USE_ITHREADS */

void modperl_perl_init_ids_server(server_rec *s)
{
    modperl_perl_ids_t ids;
    modperl_perl_ids_get(&ids);
#ifdef USE_ITHREADS
     modperl_interp_mip_walk_servers(NULL, s,
                                     modperl_perl_init_ids_mip,
                                    (void*)&ids);
#else
    modperl_perl_init_ids(aTHX_ &ids);
#endif
}

void modperl_perl_destruct(PerlInterpreter *perl)
{
    dTHXa(perl);

    PERL_SET_CONTEXT(perl);

    PL_perl_destruct_level = 2;

#ifdef USE_ENVIRON_ARRAY
    /* XXX: otherwise Perl may try to free() environ multiple times
     * but it wasn't Perl that modified environ
     * at least, not if modperl is doing things right
     * this is a bug in Perl.
     */
    PL_origenviron = environ;
#endif

    if (PL_endav) {
        modperl_perl_call_list(pTHX_ PL_endav, "END");
    }

    perl_destruct(perl);

    /* XXX: big bug in 5.6.1 fixed in 5.7.2+
     * XXX: see CLONEf_CLONE_HOST perl_clone() flag
     * XXX: try to find a workaround for 5.6.1
     */
#ifndef WIN32
    perl_free(perl);
#endif
}
