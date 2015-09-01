## libconf

libconf is a lightweight C++ utility library for command-line programs.
It allows simple yet effective handling of command-line options and arguments,
and features JSON configuration loading and saving with as few calls as possible.

## Example code

Here is a simple JSON configuration file loading sample :

```JSON
{
        "a" : 123,
        "b" : [ true, false ]
}
```

```C++
    // We will read those variables from the file
    int a;
    std::vector<bool> b;

    // We construct the conf. file template and bind
    //   variables to it.
    json::Template tpl;
    tpl.bind("a", a)
       .bind("b", b);
    
    // Read file
    json::extract(tpl, "conf.json");
    
    // At this point a and b were loaded from the file :)
    // If you wish to modify them and save the conf. again :
    
    a = 314169;
    b.push_back(false);
    
    json::synthetize(tpl, "conf.json");
```

Commented test files are presented in the test/ directory.
To build/run a test case, type in :

```shell
make {build,run}-{testname}
# For example, to run the json.cpp test case :
make run-json
```
The test executables resides in bin/.

libconf also features a command-line arguments and options parser very simple to use,
and that will probably be improved again in the near future.
See test/cli.cpp for more usage information.

## Building

libconf uses a very simple GNU Make system, so just type in

```shell
make
```

And the statically-linked library will be created in bin/libconf.a
libconf has no other dependency than the STL and compiles fine with C++99.

## License

libconf is licensed under the GNU GPL license :

```
libconf is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

libconf is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with libconf.  If not, see <http://www.gnu.org/licenses/>.
```