#ifdef WIN32
#define NO_PERL_CHILD_INIT
#define NO_PERL_CHILD_EXIT
#include "dirent.h"
#endif

#define IS_MODULE
#define SHARED_MODULE

#ifdef PERL_THREADS
#define _INCLUDE_APACHE_FIRST
#endif

#ifdef _INCLUDE_APACHE_FIRST
#include "httpd.h" 
#include "http_config.h" 
#include "http_protocol.h" 
#include "http_log.h" 
#include "http_main.h" 
#include "http_core.h" 
#include "http_request.h" 
#include "util_script.h" 
#include "http_conf_globals.h"
#endif

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#ifndef MOD_PERL_VERSION
#define MOD_PERL_VERSION "TRUE"
#endif

/* perl hides it's symbols in libperl when these macros are 
 * expanded to Perl_foo
 * but some cause conflict when expanded in other headers files
 */
#undef S_ISREG
#undef DIR
#undef VOIDUSED
#undef pregexec
#undef pregfree
#undef pregcomp
#undef setregid
#undef setreuid
#undef sync
#undef my_memcmp
#undef my_bcopy
#undef my_memset
#undef RETURN
#undef die
#undef __attribute__

#ifndef _INCLUDE_APACHE_FIRST
#include "httpd.h" 
#include "http_config.h" 
#include "http_protocol.h" 
#include "http_log.h" 
#include "http_main.h" 
#include "http_core.h" 
#include "http_request.h" 
#include "util_script.h" 
#include "http_conf_globals.h"
#endif

#ifndef dTHR
#define dTHR extern int errno
#endif

#ifndef ERRSV
#define ERRSV GvSV(errgv) 
#endif

#ifndef ERRHV
#define ERRHV GvHV(errgv)
#endif

#ifndef AvFILLp
#define AvFILLp(av)	((XPVAV*)  SvANY(av))->xav_fill
#endif

#define MP_EXISTS_ERROR(k) \
ERRHV && hv_exists(ERRHV, k, strlen(k))

#define MP_STORE_ERROR(k,v) \
hv_store(ERRHV, k, strlen(k), v, FALSE)

#define MP_FETCH_ERROR(k) \
*hv_fetch(ERRHV, k, strlen(k), FALSE)

#define MP_CLEAR_ERROR(k) \
(void)hv_delete(ERRHV, k, strlen(k), G_DISCARD)


#ifndef ERRSV_CAN_BE_HTTP
#define ERRSV_CAN_BE_HTTP perl_get_sv("Apache::ERRSV_CAN_BE_HTTP", FALSE)
#endif

#ifndef PERL_DESTRUCT_LEVEL
#define PERL_DESTRUCT_LEVEL 0
#endif

#ifndef PERL_SECTIONS_SELF_BOOT
#define PERL_SECTIONS_SELF_BOOT getenv("PERL_SECTIONS_SELF_BOOT")
#endif

#ifndef DO_INTERNAL_REDIRECT
#define DO_INTERNAL_REDIRECT perl_get_sv("Apache::DoInternalRedirect", FALSE)
#endif

#ifdef PERL_RESTART_HANDLER
#undef NO_PERL_RESTART
#endif

#ifdef PERL_MARK_WHERE
#define MARK_WHERE(w,s) \
   ENTER; \
   mod_perl_mark_where(w,s)
#define UNMARK_WHERE LEAVE
#else
#define MARK_WHERE(w,s)
#define UNMARK_WHERE
#endif

typedef request_rec * Apache;
typedef request_rec * Apache__SubRequest;
typedef conn_rec    * Apache__Connection;
typedef server_rec  * Apache__Server;
typedef cmd_parms   * Apache__CmdParms;

#define GvHV_init(name) gv_fetchpv(name, GV_ADDMULTI, SVt_PVHV)
#define GvSV_init(name) gv_fetchpv(name, GV_ADDMULTI, SVt_PV)

#define GvSV_setiv(gv,val) sv_setiv(GvSV(gv), val)

#define Apache__ServerStarting(val) \
{ \
    GV *sgv = GvSV_init("Apache::ServerStarting"); \
    GvSV_setiv(sgv, val); \
}

#define Apache__ServerReStarting(val) \
{ \
    GV *sgv = GvSV_init("Apache::ServerReStarting"); \
    GvSV_setiv(sgv, val); \
    if(perl_is_running == PERL_DONE_STARTUP) \
        Apache__ServerStarting((val == FALSE ? FALSE : PERL_RUNNING())); \
}

#define PUSHif(arg) \
if(arg) \
   XPUSHs(sv_2mortal(newSVpv(arg,0)))

#define iniHV(hv) hv = (HV*)sv_2mortal((SV*)newHV())
#define iniAV(av) av = (AV*)sv_2mortal((SV*)newAV())

#define AvTRUE(av) (av && (AvFILL(av) > -1) && SvREFCNT(av))

#define av_copy_array(av) av_make(av_len(av)+1, AvARRAY(av))  

