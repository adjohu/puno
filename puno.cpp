/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2010 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Rotsen Marcello <rotsen.marcello@wstech2.net>                |
  +----------------------------------------------------------------------+
*/

#include <php_puno.h>
#include "puno_class.cpp"
#include "puno_converters.cpp"
#include "puno_functions.cpp"


/* {{{ puno_functions[]
 *
 * Every user visible function must have an entry in puno_functions[].
 */

function_entry puno_functions[] = {
	ZEND_FE(get_remote_xcomponent, NULL)
	ZEND_FE(create_struct, NULL)
	{	NULL, NULL, NULL}
};

/* {{{ puno_module_entry
 */
zend_module_entry puno_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
		STANDARD_MODULE_HEADER,
#endif
		"puno",
		puno_functions,
		PHP_MINIT(puno),
		PHP_MSHUTDOWN(puno),
		PHP_RINIT(puno),
		PHP_RSHUTDOWN(puno),
		PHP_MINFO(puno),
#if ZEND_MODULE_API_NO >= 20010901
		"0.9",
#endif
		STANDARD_MODULE_PROPERTIES };
/* }}} */

#ifdef COMPILE_DL_PUNO
BEGIN_EXTERN_C()
ZEND_GET_MODULE(puno)
END_EXTERN_C()
#endif

/* {{{ php_puno_init_globals
 */
/* Uncomment this function if you have INI entries*/
static void php_puno_init_globals(zend_puno_globals *puno_globals) {
	puno_globals->x_idl_reflec_rsrc_id = 0;
	puno_globals->remote_service_factory_rsrc_id = 0;
	puno_globals->x_type_conv_rsrc_id = 0;
}




/* vinculação de funções aos métodos da classe */
function_entry puno_class_functions[] = { { NULL, NULL, NULL } };

/* Remove if there's nothing to do at module start */
/* {{{ PHP_MINIT_FUNCTION
 */

PHP_MINIT_FUNCTION(puno)
{
	zval *tmp;

	ZEND_INIT_MODULE_GLOBALS(puno, php_puno_init_globals, NULL);

	zend_class_entry puno_ce;
	INIT_CLASS_ENTRY(puno_ce,"puno_class",puno_class_functions);
	ce_ptr=zend_register_internal_class(&puno_ce TSRMLS_CC);

	ce_ptr->create_object=puno_class_object_new;
	memcpy(&puno_class_handlers,zend_get_std_object_handlers(),sizeof(zend_object_handlers));
	puno_class_handlers.clone_obj=NULL;

	puno_class_handlers.get_method=puno_class_get_method;
	puno_class_handlers.call_method=puno_class_call_method;
	puno_class_handlers.write_property=puno_class_write_property;
	puno_class_handlers.read_property=puno_class_read_property;
	puno_class_handlers.get_property_ptr_ptr=NULL;

	/* OpenOffice Classes and Interface init functions */
	uno_refer_rsrc_dtor=zend_register_list_destructors_ex(puno_refer_rsrc_dtor_hdlr, NULL,"Reference_resource_destructor", module_number);
	uno_any_rsrc_dtor=zend_register_list_destructors_ex(puno_any_rsrc_dtor_hdlr, NULL,"Any_resource_destructor", module_number);
	x_compnt_fact_cli_rsrc_dtor=zend_register_list_destructors_ex(x_compnt_fact_cli_rsrc_dtor_hdlr, NULL,"XMultiComponentFactoryClient_resource_destructor", module_number);
	x_simple_reg_rsrc_dtor=zend_register_list_destructors_ex(x_simple_reg_rsrc_dtor_hdlr, NULL,"XSimpleRegistry_resource_destructor", module_number);



	//register constants
	#include "puno_constants.c"
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(puno)
{

	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */


/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(puno)
{
	PUNO_G(debug_mode)=0;
	PUNO_DEBUG("PHP_RINIT_FUNCTION(puno) - %d<BR>", time(NULL));
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(puno)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(puno)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "OpenOffice.org UNO API support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	 DISPLAY_INI_ENTRIES();
	 */
}
/* }}} */

