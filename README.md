# php-ext-arraylist

[![Build Status](https://travis-ci.org/wxxiong6/php-ext-arraylist.svg?branch=master)](https://travis-ci.org/wxxiong6/php-ext-arraylist)

ArrayList是一个使用C实现类似于Java ArrayList的高性能扩展。
适用于构建超大数组,比原生数组创建数组使用时间更快，内存占用量更少。

## Requirement

php7.0 +

## Install

### Compile ArrayList in Linux mac

```shell
phpize
./configure --enable-arraylist
make
make install
```

### windows
 
 下载源码自行编译

## Document

```php
ArrayList implements ArrayAccess, Countable {
    /* 方法 */
    public __construct ([ int $size = 0 ] )
    public count ( void ) : int
    public getSize ( void ) : int
    public get ( int $index ) : mixed
    public add (mixed $newval ) : void
    public toArray ( void ) : array
}

```

## More

```php

$ret = new ArrayList();
$ret[] = 1; // 也可以使用 $ret->add(1); 
$ret[0];    // 也可以使用 $ret->get(0);
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

## 与数组写入性能对比

| Testing size | ArrayList                         | Array                             |
| ------------ | --------------------------------- | --------------------------------- |
| 1,000        | 0.00059604644775391<br />16.1875  | 0.00033903121948242<br />19.9375  |
| 2,000        | 0.00047087669372559<br />32.1875  | 0.00060915946960449<br />35.9375  |
| 4,000        | 0.00081586837768555<br />64.1875  | 0.00076293945312500<br />67.9375  |
| 8,000        | 0.00094485282897949<br />128.1875 | 0.00123691558837890<br />131.9375 |
| 2,048,000    | 0.19431805610657<br />32000.28125 | 0.32430100440979<br />33539.9375  |
| 4,096,000    | 0.44307684898376<br />64000.28125 | 0.67080807685852<br />67075.9375  |