#ifndef newRV_noinc
#define newRV_noinc(sv)	((Sv = newRV(sv)), --SvREFCNT(SvRV(Sv)), Sv)
#endif

#ifndef SvTAINTED_on
#define SvTAINTED_on(sv) if (tainting) sv_magic(sv, Nullsv, 't', Nullch, 0)
#endif

#define HV_SvTAINTED_on(hv,key,klen) \
    SvTAINTED_on(*hv_fetch(hv, key, klen, 0)) 

#define mp_setenv(key, val) \
{ \
    int klen = strlen(key); \
    hv_store(GvHV(envgv), key, klen, newSVpv(val,0), FALSE); \
    HV_SvTAINTED_on(GvHV(envgv), key, klen); \
    my_setenv(key, val); \
}

#define mp_debug mod_perl_debug

extern U32	mp_debug;

#ifdef PERL_TRACE
#define MP_TRACE(a)   if (mp_debug)	a
#define MP_TRACE_d(a) if (mp_debug & 1)	a /* directives */
#define MP_TRACE_s(a) if (mp_debug & 2)	a /* perl sections */
#define MP_TRACE_h(a) if (mp_debug & 4)	a /* handlers */
#define MP_TRACE_g(a) if (mp_debug & 8)	a /* globals and allocation */
#else
#define MP_TRACE(a)
#define MP_TRACE_d(a) 
#define MP_TRACE_s(a) 
#define MP_TRACE_h(a) 
#define MP_TRACE_g(a) 
#endif

/* cut down on some noise in source */
#define dSTATUS int status = DECLINED, dstatus

#define dPPDIR \
   perl_dir_config *cld = get_module_config(r->per_dir_config, &perl_module)   

#define dPSRV(srv) \
   perl_server_config *cls = get_module_config (srv->module_config, &perl_module)

/* per-directory flags */

#define MPf_On   1
#define MPf_Off -1
#define MPf_None 0

#define MPf_INCPUSH	0x00000100 /* use lib split ":", $ENV{PERL5LIB} */
#define MPf_SENDHDR	0x00000200 /* is PerlSendHeader On? */
#define MPf_SENTHDR	0x00000400 /* has PerlSendHeader sent the headers? */
#define MPf_ENV		0x00000800 /* PerlSetupEnv */
#define MPf_HASENV	0x00001000 /* do we have any PerlSetEnv's? */
#define MPf_DSTDERR	0x00002000 /* redirect stderr to error_log */
#define MPf_CLEANUP	0x00004000 /* did we register our cleanup ? */
#define MPf_RCLEANUP	0x00008000 /* for $r->register_cleanup */

#define MP_FMERGE(new,add,base,f) \
if((add->flags & f) || (base->flags & f)) \
    new->flags |= f
    
#define MP_INCPUSH(d)    (d->flags & MPf_INCPUSH)
#define MP_INCPUSH_on(d)  (d->flags |= MPf_INCPUSH)
#define MP_INCPUSH_off(d)  (d->flags  &= ~MPf_INCPUSH)

#if 0
#define MP_SENDHDR(d)    (d->flags & MPf_SENDHDR)
#define MP_SENDHDR_on(d)  (d->flags |= MPf_SENDHDR)
#define MP_SENDHDR_off(d)  (d->flags  &= ~MPf_SENDHDR)
#endif

#define MP_SENDHDR(d)     (d->SendHeader == MPf_On)
#define MP_SENDHDR_on(d)  (d->SendHeader = MPf_On)
#define MP_SENDHDR_off(d) (d->SendHeader = MPf_Off)

#define MP_SENTHDR(d)    (d->flags & MPf_SENTHDR)
#define MP_SENTHDR_on(d)  (d->flags |= MPf_SENTHDR)
#define MP_SENTHDR_off(d)  (d->flags  &= ~MPf_SENTHDR)

#if 0
#define MP_ENV(d)       (d->flags & MPf_ENV)
#define MP_ENV_on(d)     (d->flags |= MPf_ENV)
#define MP_ENV_off(d)    (d->flags  &= ~MPf_ENV)
#endif

#define MP_ENV(d)       (d->SetupEnv == MPf_On)
#define MP_ENV_on(d)    (d->SetupEnv = MPf_On)
#define MP_ENV_off(d)   (d->SetupEnv = MPf_Off)

#define MP_HASENV(d)    (d->flags & MPf_HASENV)
#define MP_HASENV_on(d)  (d->flags |= MPf_HASENV)
#define MP_HASENV_off(d)  (d->flags  &= ~MPf_HASENV)

#define MP_DSTDERR(d)    (d->flags & MPf_DSTDERR)
#define MP_DSTDERR_on(d)  (d->flags |= MPf_DSTDERR)
#define MP_DSTDERR_off(d)  (d->flags  &= ~MPf_DSTDERR)

#define MP_CLEANUP(d)    (d->flags & MPf_CLEANUP)
#define MP_CLEANUP_on(d)  (d->flags |= MPf_CLEANUP)
#define MP_CLEANUP_off(d)  (d->flags  &= ~MPf_CLEANUP)

