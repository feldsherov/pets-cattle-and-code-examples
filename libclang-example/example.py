#!/usr/bin/env python

import argparse

from clang.cindex import Index, Cursor, CursorKind, CompilationDatabase

def find_string_literal(cursor: Cursor):
    if cursor.kind == CursorKind.STRING_LITERAL:
      return next(cursor.get_tokens()).spelling.strip('\"')

    for c in cursor.get_children():
        res = find_string_literal(c)
        if res is not None:
            return res


def find_all_spans(cursor: Cursor, result: list[tuple[str, str]]):
    if cursor.kind == CursorKind.VAR_DECL:

      type_decl = cursor.type.get_declaration()
      if type_decl.displayname != "Span":
          return
      result.append(
          (cursor.displayname, find_string_literal(cursor))
        )

    for c in cursor.get_children():
        find_all_spans(c, result)


def main():

    parser = argparse.ArgumentParser()
    parser.add_argument("-f", "--file", nargs='+', help="Source file to process.")
    parser.add_argument("-p", default='.', help="Directory with compilation_commands.json.")

    args = parser.parse_args()

    compdb = CompilationDatabase.fromDirectory(args.p)

    result = list()

    for source_file_path in args.file:
      print("Processing file", source_file_path)
      index = Index.create()
      commands = compdb.getCompileCommands(source_file_path)

      commands = [list(c.arguments) for c in commands]

      if len(commands) != 1:
          print("Unexpected length of commands", commands)
          continue

      commands = commands[0][:-2]
      print("Compile commands: ", commands)

      tu = index.parse(source_file_path, commands)
      if not tu:
          print("Fail to parse", source_file_path)
          continue

      find_all_spans(tu.cursor, result)

    print(result)

if __name__ == "__main__":
    main()