#ifndef MODPERL_CONFIG_H
#define MODPERL_CONFIG_H

void *modperl_create_dir_config(ap_pool_t *p, char *dir);

void *modperl_merge_dir_config(ap_pool_t *p, void *base, void *add);

modperl_srv_config_t *modperl_srv_config_new(ap_pool_t *p);

void *modperl_create_srv_config(ap_pool_t *p, server_rec *s);

void *modperl_merge_srv_config(ap_pool_t *p, void *basev, void *addv);

char *modperl_cmd_push_handlers(MpAV *handlers, char *name, ap_pool_t *p);

#define MP_DECLARE_SRV_CMD(item) \
const char *modperl_cmd_##item(cmd_parms *parms, \
                               void *dummy, char *arg)
MP_DECLARE_SRV_CMD(trace);

MP_DECLARE_SRV_CMD(interp_start);
MP_DECLARE_SRV_CMD(interp_max);
MP_DECLARE_SRV_CMD(interp_max_spare);
MP_DECLARE_SRV_CMD(interp_min_spare);

#define MP_SRV_CMD_TAKE1(name, item, desc) \
    { name, modperl_cmd_##item, NULL, \
      RSRC_CONF, TAKE1, desc }

#define MP_dRCFG \
   modperl_request_config_t *rcfg = \
      (modperl_request_config_t *) \
          ap_get_module_config(r->request_config, &perl_module)

#define MP_dDCFG \
   modperl_dir_config_t *dcfg = \
      (modperl_dir_config_t *) \
          ap_get_module_config(r->per_dir_config, &perl_module)   

#define MP_dSCFG(s) \
   modperl_srv_config_t *scfg = \
      (modperl_srv_config_t *) \
          ap_get_module_config(s->module_config, &perl_module)

#endif /* MODPERL_CONFIG_H */
