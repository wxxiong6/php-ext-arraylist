# php-ext-arraylist

[![Build Status](https://travis-ci.org/wxxiong6/php-ext-arraylist.svg?branch=master)](https://travis-ci.org/wxxiong6/php-ext-arraylist)

ArrayList是一个使用使用C实现类似于Java ArrayList的扩展。
适用于大数组环境,提高内存使用率，比原生array使用更少的内存。

## Requirement
php7.0 +

## Install
# Compile ArrayList in Linux mac

```shell
phpize
./configure --enable-arraylist
make
make install
```

## Document
```php
ArrayList implements ArrayAccess {
    /* 方法 */
    public __construct ([ int $size = 0 ] )
    public count ( void ) : int
    public static fromArray (void) : array
    public getSize ( void ) : int
    public get ( int $index ) : mixed
    public add (mixed $newval ) : void
    public toArray ( void ) : array
}

```

## More
```php
$ret = new ArrayList();
$ret->add(1);
$ret->get(0);
$ret->count();
$ret->getSize();
$ret->toArray();
```

```php
const SIZE = 20;
$ret2 = new ArrayList();
$ret3 = new ArrayList();
for ($i = 0; $i < SIZE; $i++)
{
	$ret2->add((string)$i);
	$ret3->add((string)(SIZE-$i));
}
$ret = new Arraylist();
$ret->add($ret2);
$ret->add($ret3);
for ($i = 0; $i < $ret->count(); $i++) {
	for($j = 0; $j < $ret->get($i)->count(); $j++) {
		var_dump($ret->get($i)->get($j));
	}
	echo "\n";
}
```

```php
$ret = new ArrayList();
$ret->add([1, 3, 4, 5, 6,]);
$ret->add([8, 9, 10]);
for ($i = 0; $i< $ret->count(); $i++) {
    foreach($ret->get($i) as $v){
        var_dump($v);
    }
}
```
