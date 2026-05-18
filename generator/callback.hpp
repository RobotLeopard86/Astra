#pragma once

#include <llvm/ADT/StringRef.h>
#include <stdexcept>
#include <string>

#include "context.hpp"

#include "clang/AST/Type.h"
#include "clang/AST/Attr.h"
#include "clang/AST/Decl.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"

using namespace clang;
using namespace clang::ast_matchers;

struct JsonBuilder {
	JsonBuilder(Context* ctx,//
		const SourceManager* sm, const LangOptions& opts)
	  : _ctx(ctx), _sm(sm), _opts(opts) {
	}

	void handle_class(const CXXRecordDecl* c);
	void handle_enum(const EnumDecl* e);

  private:
	Context* _ctx;

	const SourceManager* _sm;
	const LangOptions& _opts;

	void add_class(const CXXRecordDecl* c);
	void add_enum(const EnumDecl* e);
	void add_function(nlohmann::json* functions, const FunctionDecl* f, const std::string& className, bool inherited);
	void add_field(nlohmann::json* fields, const ValueDecl* v, bool inherited);

	inline std::string type_str(QualType type, bool removeCVref = false) const {
		const PrintingPolicy pp(_opts);
		SplitQualType split;
		if(removeCVref) {
			type = type.getNonReferenceType();
			split = type.split();
			split.Quals.removeCVRQualifiers();
		} else {
			split = type.split();
		}
		return QualType::getAsString(split, pp);
	}

	std::string file_name(const NamedDecl* decl) const;
	static void set_name(nlohmann::json* item, const NamedDecl* decl);

	static inline nlohmann::json::array_t access_arr(const ValueDecl* decl) {
		nlohmann::json::array_t acc;

		acc.emplace_back(access_str(decl->getAccess()));

		if(const auto* f = dyn_cast<FunctionDecl>(decl)) {
			if(dyn_cast<FunctionType>(decl->getType())->isConst()) {
				acc.emplace_back("kConst");
			}
			if(f->isStatic()) {
				acc.emplace_back("kStatic");
			}
		} else {
			if(decl->getType().isConstQualified()) {
				acc.emplace_back("kConst");
			}
			if(isa<VarDecl>(decl)) {
				acc.emplace_back("kStatic");
			}
		}
		return acc;
	}

	static inline std::string access_str(AccessSpecifier access) {
		switch(access) {
			case clang::AS_public:
				return "public";
			case clang::AS_protected:
				return "protected";
			case clang::AS_private:
				return "private";
			default:
				return "none";
		}
	}
};

class AstCallback : public MatchFinder::MatchCallback {
  public:
	explicit AstCallback(Context* ctx)
	  : _ctx(ctx) {
	}

	void run(const MatchFinder::MatchResult& result) final {
		JsonBuilder builder(_ctx, result.SourceManager, result.Context->getLangOpts());

		if(const auto* c = result.Nodes.getNodeAs<CXXRecordDecl>("c")) {
			builder.handle_class(c);

		} else if(const auto* e = result.Nodes.getNodeAs<EnumDecl>("e")) {
			builder.handle_enum(e);

		} else {
			throw std::runtime_error("Unrechable case");
		}
	}

  private:
	Context* _ctx;
};
