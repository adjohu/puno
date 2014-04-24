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


// this function is called when the resource which contains the
// reference to the instance of the object XController is destroyed
void puno_refer_rsrc_dtor_hdlr (zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	Reference < XInterface > *x_iface_p = (Reference < XInterface > *)rsrc->ptr;
	if (x_iface_p)
	{

		(*x_iface_p).clear();
		delete x_iface_p;
	}
}
void puno_any_rsrc_dtor_hdlr (zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	Any *any_p = (Any *)rsrc->ptr;
	if (any_p)
	{
		(*any_p).clear();
		delete any_p;
	}
}

void x_compnt_fact_cli_rsrc_dtor_hdlr (zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	Reference < XInterface > *x_iface = (Reference < XInterface > *)rsrc->ptr;
	if (x_iface)
	{
		Reference< XComponent >::query( *x_iface )->dispose();
		(*x_iface).clear();
		delete x_iface;
	}
}

void x_simple_reg_rsrc_dtor_hdlr (zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	Reference < XSimpleRegistry > *x_simple_registry = (Reference < XSimpleRegistry > *)rsrc->ptr;
	if (x_simple_registry)
	{
		(*x_simple_registry)->close();
		(*x_simple_registry).clear();
		delete x_simple_registry;
	}

	/*
			 * x_simple_registry->close();
					x_simple_registry.clear();
					x_component_ctx.clear();*/
}



int puno_class_call_method(char *method, INTERNAL_FUNCTION_PARAMETERS) {
	int nargs;
	int type;
	com::sun::star::uno::Type any_type;
	InvocationInfo method_info;
	zval ***args = NULL;
	zval *this_instance_zval_p = getThis();
	php_set_error_handling(EH_THROW, zend_exception_get_default() TSRMLS_CC);
	php_set_error_handling(EH_NORMAL, NULL TSRMLS_CC);
	puno_class_object* this_instance_p;
	Reference < XInvocation2 > *x_invoc2_p;
	Reference < XTypeConverter > *x_type_conv_p;

	nargs = ZEND_NUM_ARGS();
	Sequence < com::sun::star::uno::Any > aParams(nargs);
	Sequence<short> aOutParamIndex(nargs);
	Sequence < com::sun::star::uno::Any > aOutParam(nargs);

	//restore the XInvocation2 resource
this_instance_p	= ( puno_class_object * ) zend_object_store_get_object( getThis() TSRMLS_CC);
	TEST_PTR(this_instance_p, FAILURE);

	x_invoc2_p=( Reference <XInvocation2> *) zend_list_find(this_instance_p->x_invoc_rsrc_id,&type);

	if (type != uno_refer_rsrc_dtor)
	{
		zend_throw_exception(zend_exception_get_default(),"Invalid XInvocation2 reference.",0 TSRMLS_CC);
		return FAILURE;
	}
	method_info=(*x_invoc2_p)->getInfoForName(OUString::createFromAscii(method),true);
	if (nargs)
	{
		args = (zval ***)safe_emalloc(sizeof(zval *), nargs, 0);
		TEST_PTR(args, FAILURE);
		zend_get_parameters_array_ex(nargs, args);
		for(int i=0;i<nargs;i++)
		{
			aParams[i]=create_any_from_zval(args[i] TSRMLS_CC);
		}

	}

	//do XInvocation2->invoke
	try
	{
		Any anyval;
		anyval=(*x_invoc2_p)->invoke(method_info.aName,aParams,aOutParamIndex,aOutParam);

		if(anyval.hasValue())
		{
			zval *retval = create_zval_from_any(anyval TSRMLS_CC );
			memcpy(return_value,retval,sizeof(zval));
			efree(retval);
		}
		else
		{
			ZVAL_NULL(return_value);
		}
	}
	catch(Exception& e)
	{
		//throw PHP EXCEPTION
		zend_throw_exception(zend_exception_get_default(),(char *)OUStringToOString(e.Message, RTL_TEXTENCODING_ASCII_US).getStr(),0 TSRMLS_CC);
	}

	//free arguments zval
	if (args)
	{
		efree(args);
	}

	//return
	return SUCCESS;
}


/* puno class destructor */
static void puno_class_object_dtor(void *object TSRMLS_DC)
{
	puno_class_object *intern = (puno_class_object *) object;
	zend_hash_destroy(intern->std.properties);
	FREE_HASHTABLE(intern->std.properties);
	efree(object);
}

/* class constructor - extended */

static zend_object_value puno_class_object_new_ex(zend_class_entry *class_type, puno_class_object **new_instance_pp TSRMLS_DC)
{
	zend_object_value retval;
	zval *tmp;
	puno_class_object *intern = (puno_class_object *) emalloc(sizeof(puno_class_object));
	memset(intern, 0, sizeof(puno_class_object));
	intern->std.ce = class_type;
	*new_instance_pp=intern;

	ALLOC_HASHTABLE(intern->std.properties);
	zend_hash_init(intern->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);

	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	retval.handle = zend_objects_store_put(intern, NULL, puno_class_object_dtor, NULL TSRMLS_CC);

	retval.handlers = &puno_class_handlers;

	return retval;
}

/* class constructor */
static zend_object_value puno_class_object_new(zend_class_entry *class_type TSRMLS_DC)
{
	puno_class_object *new_instance_p;
	return puno_class_object_new_ex(class_type, &new_instance_p TSRMLS_CC);
}