#define MP_RCLEANUP(d)    (d->flags & MPf_RCLEANUP)
#define MP_RCLEANUP_on(d)  (d->flags |= MPf_RCLEANUP)
#define MP_RCLEANUP_off(d)  (d->flags  &= ~MPf_RCLEANUP)

#define PERL_GATEWAY_INTERFACE "CGI-Perl/1.1"
/* Apache::SSI */
#define PERL_APACHE_SSI_TYPE "text/x-perl-server-parsed-html"
/* PerlSetVar */

#ifndef NO_PERL_DIRECTIVE_HANDLERS
#define PERL_DIRECTIVE_HANDLERS
#endif
#ifndef NO_PERL_STACKED_HANDLERS
#define PERL_STACKED_HANDLERS
#endif
#ifndef NO_PERL_METHOD_HANDLERS
#define PERL_METHOD_HANDLERS
#endif
#ifndef NO_PERL_SECTIONS
#define PERL_SECTIONS
#endif
#ifndef NO_PERL_SSI
#undef  PERL_SSI
#define PERL_SSI
#endif

#ifdef APACHE_SSL
#define PERL_DONE_STARTUP 1
#else
#define PERL_DONE_STARTUP 2
#endif

/* some 1.2.x/1.3.x compat stuff */
/* once 1.3.0 is here, we can toss most of this junk */

#if MODULE_MAGIC_NUMBER > 19970912 
#define cmd_infile   parms->config_file
#define cmd_filename parms->config_file->name
#define cmd_linenum  parms->config_file->line_number
#else
#define cmd_infile   parms->infile
#define cmd_filename parms->config_file
#define cmd_linenum  parms->config_line
#endif

#ifndef DONE
#define DONE -2
#endif

#if MODULE_MAGIC_NUMBER >= 19980413
#include "compat.h"
#endif
 
#if MODULE_MAGIC_NUMBER > 19970909

#define mod_perl_warn(s,msg) \
    aplog_error(APLOG_MARK, APLOG_WARNING | APLOG_NOERRNO, s, msg)

#define mod_perl_error(s,msg) \
    aplog_error(APLOG_MARK, APLOG_ERR | APLOG_NOERRNO, s, msg)

#define mod_perl_notice(s,msg) \
    aplog_error(APLOG_MARK, APLOG_NOERRNO|APLOG_NOTICE, s, msg)

#define mod_perl_log_reason(msg, file, r) \
    aplog_error(APLOG_MARK, APLOG_ERR | APLOG_NOERRNO, r->server, \
                "access to %s failed for %s, reason: %s", \
                file, \
                get_remote_host(r->connection, \
				r->per_dir_config, REMOTE_NAME), \
                msg)

#else

#define mod_perl_error(s,msg) log_error(msg,s)
#define mod_perl_warn   mod_perl_error
#define mod_perl_notice mod_perl_error
#define mod_perl_log_reason log_reason
#endif                    

#if MODULE_MAGIC_NUMBER < 19970719
#define is_initial_req(r) ((r->main == NULL) && (r->prev == NULL)) 
#endif

#ifndef API_EXPORT
#define API_EXPORT(type)    type
#endif

#ifndef MODULE_VAR_EXPORT
#define MODULE_VAR_EXPORT
#endif

#ifndef API_VAR_EXPORT
#define API_VAR_EXPORT
#endif

#ifdef WIN32
#if MODULE_MAGIC_NUMBER < 19980317
#undef PERL_SECTIONS
#define NO_PERL_SECTIONS
#endif
#include "multithread.h"
extern void *mod_perl_mutex;
#else
#define mod_perl_mutex NULL 
extern void *mod_perl_dummy_mutex;

#ifndef MULTITHREAD_H
typedef void mutex;
#define MULTI_OK (0)
#undef create_mutex
#undef acquire_mutex
#undef release_mutex
#define create_mutex(name)	((mutex *)mod_perl_dummy_mutex)
#define acquire_mutex(mutex_id)	((int)MULTI_OK)
#define release_mutex(mutex_id)	((int)MULTI_OK)
#endif /* MULTITHREAD_H */

#endif /* WIN32 */

#if MODULE_MAGIC_NUMBER < 19971226
char *ap_cpystrn(char *dst, const char *src, size_t dst_size);
#endif

#if MODULE_MAGIC_NUMBER >= 19980304
#ifndef SERVER_BUILT
#define SERVER_BUILT apapi_get_server_built()
#endif
#endif

#define PERL_SET_CUR_HOOK(h) \
{ \
   SV *sv = perl_get_sv("Apache::__CurrentCallback", TRUE); \
   if (sv) sv_setpv(sv, h); \
}

#ifdef PERL_STACKED_HANDLERS
#define PERL_TAKE ITERATE
#define PERL_CMD_INIT  Nullav
#define PERL_CMD_TYPE  AV

#define mod_perl_can_stack_handlers(sv) (SvTRUE(sv) && 1)

