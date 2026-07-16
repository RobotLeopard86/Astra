#pragma once

#include "callback.hpp"

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/Tooling.h"

class Action : public ASTFrontendAction {
  public:
	explicit Action(Context* ctx)
	  : astCallback(ctx), context(ctx) {
		static const auto classMatcher = cxxRecordDecl(isDefinition(), unless(isExpansionInSystemHeader())).bind("c");
		finder.addMatcher(classMatcher, &astCallback);

		static const auto enumMatcher = enumDecl(unless(isExpansionInSystemHeader())).bind("e");
		finder.addMatcher(enumMatcher, &astCallback);
	}

	std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance& /*compiler*/, StringRef /*in_file*/) override {
		//forward work to match finder
		return finder.newASTConsumer();
	}

  private:
	MatchFinder finder;
	AstCallback astCallback;
	[[maybe_unused]] Context* context;
};

class ActionFactory : public tooling::FrontendActionFactory {
  public:
	explicit ActionFactory(Context* ctx)
	  : context(ctx) {
	}

	std::unique_ptr<FrontendAction> create() override {
		return std::make_unique<Action>(context);
	}

  private:
	Context* context;
};
