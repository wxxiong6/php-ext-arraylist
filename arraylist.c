/*
  +----------------------------------------------------------------------+
  | PHP Version 7   arraylist                                            |
  +----------------------------------------------------------------------+
  | Copyright (c) 2019-2023 Wxxiong                                      |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Wxxiong <wxxiong6@gmail.com>                                 |
  |                                                                      |
  +----------------------------------------------------------------------+
*/


#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "ext/standard/php_var.h"
#include "zend_exceptions.h"
#include "php_arraylist.h"
#include "zend_interfaces.h"

#ifndef ARRAY_LIST_SIZE
#define ARRAY_LIST_SIZE  8
#endif

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

zend_class_entry *array_list_ce;
zend_object_handlers handler_array_list;

typedef struct _arraylist { /* {{{ */
	size_t nSize;     
 	size_t nNumUsed;  
	size_t nNextIndex;   
	zval *elements;  
} arraylist;
/* }}} */

typedef struct _arraylist_object { /* {{{ */
	arraylist             array;
	zend_object           std;
	int                   current;
	int                   flags;
} arraylist_object;
/* }}} */




static inline arraylist_object *arraylist_from_obj(zend_object *obj) /* {{{ */ {
	return (arraylist_object*)((char*)(obj) - XtOffsetOf(arraylist_object, std));
}
/* }}} */

#define Z_ARRAYLIST_P(zv)  arraylist_from_obj(Z_OBJ_P((zv)))


/* {{{ arginfo
 */
ZEND_BEGIN_ARG_INFO_EX(arginfo_arraylist__construct, 0, 0, 0)
	ZEND_ARG_INFO(0, size)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_arraylist_add, 0, 0, 1)
	ZEND_ARG_INFO(0, val)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_arraylist_get, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_arraylist_offsetGet, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_arraylist_offsetSet, 0, 0, 2)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_INFO(0, newval)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_arraylist_void, 0)
ZEND_END_ARG_INFO()
/* }}} */

static void arraylist_init(arraylist *array, zend_long size) /* {{{ */
{
	array->elements = NULL;
	array->elements = (zval *)ecalloc(size, sizeof(zval));
	array->nSize = size;
	array->nNextIndex = 0;
	array->nNumUsed = 0;
}
/* }}} */

/* {{{ void arraylist::__construct()
 */
PHP_METHOD(arraylist, __construct)
{
	zend_long size = ARRAY_LIST_SIZE;
	zval *object = getThis();
	arraylist_object *intern;
	intern = Z_ARRAYLIST_P(object);

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(size)
	ZEND_PARSE_PARAMETERS_END();
	
	if (intern->array.nSize > 0) {
		return;
	}
	arraylist_init(&intern->array, size);
}
/* }}} */

static void arraylist_resize(arraylist *array) /* {{{ */
{
	if (array->nNextIndex >= array->nSize)
	{
        size_t i = 0;
		size_t oldSize = array->nSize == 1? 2 : array->nSize;
		size_t newSize = oldSize + (oldSize >> 1);
		zval *elements  = (zval *)ecalloc(newSize, sizeof(zval));
		for (; i < array->nSize; i++)
		{
			if (!Z_ISUNDEF(array->elements[i])) {
				elements[i] = array->elements[i];
           	 	zval_dtor(&array->elements[i]);
			}
		}
		efree(array->elements);
		array->elements = NULL;
		array->elements = elements;
		array->nSize = newSize;
	}
}
/* }}} */

static inline void arraylist_object_write_dimension_helper(arraylist_object *intern, zval *offset, zval *value) /* {{{ */
{
	zend_long index;

	if (!offset) {
		php_error_docref(NULL, E_NOTICE, "Index invalid or out of range 0");
		return;
	}

	if (Z_TYPE_P(offset) != IS_LONG) {
		convert_to_long(offset);
	} 
	index = Z_LVAL_P(offset);
	
	if (index < 0 || index >= intern->array.nSize) {
		php_error_docref(NULL, E_NOTICE, "Index invalid or out of range %ld", index);
		return;
	} else {
		if (!Z_ISUNDEF(intern->array.elements[index])) {
			zval_ptr_dtor(&(intern->array.elements[index]));
		}
		ZVAL_DEREF(value);
		ZVAL_COPY(&intern->array.elements[index], value);
		Z_TRY_ADDREF_P(value);
	}
}
/* }}} */