/* always enable child_init for perl_init_ids */
#if (MODULE_MAGIC_NUMBER >= 19970719) && !defined(WIN32)
#define perl_init_ids
# ifdef NO_PERL_CHILD_INIT
#  undef NO_PERL_CHILD_INIT
# endif
# ifdef NO_PERL_CHILD_EXIT
#  undef NO_PERL_CHILD_EXIT
# endif
#endif

#ifndef perl_init_ids
#define perl_init_ids mod_perl_init_ids()
#endif

#define NO_HANDLERS -666

#define PERL_CALLBACK(h,name) \
PERL_SET_CUR_HOOK(h); \
(void)acquire_mutex(mod_perl_mutex); \
if(AvTRUE(name)) { \
    status = perl_run_stacked_handlers(h, r, name); \
} \
if((status != OK) && (status != DECLINED)) { \
   MP_TRACE_h(fprintf(stderr, "%s handlers returned %d\n", h, status)); \
} \
else { \
   dstatus = perl_run_stacked_handlers(h, r, Nullav); \
   if(dstatus != NO_HANDLERS) status = dstatus; \
} \
(void)release_mutex(mod_perl_mutex); \
MP_TRACE_h(fprintf(stderr, "%s handlers returned %d\n", h, status))


#else

#define PERL_TAKE TAKE1
#define PERL_CMD_INIT  NULL
#define PERL_CMD_TYPE  char

#define mod_perl_can_stack_handlers(sv) (SvTRUE(sv) && 0)

#define PERL_CALLBACK(h,name) \
PERL_SET_CUR_HOOK(h); \
if(name != NULL) { \
    SV *sv; \
    (void)acquire_mutex(mod_perl_mutex); \
    sv = newSVpv(name,0); \
    MARK_WHERE(h, sv); \
    dstatus = status = perl_call_handler(sv, r, Nullav); \
    UNMARK_WHERE; \
    SvREFCNT_dec(sv); \
    (void)release_mutex(mod_perl_mutex); \
    MP_TRACE_h(fprintf(stderr, "perl_call %s '%s' returned: %d\n", h,name,status)); \
} \
else { \
    MP_TRACE_h(fprintf(stderr, "mod_perl: declining to handle %s, no callback defined\n", h)); \
}

#endif

#if MODULE_MAGIC_NUMBER >= 19961007
#define CHAR_P const char *
#else
#define CHAR_P char * 
#endif

/* bleh */
#if MODULE_MAGIC_NUMBER >= 19961125 
#define PERL_READ_SETUP setup_client_block(r, REQUEST_CHUNKED_ERROR); 
#else
#define PERL_READ_SETUP
#endif 

#if MODULE_MAGIC_NUMBER >= 19970622 
#define PERL_SET_READ_LENGTH  r->read_length = 0
#else
#define PERL_SET_READ_LENGTH
#endif 

#if MODULE_MAGIC_NUMBER >= 19961125 
#define PERL_READ_CLIENT \
if(should_client_block(r)) { \
    nrd = get_client_block(r, buffer, bufsiz); \
    PERL_SET_READ_LENGTH; \
} 
#else 
#define PERL_READ_CLIENT \
nrd = read_client_block(r, buffer, bufsiz); 
#endif       

#define PERL_READ_FROM_CLIENT \
PERL_READ_SETUP; \
PERL_READ_CLIENT

#define PUSHelt(key,val,klen) \
{ \
    SV *psv = (SV*)newSVpv(val, 0); \
    SvTAINTED_on(psv); \
    XPUSHs(sv_2mortal((SV*)newSVpv(key, klen))); \
    XPUSHs(sv_2mortal((SV*)psv)); \
}

/* on/off switches for callback hooks during server startup/shutdown */

#ifndef NO_PERL_DISPATCH
#define PERL_DISPATCH

#define PERL_DISPATCH_HOOK perl_dispatch

#define PERL_DISPATCH_CMD_ENTRY \
"PerlDispatchHandler", perl_cmd_dispatch_handlers, \
    NULL, \
    OR_ALL, TAKE1, "the Perl Dispatch handler routine name"

#define PERL_DISPATCH_CREATE(s) s->PerlDispatchHandler = NULL
#else
#define PERL_DISPATCH_HOOK NULL
#define PERL_DISPATCH_CMD_ENTRY NULL
#define PERL_DISPATCH_CREATE(s)
#endif

#ifndef NO_PERL_CHILD_INIT
#define PERL_CHILD_INIT

#define PERL_CHILD_INIT_HOOK perl_child_init

#define PERL_CHILD_INIT_CMD_ENTRY \
"PerlChildInitHandler", perl_cmd_child_init_handlers, \
    NULL,	 \
    RSRC_CONF, PERL_TAKE, "the Perl Child init handler routine name"  

#define PERL_CHILD_INIT_CREATE(s) s->PerlChildInitHandler = PERL_CMD_INIT
#else
#define PERL_CHILD_INIT_HOOK NULL
#define PERL_CHILD_INIT_CMD_ENTRY NULL
#define PERL_CHILD_INIT_CREATE(s) 
#endif

