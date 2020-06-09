<?php
$ret = new ArrayList();

$ret3 = new ArrayList(1);
$ret4 = new ArrayList(3);

$ret->add(1);
var_dump($ret->get(0));
var_dump($ret->count());
var_dump($ret->getSize());
var_dump(count($ret->toArray()));



$ret4->add("1");
$ret4->add("2");
$ret4->add("3");
$ret4->add("4");
$ret3->add($ret4);
$ret3->add($ret);

$ret3->add($ret);
debug_zval_dump($ret3->get(0));
for ($i=0; $i<$ret3->count(); $i++) {
    for ($j=0; $j<$ret4->count(); $j++) {
        debug_zval_dump($ret4->get($j));
    }
}