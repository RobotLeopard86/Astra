#pragma once

#include "callback.hpp"

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/Tooling.h"

class Action : public ASTFrontendAction {
  public:
	explicit Action(Context* ctx)
	  : _ast_callback(ctx), _ctx(ctx) {
		static const auto classMatcher = cxxRecordDecl(isDefinition(), unless(isExpansionInSystemHeader())).bind("c");
		_finder.addMatcher(classMatcher, &_ast_callback);

		static const auto enumMatcher = enumDecl(unless(isExpansionInSystemHeader())).bind("e");
		_finder.addMatcher(enumMatcher, &_ast_callback);
	}

	std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance& /*compiler*/, StringRef /*in_file*/) override {
		//forward work to match finder
		return _finder.newASTConsumer();
	}

  private:
	MatchFinder _finder;
	AstCallback _ast_callback;
	[[maybe_unused]] Context* _ctx;
};

class ActionFactory : public tooling::FrontendActionFactory {
  public:
	explicit ActionFactory(Context* ctx)
	  : _ctx(ctx) {
	}

	std::unique_ptr<FrontendAction> create() override {
		return std::make_unique<Action>(_ctx);
	}

  private:
	Context* _ctx;
};