#ifndef NO_PERL_CHILD_EXIT
#define PERL_CHILD_EXIT

#define PERL_CHILD_EXIT_HOOK perl_child_exit

#define PERL_CHILD_EXIT_CMD_ENTRY \
"PerlChildExitHandler", perl_cmd_child_exit_handlers, \
    NULL,	 \
    RSRC_CONF, PERL_TAKE, "the Perl Child exit handler routine name"  

#define PERL_CHILD_EXIT_CREATE(s) s->PerlChildExitHandler = PERL_CMD_INIT
#else
#define PERL_CHILD_EXIT_HOOK NULL
#define PERL_CHILD_EXIT_CMD_ENTRY NULL
#define PERL_CHILD_EXIT_CREATE(s) 
#endif

#ifndef NO_PERL_RESTART
#define PERL_RESTART

#define PERL_RESTART_CMD_ENTRY \
"PerlRestartHandler", perl_cmd_restart_handlers, \
    NULL,	 \
    RSRC_CONF, PERL_TAKE, "the Perl Restart handler routine name"  

#define PERL_RESTART_CREATE(s) s->PerlRestartHandler = PERL_CMD_INIT
#else

#define PERL_RESTART_CMD_ENTRY NULL
#define PERL_RESTART_CREATE(s) 
#endif

/* on/off switches for callback hooks during request stages */

#ifndef NO_PERL_POST_READ_REQUEST
#define PERL_POST_READ_REQUEST

#define PERL_POST_READ_REQUEST_HOOK perl_post_read_request

#define PERL_POST_READ_REQUEST_CMD_ENTRY \
"PerlPostReadRequestHandler", perl_cmd_post_read_request_handlers, \
    NULL, \
    RSRC_CONF, PERL_TAKE, "the Perl Post Read Request handler routine name" 

#define PERL_POST_READ_REQUEST_CREATE(s) s->PerlPostReadRequestHandler = PERL_CMD_INIT
#else
#define PERL_POST_READ_REQUEST_HOOK NULL
#define PERL_POST_READ_REQUEST_CMD_ENTRY NULL
#define PERL_POST_READ_REQUEST_CREATE(s)
#endif

#ifndef NO_PERL_TRANS
#define PERL_TRANS

#define PERL_TRANS_HOOK perl_translate

#define PERL_TRANS_CMD_ENTRY \
"PerlTransHandler", perl_cmd_trans_handlers, \
    NULL,	 \
    RSRC_CONF, PERL_TAKE, "the Perl Translation handler routine name"  

#define PERL_TRANS_CREATE(s) s->PerlTransHandler = PERL_CMD_INIT
#else
#define PERL_TRANS_HOOK NULL
#define PERL_TRANS_CMD_ENTRY NULL
#define PERL_TRANS_CREATE(s) 
#endif


#ifndef NO_PERL_AUTHEN
#define PERL_AUTHEN

#define PERL_AUTHEN_HOOK perl_authenticate

#define PERL_AUTHEN_CMD_ENTRY \
"PerlAuthenHandler", perl_cmd_authen_handlers, \
    NULL, \
    OR_ALL, PERL_TAKE, "the Perl Authentication handler routine name"

#define PERL_AUTHEN_CREATE(s) s->PerlAuthenHandler = PERL_CMD_INIT
#else
#define PERL_AUTHEN_HOOK NULL
#define PERL_AUTHEN_CMD_ENTRY NULL
#define PERL_AUTHEN_CREATE(s)
#endif

#ifndef NO_PERL_AUTHZ
#define PERL_AUTHZ

#define PERL_AUTHZ_HOOK perl_authorize

#define PERL_AUTHZ_CMD_ENTRY \
"PerlAuthzHandler", perl_cmd_authz_handlers, \
    NULL, \
    OR_ALL, PERL_TAKE, "the Perl Authorization handler routine name" 
#define PERL_AUTHZ_CREATE(s) s->PerlAuthzHandler = PERL_CMD_INIT
#else
#define PERL_AUTHZ_HOOK NULL
#define PERL_AUTHZ_CMD_ENTRY NULL
#define PERL_AUTHZ_CREATE(s)
#endif

#ifndef NO_PERL_ACCESS
#define PERL_ACCESS

#define PERL_ACCESS_HOOK perl_access

#define PERL_ACCESS_CMD_ENTRY \
"PerlAccessHandler", perl_cmd_access_handlers, \
    NULL, \
    OR_ALL, PERL_TAKE, "the Perl Access handler routine name" 

#define PERL_ACCESS_CREATE(s) s->PerlAccessHandler = PERL_CMD_INIT
#else
#define PERL_ACCESS_HOOK NULL
#define PERL_ACCESS_CMD_ENTRY NULL
#define PERL_ACCESS_CREATE(s)
#endif

/* un-tested hooks */

#ifndef NO_PERL_TYPE
#define PERL_TYPE

