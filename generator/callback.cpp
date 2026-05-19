#include "callback.hpp"

#include "clang/AST/Type.h"
#include "clang/AST/Attr.h"
#include "clang/AST/Decl.h"
#include "clang/Sema/Lookup.h"

#include <clang/AST/ASTContext.h>
#include <clang/AST/DeclTemplate.h>
#include <clang/AST/DeclarationName.h>
#include <clang/AST/TemplateBase.h>
#include <clang/AST/TypeBase.h>
#include <clang/Basic/IdentifierTable.h>
#include <clang/Basic/LangOptions.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Sema/Ownership.h>
#include <llvm/Support/Casting.h>

#include <filesystem>
#include <vector>

using namespace clang;
using namespace clang::ast_matchers;

void JsonBuilder::handleClass(const CXXRecordDecl* c) {
	bool ok = false;
	for(clang::Attr* attr : c->getAttrs()) {
		if(clang::AnnotateAttr* annAttr = dyn_cast<clang::AnnotateAttr>(attr)) {
			llvm::StringRef annText = annAttr->getAnnotation();
			if(annText.compare("astra.reflect") == 0) {
				ok = true;
				break;
			}
		}
	}
	if(!ok) return;

	clang::ASTContext& astCtx = c->getASTContext();
	clang::IdentifierInfo& ii = astCtx.Idents.get("astragen_myconcept_check");
	clang::ClassTemplateDecl* checkDecl = nullptr;
	for(clang::NamedDecl* namedDecl : astCtx.getTranslationUnitDecl()->lookup(&ii)) {
		if(clang::ClassTemplateDecl* ctd = dyn_cast<ClassTemplateDecl>(namedDecl)) {
			checkDecl = ctd;
			break;
		}
	}
	if(checkDecl == nullptr) return;
	llvm::SmallVector<clang::TemplateArgument, 1> templateArgs;
	templateArgs.emplace_back(astCtx.getCanonicalTypeDeclType(c));
	void* ip = nullptr;
	clang::ClassTemplateSpecializationDecl* specDecl = checkDecl->findSpecialization(templateArgs, ip);
	if(specDecl == nullptr) return;
	for(clang::Decl* decl : specDecl->decls()) {
		auto* varDecl = llvm::dyn_cast<clang::VarDecl>(decl);
		if(!varDecl || varDecl->getName() != "value")
			continue;
		clang::Expr* init = varDecl->getInit();
		if(!init) return;
		clang::Expr::EvalResult eval;
		if(!init->EvaluateAsConstantExpr(eval, astCtx)) return;
		if(!eval.Val.getInt().getBoolValue()) return;
	}

	addClass(c);
}

void JsonBuilder::handleEnum(const EnumDecl* e) {
	bool ok = false;
	for(clang::Attr* attr : e->getAttrs()) {
		if(clang::AnnotateAttr* annAttr = dyn_cast<clang::AnnotateAttr>(attr)) {
			llvm::StringRef annText = annAttr->getAnnotation();
			if(annText.compare("astra.reflect") == 0) {
				ok = true;
				break;
			}
		}
	}
	if(!ok) return;

	addEnum(e);
}

void JsonBuilder::addClass(const CXXRecordDecl* c) {
	auto name = c->getQualifiedNameAsString();

	//check if this class is already handled
	if(_ctx->result.count(name) != 0) {
		return;
	}

	nlohmann::json json;

	json["kind"] = 0;
	json["name"] = name;
	json["origin"] = std::filesystem::path(fileName(c)).filename();

	std::vector<const clang::CXXRecordDecl*> decls;

	//if(_options.count(AstraReflectAttr::Option::Base) != 0) {
	{
		auto parents = nlohmann::json::array();

		for(auto&& b : c->bases()) {
			nlohmann::json item;

			item["acc"] = accessStr(b.getAccessSpecifier());
			item["name"] = b.getType()->getAsRecordDecl()->getQualifiedNameAsString();

			parents.push_back(std::move(item));

			decls.push_back(b.getType()->getAsCXXRecordDecl());
		}
		json.emplace("parents", std::move(parents));
	}

	auto fields = nlohmann::json::array();
	auto func = nlohmann::json::array();

	std::vector<std::string> funcNames;

	//Process primary class members
	for(auto&& d : c->getPrimaryContext()->decls()) {
		if(const auto* f = dyn_cast<FieldDecl>(d)) {
			addField(&fields, f, false);
		} else if(const auto* v = dyn_cast<VarDecl>(d)) {
			addField(&fields, v, false);
		} else if(const auto* f = dyn_cast<FunctionDecl>(d)) {
			addFunction(&func, f, c->getNameAsString(), false);
			funcNames.push_back(f->getNameAsString());
		} else if(const auto* nc = dyn_cast<CXXRecordDecl>(d)) {
			if(!nc->isThisDeclarationADefinition() ||//
				nc->hasAttr<clang::AnnotateAttr>()) {
				//skip nested classes with dedicated 'reflect' attribute,
				//handle them further as root declarations
				continue;
			}
			addClass(nc);
		} else if(const auto* ne = dyn_cast<EnumDecl>(d)) {
			if(ne->hasAttr<clang::AnnotateAttr>()) {
				//skip nested enums with dedicated 'reflect' attribute,
				//handle them further as root declarations
				continue;
			}
			addEnum(ne);
		}
	}

	//Process inherited members
	for(auto&& de : decls) {
		for(auto&& d : de->getPrimaryContext()->decls()) {
			if(const auto* f = dyn_cast<FieldDecl>(d)) {
				addField(&fields, f, true);
			} else if(const auto* v = dyn_cast<VarDecl>(d)) {
				addField(&fields, v, true);
			} else if(const auto* f = dyn_cast<FunctionDecl>(d)) {
				if(std::find(funcNames.begin(), funcNames.end(), f->getNameAsString()) != funcNames.end()) continue;
				addFunction(&func, f, de->getNameAsString(), true);
				funcNames.push_back(f->getNameAsString());
			} else if(const auto* nc = dyn_cast<CXXRecordDecl>(d)) {
				if(!nc->isThisDeclarationADefinition() ||//
					nc->hasAttr<clang::AnnotateAttr>()) {
					//skip nested classes with dedicated 'reflect' attribute,
					//handle them further as root declarations
					continue;
				}
				addClass(nc);
			} else if(const auto* ne = dyn_cast<EnumDecl>(d)) {
				if(ne->hasAttr<clang::AnnotateAttr>()) {
					//skip nested enums with dedicated 'reflect' attribute,
					//handle them further as root declarations
					continue;
				}
				addEnum(ne);
			}
		}
	}

	json.emplace("fields", std::move(fields));
	json.emplace("methods", std::move(func));

	_ctx->result.emplace(std::move(name), std::move(json));
}

