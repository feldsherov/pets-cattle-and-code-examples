## Transform example

## Get pointer-to-ref-tool

Pointer to ref is custom clang tool.
Code of it is in `pointer-to-ref` sub directory.

The most challenging part is to build it. You can copy whole `pointer-to-ref` dir
to `clang-tools-extra` in llvm-project, append `add_subdirectory(pointer-to-ref)` tot `clang-tools-extra/CMakeLists.txt` and you are ready to build.

[Here is blog post about building tool from cratch](https://www.freecodecamp.org/news/clang-ast-based-static-analysis-tools/).
[Here is official doc for building clang.](https://clang.llvm.org/get_started.html)

## Generate compile_commands.json

```bash
CMAKE_EXPORT_COMPILE_COMMANDS=ON cmake .
```

## Usage

```bash
$ ../../llvm-project/build/bin/pointer-to-ref lib.cc main.cc
[1/2] Processing file /home/feldsherov/pets-cattle-and-code-examples/transform-example/lib.cc.
[2/2] Processing file /home/feldsherov/pets-cattle-and-code-examples/transform-example/main.cc.
$ git diff
diff --git a/transform-example/main.cc b/transform-example/main.cc
index 49ce4ad..54e8218 100644
--- a/transform-example/main.cc
+++ b/transform-example/main.cc
@@ -4,7 +4,7 @@

 int main() {
   SomeObject obj;
-  AwesomeFunc(&obj);
+  AwesomeFunc(obj);

   std::cout << "I am alive!" << std::endl;
   return 0;

```
