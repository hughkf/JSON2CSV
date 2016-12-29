# JSON2CSV
This command-line tool converts nested JSON to CSV. In the case of 'parallel' nesting, i.e. {["abc", "def"], "foo"}, the tool generates the cross product. In this example, the tool would generate:

```
foo, abc
foo, def
```

I know of no other standard or convention for how else to translate such JSON.

### Requirements:
* [JSON for Modern C++](https://github.com/nlohmann/json)
* [Boost](http://www.boost.org)

### Basic testing passed on:
* Ubuntu 16.04
* Windows 10