#define PERL_TYPE_HOOK perl_type_checker

#define PERL_TYPE_CMD_ENTRY \
"PerlTypeHandler", perl_cmd_type_handlers, \
    NULL, \
    OR_ALL, PERL_TAKE, "the Perl Type check handler routine name" 

#define PERL_TYPE_CREATE(s) s->PerlTypeHandler = PERL_CMD_INIT
#else
#define PERL_TYPE_HOOK NULL
#define PERL_TYPE_CMD_ENTRY NULL
#define PERL_TYPE_CREATE(s) 
#endif

#ifndef NO_PERL_FIXUP
#define PERL_FIXUP

#define PERL_FIXUP_HOOK perl_fixup

#define PERL_FIXUP_CMD_ENTRY \
"PerlFixupHandler", perl_cmd_fixup_handlers, \
    NULL, \
    OR_ALL, PERL_TAKE, "the Perl Fixup handler routine name" 

#define PERL_FIXUP_CREATE(s) s->PerlFixupHandler = PERL_CMD_INIT
#else
#define PERL_FIXUP_HOOK NULL
#define PERL_FIXUP_CMD_ENTRY NULL
#define PERL_FIXUP_CREATE(s)
#endif

#ifndef NO_PERL_LOG
#define PERL_LOG

#define PERL_LOG_HOOK perl_logger

#define PERL_LOG_CMD_ENTRY \
"PerlLogHandler", perl_cmd_log_handlers, \
    NULL, \
    OR_ALL, PERL_TAKE, "the Perl Log handler routine name" 

#define PERL_LOG_CREATE(s) s->PerlLogHandler = PERL_CMD_INIT
#else
#define PERL_LOG_HOOK NULL
#define PERL_LOG_CMD_ENTRY NULL
#define PERL_LOG_CREATE(s) 
#endif

#ifndef NO_PERL_CLEANUP
#define PERL_CLEANUP

#define PERL_CLEANUP_HOOK perl_cleanup

#define PERL_CLEANUP_CMD_ENTRY \
"PerlCleanupHandler", perl_cmd_cleanup_handlers, \
    NULL, \
    OR_ALL, PERL_TAKE, "the Perl Cleanup handler routine name" 

#define PERL_CLEANUP_CREATE(s) s->PerlCleanupHandler = PERL_CMD_INIT
#else
#define PERL_CLEANUP_HOOK NULL
#define PERL_CLEANUP_CMD_ENTRY NULL
#define PERL_CLEANUP_CREATE(s)
#endif

#ifndef NO_PERL_INIT
#define PERL_INIT

#define PERL_INIT_HOOK perl_init

#define PERL_INIT_CMD_ENTRY \
"PerlInitHandler", perl_cmd_init_handlers, \
    NULL, \
    OR_ALL, PERL_TAKE, "the Perl Init handler routine name" 

#define PERL_INIT_CREATE(s) s->PerlInitHandler = PERL_CMD_INIT
#else
#define PERL_INIT_HOOK NULL
#define PERL_INIT_CMD_ENTRY NULL
#define PERL_INIT_CREATE(s) 
#endif

#ifndef NO_PERL_HEADER_PARSER
#define PERL_HEADER_PARSER

#define PERL_HEADER_PARSER_HOOK perl_header_parser

#define PERL_HEADER_PARSER_CMD_ENTRY \
"PerlHeaderParserHandler", perl_cmd_header_parser_handlers, \
    NULL, \
    OR_ALL, PERL_TAKE, "the Perl Header Parser handler routine name" 

#define PERL_HEADER_PARSER_CREATE(s) s->PerlHeaderParserHandler = PERL_CMD_INIT
#else
#define PERL_HEADER_PARSER_HOOK NULL
#define PERL_HEADER_PARSER_CMD_ENTRY NULL
#define PERL_HEADER_PARSER_CREATE(s)
#endif

typedef struct {
    array_header *PerlPassEnv;
    array_header *PerlRequire;
    array_header *PerlModule;
    int PerlTaintCheck;
    int PerlWarn;
    int FreshRestart;
    PERL_CMD_TYPE *PerlPostReadRequestHandler;
    PERL_CMD_TYPE *PerlTransHandler;
    PERL_CMD_TYPE *PerlChildInitHandler;
    PERL_CMD_TYPE *PerlChildExitHandler;
    PERL_CMD_TYPE *PerlRestartHandler;
} perl_server_config;

typedef struct {
    char *PerlDispatchHandler;
    PERL_CMD_TYPE *PerlHandler;
    PERL_CMD_TYPE *PerlAuthenHandler;
    PERL_CMD_TYPE *PerlAuthzHandler;
    PERL_CMD_TYPE *PerlAccessHandler;
    PERL_CMD_TYPE *PerlTypeHandler;
    PERL_CMD_TYPE *PerlFixupHandler;
    PERL_CMD_TYPE *PerlLogHandler;
    PERL_CMD_TYPE *PerlCleanupHandler;
    PERL_CMD_TYPE *PerlHeaderParserHandler;
    PERL_CMD_TYPE *PerlInitHandler;
    table *env;
    table *vars;
    U32 flags;
    int SendHeader;
    int SetupEnv;
} perl_dir_config;

