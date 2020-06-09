--TEST--
Check for arraylist multidimensional arrays 
--SKIPIF--
<?php if (!extension_loaded("arraylist")) print "skip"; ?>
--FILE--
<?php
const SIZE = 20;
$ret2 = new ArrayList();
$ret3 = new ArrayList();
$ret4 = new ArrayList();
for ($i = 0; $i < SIZE; $i++)
{
	$ret2->add((string)$i);
	$ret3[] = ((string)(SIZE-$i));
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
?>
--EXPECT--
string(1) "0"
string(1) "1"
string(1) "2"
string(1) "3"
string(1) "4"
string(1) "5"
string(1) "6"
string(1) "7"
string(1) "8"
string(1) "9"
string(2) "10"
string(2) "11"
string(2) "12"
string(2) "13"
string(2) "14"
string(2) "15"
string(2) "16"
string(2) "17"
string(2) "18"
string(2) "19"

string(2) "20"
string(2) "19"
string(2) "18"
string(2) "17"
string(2) "16"
string(2) "15"
string(2) "14"
string(2) "13"
string(2) "12"
string(2) "11"
string(2) "10"
string(1) "9"
string(1) "8"
string(1) "7"
string(1) "6"
string(1) "5"
string(1) "4"
string(1) "3"
string(1) "2"
string(1) "1"