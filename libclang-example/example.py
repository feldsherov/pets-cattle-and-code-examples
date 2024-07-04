#!/usr/bin/env python

import argparse
import sys
import dataclasses

from clang.cindex import Index, Cursor, CursorKind, CompilationDatabase, SourceRange


@dataclasses.dataclass()
class MessageDescription:
    source_range: SourceRange
    message: str


@dataclasses.dataclass()
class SpanDescription:
    message_description: MessageDescription | None
    variable_name: str


def find_string_literal(cursor: Cursor) -> MessageDescription | None:
    if cursor.kind == CursorKind.STRING_LITERAL:
        return MessageDescription(
            source_range=cursor.extent,
            message=next(cursor.get_tokens()).spelling.strip('"'),
        )

    for c in cursor.get_children():
        res = find_string_literal(c)
        if res is not None:
            return res


def find_all_spans(cursor: Cursor, result: list[SpanDescription]):
    if cursor.kind == CursorKind.VAR_DECL:

        type_decl = cursor.type.get_declaration()
        if type_decl.displayname != "Span":
            return

        var_name = cursor.displayname
        message_description = find_string_literal(cursor)
        result.append(
            SpanDescription(
                message_description=message_description, variable_name=var_name
            )
        )

    for c in cursor.get_children():
        find_all_spans(c, result)


def output_result(spans: list[SpanDescription]):
    for s in spans:
        print(
            f"Found span:\n"
            f" name: {s.variable_name}\n"
            f" location:\n"
            f"  start: {s.message_description.source_range.start}\n"
            f"  end: {s.message_description.source_range.end}\n"
            f" message: {s.message_description.message}"
        )


def main():

    parser = argparse.ArgumentParser()
    parser.add_argument("-f", "--file", help="Source file to process.")
    parser.add_argument(
        "-p", default=".", help="Directory with compilation_commands.json."
    )

    args = parser.parse_args()

    compdb = CompilationDatabase.fromDirectory(args.p)

    result = list()

    source_file_path = args.file
    print("Processing file", source_file_path)
    index = Index.create()
    commands = compdb.getCompileCommands(source_file_path)

    commands = [list(c.arguments) for c in commands]

    if len(commands) != 1:
        print("Unexpected length of commands", commands)
        sys.exit(1)

    commands = commands[0][:-2]
    print("Compile commands: ", commands)

    tu = index.parse(source_file_path, commands)
    if not tu:
        print("Fail to parse", source_file_path)
        sys.exit(1)

    find_all_spans(tu.cursor, result)

    output_result(result)


if __name__ == "__main__":
    main()
