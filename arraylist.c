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
#include "zend_exceptions.h"

#include "php_arraylist.h"

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

zend_class_entry *array_list_ce;

typedef struct _arraylist { /* {{{ */
	zend_long size;    //数组大小
 	zend_long key;    //数组当前使用key 
	zval *elements;  //数组元素
} arraylist;
/* }}} */

typedef struct _arraylist_object { /* {{{ */
	arraylist             array;
	zend_function         *fptr_offset_get;
	zend_function         *fptr_offset_set;
	zend_function         *fptr_offset_has;
	zend_function         *fptr_offset_del;
	zend_function         *fptr_count;
	int                    current;
	int                    flags;
	zend_class_entry      *ce_get_iterator;
	zend_object            std;
} arraylist_object;
/* }}} */




static inline arraylist_object *arraylist_from_obj(zend_object *obj) /* {{{ */ {
	return (arraylist_object*)((char*)(obj) - XtOffsetOf(arraylist_object, std));
}
/* }}} */

#define Z_ARRAYLIST_P(zv)  arraylist_from_obj(Z_OBJ_P((zv)))




static void arraylist_init(arraylist *array, zend_long size) /* {{{ */
{
	if (size > 0) {
		array->size = 0; /* reset size in case ecalloc() fails */
		array->key = 0;
		array->elements = ecalloc(size, sizeof(zval));
		array->size = size;
	} else {
		array->elements = NULL;
		array->size = 0;
		array->key = 0;
	}

}
/* }}} */



static void resize(arraylist *array)
{
	if (array->key == array->size)
	{
		zend_long oldSize = array->size == 1? 2 : array->size;
		zend_long newSize = oldSize + (oldSize >> 1);

		zval *elements;	
		elements = ecalloc(newSize, sizeof(zval));
		zend_long i = 0;
		for (; i < newSize; i++)
		{
			elements[i] = array->elements[i];
		}
		efree(&array->elements[i]);
		array->elements = elements;
		array->size = newSize;
	}
}

static void arraylist_add(arraylist *array, zval *val) /* {{{ */
{
	if (!array)
	{
		return ;
	}
	resize(array);
	ZVAL_COPY(&array->elements[array->key], val);
	array->key++;
}
/* }}} */


/* }}}*/

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

/* {{{ arginfo
 */
ZEND_BEGIN_ARG_INFO_EX(arginfo_arraylist__construct, 0, 0, 1)
	ZEND_ARG_INFO(0, size)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_arraylist_add, 0, 0, 1)
	ZEND_ARG_INFO(0, val)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_arraylist_get, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_arraylist_void, 0)
ZEND_END_ARG_INFO()
/* }}} */


/* {{{ void arraylist::__construct()
 */
PHP_METHOD(arraylist, __construct)
{
	zend_long size = 16;

	zval *object = getThis();
	arraylist_object *intern;
	intern = Z_ARRAYLIST_P(object);

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(size)
	ZEND_PARSE_PARAMETERS_END();



	arraylist_init(&intern->array, size);

}
/* }}} */

/* {{{ string arraylist::add( [  $val ] )
 */
PHP_METHOD(arraylist, add)
{

	zval *object = getThis();
	arraylist_object *intern;
	size_t var_len;
    zval *val;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(val)
	ZEND_PARSE_PARAMETERS_END();


	intern = Z_ARRAYLIST_P(object);
	arraylist_add(&intern->array, val);
	
	RETURN_TRUE;
}

/* {{{ zval arraylist::get(int $i)
 */
PHP_METHOD(arraylist, get)
{
	zval *object = getThis();
	arraylist_object *intern;
    zend_long key;
	zval *element;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(key)
	ZEND_PARSE_PARAMETERS_END();


	intern = Z_ARRAYLIST_P(object);
	if (key <= intern->array.size)
	{
		RETURN_ZVAL(&intern->array.elements[key], 0, 1);
	} 
	else
	{
		RETURN_NULL();
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
	RETURN_LONG(intern->array.key);
}

PHP_METHOD(arraylist, getSize)
{
	zval *object = getThis();
	arraylist_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern = Z_ARRAYLIST_P(object);
	RETURN_LONG(intern->array.size);
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

	if (intern->array.size > 0) {
		int i = 0;

		array_init_size(return_value, intern->array.size);
		for (; i < intern->array.size; i++) {
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

/* {{{ arraylist_functions[] 扩展函数
 */
static const zend_function_entry arraylist_functions[] = {
	PHP_FE_END
};
/* }}} */

/* {{{ arraylist__methods[] 扩展类方法
 */
static const zend_function_entry arraylist_methods[] = { 
	PHP_ME(arraylist, __construct,	arginfo_arraylist__construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(arraylist, add, arginfo_arraylist_add, ZEND_ACC_PUBLIC)
	PHP_ME(arraylist, get, arginfo_arraylist_get, ZEND_ACC_PUBLIC)
	PHP_ME(arraylist, count, arginfo_arraylist_void, ZEND_ACC_PUBLIC)
	PHP_ME(arraylist, toArray, arginfo_arraylist_void, ZEND_ACC_PUBLIC)
	PHP_ME(arraylist, getSize, arginfo_arraylist_void, ZEND_ACC_PUBLIC)
	PHP_FE_END
};
/* }}} */

/* {{{ arraylist_module_entry
 */
zend_module_entry arraylist_module_entry = { 
	STANDARD_MODULE_HEADER,
	"arraylist",					/* 扩展名 name */
	arraylist_functions,			/* 注册函数，注意这里不是注册方法 */
	PHP_MINIT(arraylist),							/* PHP_MINIT - Module initialization */
	NULL,							/* PHP_MSHUTDOWN - Module shutdown */
	NULL,			               /* PHP_RINIT - Request initialization */
	NULL,							/* PHP_RSHUTDOWN - Request shutdown */
	NULL,			/* 
 - Module info */
	PHP_ARRAYLIST_VERSION,		/* Version */
	STANDARD_MODULE_PROPERTIES
};
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

    return SUCCESS;
}
/* }}} */


#ifdef COMPILE_DL_ARRAYLIST
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(arraylist)
#endif