typedef struct {
    int is_method;
    int is_anon;
    int in_perl;
    SV *class;
    char *method;
} mod_perl_handler;

typedef struct {
    SV *obj;
    char *class;
} mod_perl_perl_dir_config;

typedef struct {
    char *subname;
    char *info;
} mod_perl_cmd_info;

extern module MODULE_VAR_EXPORT perl_module;

/* a couple for -Wall sanity sake */
int translate_name (request_rec *);
int log_transaction (request_rec *r);

/* mod_perl prototypes */

/* perlxsi.c */
void xs_init (void);

/* mod_perl.c */

/* generic handler stuff */ 
int perl_handler_ismethod(HV *class, char *sub);
int perl_call_handler(SV *sv, request_rec *r, AV *args);

/* stacked handler stuff */
int mod_perl_push_handlers(SV *self, char *hook, SV *sub, AV *handlers);
SV *mod_perl_pop_handlers(SV *self, SV *hook);
void *mod_perl_clear_handlers(SV *self, SV *hook);
SV *mod_perl_fetch_handlers(SV *self, SV *hook);
int perl_run_stacked_handlers(char *hook, request_rec *r, AV *handlers);

/* plugin slots */
void perl_startup(server_rec *s, pool *p);
int perl_handler(request_rec *r);
void perl_child_init(server_rec *, pool *);
void perl_child_exit(server_rec *, pool *);
int perl_translate(request_rec *r);
int perl_authenticate(request_rec *r);
int perl_authorize(request_rec *r);
int perl_access(request_rec *r);
int perl_type_checker(request_rec *r);
int perl_fixup(request_rec *r);
int perl_post_read_request(request_rec *r);
int perl_logger(request_rec *r);
int perl_header_parser(request_rec *r);
int perl_hook(char *name);
int PERL_RUNNING(void);

/* per-request gunk */
int mod_perl_sent_header(request_rec *r, int val);
int mod_perl_seqno(SV *self, int inc);
request_rec *perl_request_rec(request_rec *);
void perl_setup_env(request_rec *r);
SV  *perl_bless_request_rec(request_rec *); 
void perl_set_request_rec(request_rec *); 
void mod_perl_cleanup_handler(void *data);
void mod_perl_end_cleanup(void *data);
void mod_perl_register_cleanup(request_rec *r, SV *sv);
void mod_perl_noop(void *data);
SV *mod_perl_resolve_handler(request_rec *r, SV *sv, mod_perl_handler *h); 
mod_perl_handler *mod_perl_new_handler(request_rec *r, SV *sv);
void mod_perl_destroy_handler(void *data);

/* perl_util.c */

SV *array_header2avrv(array_header *arr);
array_header *avrv2array_header(SV *avrv, pool *p);
SV *mod_perl_gensym (char *pack);
void perl_tie_hash(HV *hv, char *class);
void perl_util_cleanup(void);
void mod_perl_clear_rgy_endav(request_rec *r, SV *sv);
void perl_run_rgy_endav(char *s);
void perl_run_endav(char *s);
void perl_call_halt(int status);
CV *empty_anon_sub(void);
void perl_reload_inc(void);
I32 perl_module_is_loaded(char *name);
SV *perl_module2file(char *name);
int perl_require_module(char *module, server_rec *s);
int perl_load_startup_script(server_rec *s, pool *p, char *script, I32 my_warn);
array_header *perl_cgi_env_init(request_rec *r);
void perl_clear_env(void);
void mod_perl_init_ids(void);
int perl_eval_ok(server_rec *s);
int perl_sv_is_http_code(SV *sv, int *status);
void perl_incpush(char *s);
SV *mod_perl_sv_name(SV *svp);
void mod_perl_mark_where(char *where, SV *sub);

/* perlio.c */

void perl_soak_script_output(request_rec *r);
void perl_stdin2client(request_rec *r);
void perl_stdout2client(request_rec *r); 

/* perl_config.c */

char *mod_perl_auth_name(request_rec *r, char *val);

void *perl_merge_dir_config(pool *p, void *basev, void *addv);
void *perl_create_dir_config(pool *p, char *dirname);
void *perl_create_server_config(pool *p, server_rec *s);

