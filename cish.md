# cish - `C` alike that remove some of the thing I don't like cause Im the one making it

## `#include <>`

This will be replace by a `module` system that is compile, similar to odin and other modern languages;

`import "<module>/<file>"`
`import "std:io"

## int main(int argc, char** argv) {}

Function declaration syntax a an endless topic, but to keep C as the base, it should stay mostly the same;

## typedef

Just get rid of it;

## arrays: int xs[12]

Arrays are one of the most sugar coated topics in language design, it will probably get changed to a `slice` implementation, where the array has its size;

`int[] xs = {...}` when a initializer list is available so we can infer the size. This should be internalized as a slice{data,len}.
`int[2] xs = {1, 2}` 

## varibles:

Variables won't change much from the original `C`, it may get some sugaring later;
Const may be added, and undefined varibables won't work;

`int a = 0;`