/* {{{ string arraylist::add( [  $val ] )
 */
PHP_METHOD(arraylist, add)
{
	zval *object = getThis();
	arraylist_object *intern;
    zval *val, offset;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(val)
	ZEND_PARSE_PARAMETERS_END();

	intern = Z_ARRAYLIST_P(object);
	arraylist_resize(&intern->array);
	if (intern->array.nSize > 0 && intern->array.nNextIndex < intern->array.nSize) {
		ZVAL_LONG(&offset, intern->array.nNextIndex);
		arraylist_object_write_dimension_helper(intern, &offset, val);
		intern->array.nNextIndex++;
		intern->array.nNumUsed++;
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

static inline zval *arraylist_object_read_dimension_helper(arraylist_object *intern, zval *offset) /* {{{ */
{
	zend_long index;
	if (!offset) {
		php_error_docref(NULL, E_NOTICE, "Index invalid or out of range 0");
		return NULL;
	}

	if (Z_TYPE_P(offset) != IS_LONG) {
		 convert_to_long(offset);
	} 
	index = Z_LVAL_P(offset);

	if (index < 0 || index >= intern->array.nSize) {
		php_error_docref(NULL, E_NOTICE,"Index invalid or out of range %ld", index);
		return NULL;
	} else if (Z_ISUNDEF(intern->array.elements[index])) {
		return NULL;
	} else {
		return &intern->array.elements[index];
	}
}
/* }}} */


/* {{{ zval arraylist::get(int $i)
 */
PHP_METHOD(arraylist, get)
{
	zval *object = getThis();
	arraylist_object *intern;
    zval *offset, *retval;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(offset)
	ZEND_PARSE_PARAMETERS_END();

	intern = Z_ARRAYLIST_P(object);
	
	retval = arraylist_object_read_dimension_helper(intern, offset);
	if (retval != NULL) {
		Z_TRY_ADDREF_P(retval);
		RETURN_ZVAL(retval, 0, 0);
	} else {
		RETURN_FALSE;
	}
} 


PHP_METHOD(arraylist, count)
{
	zval *object = getThis();
	arraylist_object *intern;
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	intern = Z_ARRAYLIST_P(object);
	RETURN_LONG(intern->array.nNextIndex);
}

PHP_METHOD(arraylist, getSize)
{
	zval *object = getThis();
	arraylist_object *intern;
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	intern = Z_ARRAYLIST_P(object);
	RETURN_LONG(intern->array.nSize);
}

/* {{{ proto object arraylist::toArray()
*/
PHP_METHOD(arraylist, toArray)
{
	arraylist_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern = Z_ARRAYLIST_P(getThis());
	if (intern->array.nNextIndex > 0) {
		int i = 0;
		array_init_size(return_value, intern->array.nNextIndex);
		for (; i < intern->array.nNextIndex; i++) {
			if (!Z_ISUNDEF(intern->array.elements[i])) {
				zend_hash_index_update(Z_ARRVAL_P(return_value), i, &intern->array.elements[i]);
				Z_TRY_ADDREF(intern->array.elements[i]);
			} else {
				zend_hash_index_update(Z_ARRVAL_P(return_value), i, &EG(uninitialized_zval));
			}
		}
	} else {
		RETURN_NULL();
	}
}
/* }}} */


static void arraylist_destruct(arraylist_object *intern)/* {{{ */
{

	zend_long i;
	if (intern->array.nSize > 0) {
		for (i = 0; i < intern->array.nSize; i++) {
			if (&(intern->array.elements[i]) != NULL && !Z_ISUNDEF(intern->array.elements[i])) {
				zval_ptr_dtor(&(intern->array.elements[i]));
			}
		}
		intern->array.nSize = 0;
		if (intern->array.elements) {
			efree(intern->array.elements);
			intern->array.elements = NULL;
		}
	}	
}
/* }}} */

PHP_METHOD(arraylist, __destruct)
{
}

static inline int arraylist_object_has_dimension_helper(arraylist_object *intern, zval *offset, int check_empty) /* {{{ */
{
	zend_long index;
	int retval;

	if (Z_TYPE_P(offset) != IS_LONG) {
		 convert_to_long(offset);
	} 
	index = Z_LVAL_P(offset);

	if (index < 0 || index >= intern->array.nSize) {
		retval = 0;
	} else {
		if (Z_ISUNDEF(intern->array.elements[index])) {
			retval = 0;
		} else if (check_empty) {
			if (zend_is_true(&intern->array.elements[index])) {
				retval = 1;
			} else {
				retval = 0;
			}
		} else { /* != NULL and !check_empty */
			retval = 1;
		}
	}

	return retval;
}
/* }}} */

static inline void arraylist_object_unset_dimension_helper(arraylist_object *intern, zval *offset) /* {{{ */
{
	zend_long index;

	if (Z_TYPE_P(offset) != IS_LONG) {
		 convert_to_long(offset);
	}
	index = Z_LVAL_P(offset);
	
	if (index < 0 || index >= intern->array.nSize) {
		php_error_docref(NULL, E_NOTICE,"Index invalid or out of range '%ld'", index);
		return;
	} else {
		zval_ptr_dtor(&(intern->array.elements[index]));
		ZVAL_UNDEF(&intern->array.elements[index]);
	}
}
/* }}} */

PHP_METHOD(arraylist, offsetExists)
{
	zval              *zindex;
	arraylist_object  *intern;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(zindex)
	ZEND_PARSE_PARAMETERS_END();

	intern = Z_ARRAYLIST_P(getThis());
	RETURN_BOOL(arraylist_object_has_dimension_helper(intern, zindex, 0));
}

PHP_METHOD(arraylist, offsetGet)
{
	zval              *zindex, *value;
	arraylist_object  *intern;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(zindex)
	ZEND_PARSE_PARAMETERS_END();

	intern = Z_ARRAYLIST_P(getThis());
	value = arraylist_object_read_dimension_helper(intern, zindex);

	if (value) {
		ZVAL_DEREF(value);
		ZVAL_COPY(return_value, value);
	} else {
		RETURN_NULL();
	}
}


PHP_METHOD(arraylist, offsetSet)
{
	zval              *zindex, *value;
	arraylist_object  *intern;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(zindex)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();
	
	intern = Z_ARRAYLIST_P(getThis());
	arraylist_resize(&intern->array);
	if (Z_TYPE_P(zindex) != IS_LONG) {
		ZVAL_LONG(zindex, intern->array.nNextIndex);
	}
	arraylist_object_write_dimension_helper(intern, zindex, value);
	intern->array.nNextIndex++;
	intern->array.nNumUsed++;
}

PHP_METHOD(arraylist, offsetUnset)
{
	zval              *zindex;
	arraylist_object  *intern;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(zindex)
	ZEND_PARSE_PARAMETERS_END();

	intern = Z_ARRAYLIST_P(getThis());
	arraylist_object_unset_dimension_helper(intern, zindex);
}
/* {{{ arraylist_functions[] 扩展函数
 */
static const zend_function_entry arraylist_functions[] = {
	PHP_FE_END
};
/* }}} */

/* {{{ arraylist__methods[] 扩展类方法
 */
static const zend_function_entry arraylist_methods[] = { 
	PHP_ME(arraylist, __construct,	arginfo_arraylist__construct, ZEND_ACC_PUBLIC)
	PHP_ME(arraylist, add,          arginfo_arraylist_add,        ZEND_ACC_PUBLIC)
	PHP_ME(arraylist, get,          arginfo_arraylist_get,        ZEND_ACC_PUBLIC)
	PHP_ME(arraylist, count,        arginfo_arraylist_void,       ZEND_ACC_PUBLIC)
	PHP_ME(arraylist, toArray,      arginfo_arraylist_void,       ZEND_ACC_PUBLIC)
	PHP_ME(arraylist, getSize,      arginfo_arraylist_void,       ZEND_ACC_PUBLIC)
	PHP_ME(arraylist, __destruct,   arginfo_arraylist_void,       ZEND_ACC_PUBLIC)
	PHP_ME(arraylist, offsetExists, arginfo_arraylist_offsetGet,  ZEND_ACC_PUBLIC)
	PHP_ME(arraylist, offsetGet,    arginfo_arraylist_offsetGet,  ZEND_ACC_PUBLIC)
	PHP_ME(arraylist, offsetSet,    arginfo_arraylist_offsetSet,  ZEND_ACC_PUBLIC)
	PHP_ME(arraylist, offsetUnset,  arginfo_arraylist_offsetGet,  ZEND_ACC_PUBLIC)
	PHP_FE_END
};
/* }}} */

static void arraylist_object_free_storage(zend_object *object)/* {{{ */
{
	arraylist_object *intern = arraylist_from_obj(object);
	arraylist_destruct(intern);
	zend_object_std_dtor(&intern->std);
}
/* }}} */

static zend_object *arraylist_object_new_ex(zend_class_entry *class_type, zval *orig, int clone_orig) /* {{{ */
{
	arraylist_object     *intern;
	zend_class_entry     *parent = class_type;
	int                   inherited = 0;

	intern = ecalloc(1, sizeof(arraylist_object) + zend_object_properties_size(parent));

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

	intern->current = 0;
	intern->flags = 0;

	if (orig && clone_orig) {
		arraylist_object *other = Z_ARRAYLIST_P(orig);
		arraylist_init(&intern->array, other->array.nSize);
	}

	while (parent) {
		if (parent == array_list_ce) {
			intern->std.handlers = &handler_array_list;
			break;
		}

		parent = parent->parent;
		inherited = 1;
	}

	if (!parent) { /* this must never happen */
		php_error_docref(NULL, E_COMPILE_ERROR, "Internal compiler error, Class is not child of ArrayList");
	}

	return &intern->std;
}
/* }}} */

static zend_object *arraylist_new(zend_class_entry *class_type) /* {{{ */
{
	return arraylist_object_new_ex(class_type, NULL, 0);
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(arraylist)
{
	return SUCCESS;
}
/* }}} */


/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(arraylist)
{
	return SUCCESS;
}


/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(arraylist)
{
#if defined(ZTS) && defined(COMPILE_DL_ARRAYLIST)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION  phpinfo显示扩展信息
 */
PHP_MINFO_FUNCTION(arraylist)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "arraylist support", "enabled");
	php_info_print_table_row(2, "Version", PHP_ARRAYLIST_VERSION);
	php_info_print_table_end();
}
/* }}} */

// 模块初始化时调用
PHP_MINIT_FUNCTION(arraylist) /* {{{ */ {
    zend_class_entry ce;
	//REGISTER_INI_ENTRIES(); // 注册ini

	//注册常量
	//第一个参数是常量的名字,
	//第二个参数来表明长度
	//CONST_CS标识是否大小写敏感
	REGISTER_STRINGL_CONSTANT(
		"ARRAYLIST_VERSION", 
		PHP_ARRAYLIST_VERSION, 
		sizeof(PHP_ARRAYLIST_VERSION)-1, 
		CONST_CS|CONST_PERSISTENT
	);

	INIT_CLASS_ENTRY(ce, "ArrayList", arraylist_methods); //注册类及类方法
    array_list_ce = zend_register_internal_class(&ce);
	array_list_ce->create_object   = arraylist_new;
    memcpy(&handler_array_list, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	handler_array_list.offset          = XtOffsetOf(arraylist_object, std);
	
	/* 通用对象功能 */
	handler_array_list.clone_obj       = zend_objects_clone_obj;
	handler_array_list.free_obj        = arraylist_object_free_storage;
	handler_array_list.dtor_obj        = zend_objects_destroy_object;

	/* 单个对象的功能 */
	zend_class_implements(array_list_ce, 1, zend_ce_arrayaccess);

    return SUCCESS;
}
/* }}} */

/* {{{ arraylist_module_entry
 */
zend_module_entry arraylist_module_entry = { 
	STANDARD_MODULE_HEADER,
	"arraylist",					/* 扩展名 name */
	arraylist_functions,			/* 注册函数，注意这里不是注册方法 */
	PHP_MINIT(arraylist),			/* PHP_MINIT - Module initialization */
	PHP_MSHUTDOWN(arraylist),		/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(arraylist),			/* PHP_RINIT - Request initialization */
	PHP_RSHUTDOWN(arraylist),		/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(arraylist),			/*  - Module info */
	PHP_ARRAYLIST_VERSION,		    /* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_ARRAYLIST
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(arraylist)
#endif