void perl_section_self_boot(cmd_parms *parms, void *dummy, const char *arg);
CHAR_P perl_section (cmd_parms *cmd, void *dummy, CHAR_P arg);
CHAR_P perl_end_section (cmd_parms *cmd, void *dummy);
CHAR_P perl_pod_section (cmd_parms *cmd, void *dummy, CHAR_P arg);
CHAR_P perl_pod_end_section (cmd_parms *cmd, void *dummy);
CHAR_P perl_cmd_autoload (cmd_parms *parms, void *dummy, const char *arg);
CHAR_P perl_config_END (cmd_parms *cmd, void *dummy, CHAR_P arg);
CHAR_P perl_limit_section(cmd_parms *cmd, void *dummy, HV *hv);
CHAR_P perl_urlsection (cmd_parms *cmd, void *dummy, HV *hv);
CHAR_P perl_dirsection (cmd_parms *cmd, void *dummy, HV *hv);
CHAR_P perl_filesection (cmd_parms *cmd, void *dummy, HV *hv);
void perl_add_file_conf (server_rec *s, void *url_config);
void perl_handle_command(cmd_parms *cmd, void *dummy, char *line);
void perl_handle_command_hv(HV *hv, char *key, cmd_parms *cmd, void *dummy);
void perl_handle_command_av(AV *av, I32 n, char *key, cmd_parms *cmd, void *dummy);

CHAR_P perl_cmd_require (cmd_parms *parms, void *dummy, char *arg);
CHAR_P perl_cmd_module (cmd_parms *parms, void *dummy, char *arg);
CHAR_P perl_cmd_var(cmd_parms *cmd, perl_dir_config *rec, char *key, char *val);
CHAR_P perl_cmd_setenv(cmd_parms *cmd, perl_dir_config *rec, char *key, char *val);
CHAR_P perl_cmd_env (cmd_parms *cmd, perl_dir_config *rec, int arg);
CHAR_P perl_cmd_pass_env (cmd_parms *parms, void *dummy, char *arg);
CHAR_P perl_cmd_sendheader (cmd_parms *cmd, perl_dir_config *rec, int arg);
CHAR_P perl_cmd_tainting (cmd_parms *parms, void *dummy, int arg);
CHAR_P perl_cmd_warn (cmd_parms *parms, void *dummy, int arg);
CHAR_P perl_cmd_fresh_restart (cmd_parms *parms, void *dummy, int arg);

CHAR_P perl_cmd_dispatch_handlers (cmd_parms *parms, perl_dir_config *rec, char *arg);
CHAR_P perl_cmd_init_handlers (cmd_parms *parms, perl_dir_config *rec, char *arg);
CHAR_P perl_cmd_cleanup_handlers (cmd_parms *parms, perl_dir_config *rec, char *arg);
CHAR_P perl_cmd_header_parser_handlers (cmd_parms *parms, perl_dir_config *rec, char *arg);
CHAR_P perl_cmd_post_read_request_handlers (cmd_parms *parms, void *dumm, char *arg);
CHAR_P perl_cmd_trans_handlers (cmd_parms *parms, void *dumm, char *arg);
CHAR_P perl_cmd_child_init_handlers (cmd_parms *parms, void *dumm, char *arg);
CHAR_P perl_cmd_child_exit_handlers (cmd_parms *parms, void *dumm, char *arg);
CHAR_P perl_cmd_restart_handlers (cmd_parms *parms, void *dumm, char *arg);
CHAR_P perl_cmd_authen_handlers (cmd_parms *parms, perl_dir_config *rec, char *arg);
CHAR_P perl_cmd_authz_handlers (cmd_parms *parms, perl_dir_config *rec, char *arg);
CHAR_P perl_cmd_access_handlers (cmd_parms *parms, perl_dir_config *rec, char *arg);
CHAR_P perl_cmd_type_handlers (cmd_parms *parms, perl_dir_config *rec, char *arg);
CHAR_P perl_cmd_fixup_handlers (cmd_parms *parms, perl_dir_config *rec, char *arg);
CHAR_P perl_cmd_handler_handlers (cmd_parms *parms, perl_dir_config *rec, char *arg);
CHAR_P perl_cmd_log_handlers (cmd_parms *parms, perl_dir_config *rec, char *arg);
CHAR_P perl_cmd_perl_TAKE1(cmd_parms *cmd, mod_perl_perl_dir_config *d, char *one);
CHAR_P perl_cmd_perl_TAKE2(cmd_parms *cmd, mod_perl_perl_dir_config *d, char *one, char *two);
CHAR_P perl_cmd_perl_TAKE123(cmd_parms *cmd, mod_perl_perl_dir_config *d,
			     char *one, char *two, char *three);

#define perl_cmd_perl_RAW_ARGS perl_cmd_perl_TAKE1
#define perl_cmd_perl_NO_ARGS perl_cmd_perl_TAKE1
#define perl_cmd_perl_ITERATE perl_cmd_perl_TAKE1
#define perl_cmd_perl_ITERATE2 perl_cmd_perl_TAKE2
#define perl_cmd_perl_TAKE12 perl_cmd_perl_TAKE2
#define perl_cmd_perl_TAKE23 perl_cmd_perl_TAKE123
#define perl_cmd_perl_TAKE3 perl_cmd_perl_TAKE123

void mod_perl_dir_env(perl_dir_config *cld);
void mod_perl_pass_env(pool *p, perl_server_config *cls);

/* Apache.xs */

pool *perl_get_startup_pool(void);
server_rec *perl_get_startup_server(void);
