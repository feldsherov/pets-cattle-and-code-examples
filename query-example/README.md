# Clang query example

## Get clang-query one way or another

I built from source. There is nice `clang-tools` [PyPi packet](https://pypi.org/project/clang-tools/).
_
## Run it and enjoy

```bash
> export CLANG_QUERY_PATH="${HOME}/llvm-project/build/bin/clang-query"
> ${CLANG_QUERY_PATH} main.cc lib.cc
[1/2] Processing file /home/feldsherov/pets-cattle-and-code-examples/query-example/main.cc.
[2/2] Processing file /home/feldsherov/pets-cattle-and-code-examples/query-example/lib.cc.
clang-query> m functionDecl(hasName("AwesomeFunc"))
Match #1: "root" binds here
    5 | bool AwesomeFunc(SomeObject*ptr);
      |^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
```
