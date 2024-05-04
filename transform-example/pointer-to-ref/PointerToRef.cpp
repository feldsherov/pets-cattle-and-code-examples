//===---- tools/extra/ToolTemplate.cpp - Template for refactoring tool ----===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//  This file implements an empty refactoring tool using the clang tooling.
//  The goal is to lower the "barrier to entry" for writing refactoring tools.
//
//  Usage:
//  tool-template <cmake-output-dir> <file1> <file2> ...
//
//  Where <cmake-output-dir> is a CMake build directory in which a file named
//  compile_commands.json exists (enable -DCMAKE_EXPORT_COMPILE_COMMANDS in
//  CMake to get this output).
//
//  <file1> ... specify the paths of files in the CMake source tree. This path
//  is looked up in the compile command database. If the path of a file is
//  absolute, it needs to point into CMake's source tree. If the path is
//  relative, the current working directory needs to be in the CMake source
//  tree and the file must be in a subdirectory of the current working
//  directory. "./" prefixes in the relative files will be automatically
//  removed, but the rest of a relative path must be a suffix of a path in
//  the compile command line database.
//
//  For example, to use tool-template on all files in a subtree of the
//  source tree, use:
//
//    /path/in/subtree $ find . -name '*.cpp'|
//        xargs tool-template /path/to/build
//
//===----------------------------------------------------------------------===//

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Lex/Lexer.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Core/Replacement.h"
#include "clang/Tooling/Execution.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Tooling/Refactoring/AtomicChange.h"
#include "clang/Tooling/StandaloneExecution.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/Transformer/RewriteRule.h"
#include "clang/Tooling/Transformer/Stencil.h"
#include "clang/Tooling/Transformer/Transformer.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;
using namespace llvm;
using ::clang::Rewriter;
using ::clang::tooling::Transformer;
using ::clang::transformer::cat;
using ::clang::transformer::makeRule;
using ::clang::transformer::member;
using ::clang::transformer::name;
using ::clang::transformer::node;

bool applySourceChanges(const AtomicChanges &Replacements) {
  std::set<std::string> Files;
  for (const auto &Change : Replacements)
    Files.insert(Change.getFilePath());
  // FIXME: Add automatic formatting support as well.
  tooling::ApplyChangesSpec Spec;
  // FIXME: We should probably cleanup the result by default as well.
  Spec.Cleanup = false;
  for (const auto &File : Files) {
    llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> BufferErr =
        llvm::MemoryBuffer::getFile(File);
    if (!BufferErr) {
      llvm::errs() << "error: failed to open " << File << " for rewriting\n";
      return true;
    }
    auto Result = tooling::applyAtomicChanges(File, (*BufferErr)->getBuffer(),
                                              Replacements, Spec);
    if (!Result) {
      llvm::errs() << toString(Result.takeError());
      return true;
    }

    std::error_code EC;
    llvm::raw_fd_ostream OS(File, EC, llvm::sys::fs::OF_Text);
    if (EC) {
      llvm::errs() << EC.message() << "\n";
      return true;
    }
    OS << *Result;
    continue;
  }
  return false;
}

// Set up the command line options
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
static cl::OptionCategory ToolTemplateCategory("tool-template options");

int main(int argc, const char **argv) {
  llvm::sys::PrintStackTraceOnErrorSignal(argv[0]);
  llvm::Expected<CommonOptionsParser> OptionsParser =
      CommonOptionsParser::create(argc, argv, ToolTemplateCategory);

  if (!OptionsParser) {
    llvm::errs() << OptionsParser.takeError();
    return -1;
  }

  StandaloneToolExecutor Executor(std::move(OptionsParser.get()));

  std::string ArgId = "arg";

  auto Rule = makeRule(
      callExpr(callee(functionDecl(hasName("AwesomeFunc"))),
               hasArgument(0, expr(hasType(pointerType())).bind(ArgId))),
      changeTo(node(ArgId), transformer::maybeDeref(ArgId)));

  AtomicChanges AllChanges;

  Transformer TransformerInstance(
      Rule,
      [&AllChanges](Expected<llvm::MutableArrayRef<AtomicChange>> Changes) {
        if (!Changes) {
          llvm::errs() << Changes.takeError();
        }
        AllChanges.insert(AllChanges.end(),
                          std::make_move_iterator(Changes->begin()),
                          std::make_move_iterator(Changes->end()));
      });

  clang::ast_matchers::MatchFinder MatchFinder;
  TransformerInstance.registerMatchers(&MatchFinder);

  auto Err = Executor.execute(newFrontendActionFactory(&MatchFinder));
  if (Err) {
    llvm::errs() << llvm::toString(std::move(Err)) << "\n";
  }

  applySourceChanges(AllChanges);
}
