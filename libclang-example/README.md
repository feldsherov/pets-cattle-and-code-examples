# Python libclang

## Get libclang

`pip install libclang`

## Generate compile_commands.json

```bash
CMAKE_EXPORT_COMPILE_COMMANDS=ON cmake .
```
_
## Run it and enjoy

```bash
$ python example.py -f main.cc span.cc
Processing file main.cc
Compile commands:  ['/usr/bin/c++', '--driver-mode=g++', '-c', '-std=gnu++20']
Processing file span.cc
Compile commands:  ['/usr/bin/c++', '--driver-mode=g++', '-c', '-std=gnu++20']
[('event_first', 'some long explanation what is this event'), ('event_second', 'another one long explanation'), ('awesome_event', 'explanation on awesomeness')]
```