void JsonBuilder::addEnum(const EnumDecl* e) {
	auto name = e->getQualifiedNameAsString();

	//check if this enum is already handled
	if(_ctx->result.find(name) != _ctx->result.end()) {
		return;
	}

	nlohmann::json json;
	json["kind"] = 1;
	json["name"] = name;
	json["origin"] = std::filesystem::path(fileName(e)).filename();

	auto& arr = json["constants"];

	for(auto&& c : e->enumerators()) {
		if(c->hasAttr<clang::AnnotateAttr>()) {
			continue;
		}
		auto& item = arr.emplace_back();
		setName(&item, c);
	}

	_ctx->result.emplace(std::move(name), std::move(json));
}

void JsonBuilder::addFunction(nlohmann::json* functions, const FunctionDecl* f, const std::string& className, bool inherited) {
	if(f->hasAttr<clang::AnnotateAttr>()) {
		return;
	}

	auto acc = f->getAccess();
	/*if((acc != clang::AS_public && _options.count(AstraReflectAttr::Option::NonPublic) == 0) ||
		_options.count(AstraReflectAttr::Option::Func) == 0 || (acc == clang::AS_private && inherited)) {
		return;
	}*/

	auto name = f->getNameAsString();

	if(name == className ||								   //constructor
		(name.find(className, 1) == 1 && name[0] == '~') ||//destructor
		name.find("operator") == 0) {
		return;
	}

	auto& func = functions->emplace_back();

	setName(&func, f);
	func["acc"] = accessArr(f);
	auto ret = f->getDeclaredReturnType().getAsString();
	if(ret.compare("_Bool") == 0)
		func["return"] = "bool";
	else
		func["return"] = ret;

	auto params = nlohmann::json::array();
	for(auto&& p : f->parameters()) {
		params.emplace_back(typeStr(p->getType(), true));
	}
	func.emplace("params", std::move(params));
}

void JsonBuilder::addField(nlohmann::json* fields, const ValueDecl* v, bool inherited) {
	if(v->template hasAttr<clang::AnnotateAttr>()) {
		return;
	}

	/*auto acc = v->getAccess();
	if((acc != clang::AS_public && _options.count(AstraReflectAttr::Option::NonPublic) == 0) ||
		_options.count(AstraReflectAttr::Option::Data) == 0 || (acc == clang::AS_private && inherited)) {
		return;
	}*/

	auto& field = fields->emplace_back();

	setName(&field, v);
	field["acc"] = accessArr(v);
	field["type"] = typeStr(v->getType());
}

std::string JsonBuilder::fileName(const NamedDecl* decl) const {
	llvm::StringRef llpath = _sm->getFilename(decl->getLocation());
	std::filesystem::path path(llpath.begin(), llpath.end());
	std::string rel = "../";
	rel += std::filesystem::relative(path, _ctx->outputDir).string();
#ifdef _WIN32
	std::replace(rel.begin(), rel.end(), '\\', '/');
#endif
	return rel;
}

void JsonBuilder::setName(nlohmann::json* item, const NamedDecl* decl) {
	auto name = decl->getNameAsString();

	(*item)["name"] = name;
	(*item)["safe_name"] = name;

	if(const auto* alias = decl->getAttr<clang::AnnotateAttr>()) {
		(*item)["alias"] = alias->getAnnotation().str();

		return;
	}
	(*item)["alias"] = name;
}