//read the property content to the PHP user space.
zval *puno_class_read_property(zval *object, zval *member, int t TSRMLS_DC)
{
	zval *return_value;

	int type;
	puno_class_object *this_instance_p;
	Any *prop_obj_p;
	Any prop_value;
	Reference <XIdlClass> *x_idl_class;

	MAKE_STD_ZVAL(return_value);
	try
	{
		this_instance_p = (puno_class_object *)zend_objects_get_address(object TSRMLS_CC);
		TEST_PTR(this_instance_p,return_value);

		//restore XIdlClass
		x_idl_class =
		(Reference <XIdlClass> *) zend_list_find(
				this_instance_p->x_idl_class_rsrc_id,&type);
		TEST_PTR(x_idl_class,return_value);

		//restore Any that represents the property
		prop_obj_p =
		(Any *) zend_list_find(this_instance_p->this_rsrc_id,&type);

		//create the field interface
		Reference <XIdlField> x_idl_field = (*x_idl_class)->getField(OUString::createFromAscii(member->value.str.val));
		TEST_PTR(x_idl_field.is(),return_value);
		Reference <XIdlField2> x_idl_field2 = Reference <XIdlField2>(x_idl_field,UNO_QUERY);
		TEST_PTR(x_idl_field2.is(),return_value);
		prop_value=x_idl_field2->get(*prop_obj_p);

		x_idl_field.clear();
		x_idl_field2.clear();
		//create Zval from the Property Any Value
		zval *prop_zval=create_zval_from_any(prop_value TSRMLS_CC);
		memcpy(return_value,prop_zval,sizeof(zval));
		efree(prop_zval);
	}
	catch(Exception& e)
	{
		ZVAL_NULL(return_value);
		//throw PHP EXCEPTION
		zend_throw_exception(zend_exception_get_default(),(char *)OUStringToOString(e.Message, RTL_TEXTENCODING_ASCII_US).getStr(),0 TSRMLS_CC);

	}
	return return_value;
}

void puno_class_write_property(zval *object, zval *member, zval *value TSRMLS_DC)
{
	zval tmp_member;
	int type;
	puno_class_object *this_instance_p;
	Any *prop_obj_p;
	Any *prop_value_p;
	Reference <XIdlClass> *x_idl_class;
	Reference<XTypeConverter> *x_type_conv_p;
	Reference< XInvocation2 > *x_invoc2_p;
	com::sun::star::uno::Type any_type;

	try
	{
		this_instance_p = (puno_class_object *)zend_objects_get_address(object TSRMLS_CC);
		TEST_PTR(this_instance_p,);
		if(value->type==IS_NULL)
		{
			return;
		}

		//restore XIdlClass
		x_idl_class =
		(Reference <XIdlClass> *) zend_list_find(
				this_instance_p->x_idl_class_rsrc_id,&type);
		TEST_PTR(x_idl_class,);
		//restore Any that represents the property
		prop_obj_p =
		(Any *) zend_list_find(this_instance_p->this_rsrc_id,&type);
		TEST_PTR(prop_obj_p,);

		//create the field interface
		Reference <XIdlField> x_idl_field = (*x_idl_class)->getField(OUString::createFromAscii(member->value.str.val));
		TEST_PTR(x_idl_field.is(),);

		Reference <XIdlField2> x_idl_field2 = Reference <XIdlField2>(x_idl_field,UNO_QUERY);
		TEST_PTR(x_idl_field2.is(),);

		x_type_conv_p=(Reference<XTypeConverter> *)zend_list_find(PUNO_G(x_type_conv_rsrc_id),&type);
		TEST_PTR(x_type_conv_p,);

		//get expected attribute type
		any_type=Type(x_idl_field2->getType()->getTypeClass(),x_idl_field2->getType()->getName());

		//create Any from the Value
		prop_value_p=new Any(
				(*x_type_conv_p)->convertTo(
						create_any_from_zval(&value TSRMLS_CC),
						any_type)
		);

		x_idl_field2->set(*prop_obj_p,*prop_value_p);
		//register the var as a resource, so it will get destroyed when the script ends
		ZEND_REGISTER_RESOURCE (NULL, prop_value_p,uno_any_rsrc_dtor);
	}

	catch(Exception& e)
	{
		//throw PHP EXCEPTION
		zend_throw_exception(zend_exception_get_default(),(char *)OUStringToOString(e.Message, RTL_TEXTENCODING_ASCII_US).getStr(),0 TSRMLS_CC);
	}
}

static union _zend_function *puno_class_get_method(zval **object, char *name, int len TSRMLS_DC)
{
	zend_internal_function func, *func_p = NULL;
	union _zend_function *func_desc_p;
	func.type = ZEND_OVERLOADED_FUNCTION;
	func.num_args = 0;
	func.arg_info = NULL;
	func.fn_flags = 0;

	func.function_name = estrndup(name, len);
	//f.handler = puno_call_method);
	func_p = &func;

	if (func_p)
	{
		/* duplicate this into a new chunk of emalloc'd memory,
		 * since the engine will efree it */
		func_desc_p = (union _zend_function *) emalloc(sizeof(*func_p));
		memcpy(func_desc_p, func_p, sizeof(*func_p));
		return func_desc_p;
	}

}

