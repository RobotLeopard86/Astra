#include "callback.hpp"

#include "clang/AST/DeclBase.h"
#include "clang/AST/Type.h"
#include "clang/AST/Attr.h"
#include "clang/AST/Decl.h"
#include "clang/Sema/Lookup.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Attrs.inc"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/DeclarationName.h"
#include "clang/AST/TemplateBase.h"
#include "clang/AST/TypeBase.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/Specifiers.h"
#include "clang/Sema/Ownership.h"
#include "llvm/Support/Casting.h"

#include <filesystem>
#include <stdexcept>
#include <vector>

using namespace clang;
using namespace clang::ast_matchers;

bool hasReflectAttr(const clang::Decl* decl) {
	if(!decl->hasAttrs()) return false;
	for(clang::Attr* attr : decl->getAttrs()) {
		if(clang::AnnotateAttr* annAttr = dyn_cast<clang::AnnotateAttr>(attr)) {
			llvm::StringRef annText = annAttr->getAnnotation();
			if(annText.compare("astra.reflect") == 0) {
				return true;
			}
		}
	}
	return false;
}

bool hasIgnoreAttr(const clang::Decl* decl) {
	if(!decl->hasAttrs()) return false;
	for(clang::Attr* attr : decl->getAttrs()) {
		if(clang::AnnotateAttr* annAttr = dyn_cast<clang::AnnotateAttr>(attr)) {
			llvm::StringRef annText = annAttr->getAnnotation();
			if(annText.compare("astra.ignore") == 0) {
				return true;
			}
		}
	}
	return false;
}

bool hasAliasAttr(const clang::Decl* decl) {
	if(!decl->hasAttrs()) return false;
	for(clang::Attr* attr : decl->getAttrs()) {
		if(clang::AnnotateAttr* annAttr = dyn_cast<clang::AnnotateAttr>(attr)) {
			llvm::StringRef annText = annAttr->getAnnotation();
			if(annText.starts_with("astra.alias:")) {
				return true;
			}
		}
	}
	return false;
}

std::string getAttrAlias(const clang::Decl* decl) {
	if(!decl->hasAttrs()) return "";
	for(clang::Attr* attr : decl->getAttrs()) {
		if(clang::AnnotateAttr* annAttr = dyn_cast<clang::AnnotateAttr>(attr)) {
			llvm::StringRef annText = annAttr->getAnnotation();
			if(annText.starts_with("astra.alias:")) {
				return annText.substr(12).str();
			}
		}
	}
	throw std::runtime_error("This decl does not have an alias!");
}

void JsonBuilder::handleClass(const CXXRecordDecl* c) {
	if(srcMgr->isInSystemHeader(c->getLocation())) return;
	if(!srcMgr->isInMainFile(c->getLocation())) return;
	if(!hasReflectAttr(c)) return;
	addClass(c);
}

void JsonBuilder::handleEnum(const EnumDecl* e) {
	if(srcMgr->isInSystemHeader(e->getLocation())) return;
	if(!srcMgr->isInMainFile(e->getLocation())) return;
	if(!hasReflectAttr(e)) return;
	addEnum(e);
}

void JsonBuilder::addClass(const CXXRecordDecl* c) {
	auto name = c->getQualifiedNameAsString();

	//check if this class is already handled
	if(context->result.count(name) != 0) {
		return;
	}

	nlohmann::json json;

	json["kind"] = 0;
	json["name"] = name;
	json["origin"] = std::filesystem::path(fileName(c)).filename();

	//Generate namespace identifier
	if(const clang::NamespaceDecl* nsDecl = llvm::dyn_cast<clang::NamespaceDecl>(c->getDeclContext()->getEnclosingNamespaceContext())) {
		std::vector<const clang::NamespaceDecl*> namespaces;
		const clang::DeclContext* dc = nsDecl;
		while(dc) {
			if(const auto* ns = llvm::dyn_cast<clang::NamespaceDecl>(dc)) {
				namespaces.push_back(ns);
			}
			dc = dc->getParent();
		}
		if(!namespaces.empty()) {
			std::string qualifiedNS = "";
			for(auto it = namespaces.rbegin(); it != namespaces.rend(); ++it) {
				if(it != namespaces.rbegin()) qualifiedNS += "::";
				qualifiedNS += (*it)->getNameAsString();
			}
			json["namespace"] = qualifiedNS;
		} else {
			json["namespace"] = "";
		}
	} else {
		json["namespace"] = "";
	}

	std::vector<const clang::CXXRecordDecl*> decls;

	auto parents = nlohmann::json::array();

	for(auto&& b : c->bases()) {
		nlohmann::json item;

		item["acc"] = accessStr(b.getAccessSpecifier());
		item["name"] = b.getType()->getAsRecordDecl()->getQualifiedNameAsString();

		parents.push_back(std::move(item));

		decls.push_back(b.getType()->getAsCXXRecordDecl());
	}
	json.emplace("parents", std::move(parents));

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
			if(!nc->isThisDeclarationADefinition() || hasReflectAttr(nc)) {
				//skip nested classes with dedicated 'reflect' attribute,
				//handle them further as root declarations
				continue;
			}
			addClass(nc);
		} else if(const auto* ne = dyn_cast<EnumDecl>(d)) {
			if(hasReflectAttr(ne)) {
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
				if(!nc->isThisDeclarationADefinition() || hasReflectAttr(nc)) {
					//skip nested classes with dedicated 'reflect' attribute,
					//handle them further as root declarations
					continue;
				}
				addClass(nc);
			} else if(const auto* ne = dyn_cast<EnumDecl>(d)) {
				if(hasReflectAttr(ne)) {
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

	context->result.emplace(std::move(name), std::move(json));
}

void JsonBuilder::addEnum(const EnumDecl* e) {
	auto name = e->getQualifiedNameAsString();

	//check if this enum is already handled
	if(context->result.find(name) != context->result.end()) {
		return;
	}

	nlohmann::json json;
	json["kind"] = 1;
	json["name"] = name;
	json["origin"] = std::filesystem::path(fileName(e)).filename();

	//Generate namespace identifier
	if(const clang::NamespaceDecl* nsDecl = llvm::dyn_cast<clang::NamespaceDecl>(e->getDeclContext()->getEnclosingNamespaceContext())) {
		std::vector<const clang::NamespaceDecl*> namespaces;
		const clang::DeclContext* dc = nsDecl;
		while(dc) {
			if(const auto* ns = llvm::dyn_cast<clang::NamespaceDecl>(dc)) {
				namespaces.push_back(ns);
			}
			dc = dc->getParent();
		}
		if(!namespaces.empty()) {
			std::string qualifiedNS = "";
			for(auto it = namespaces.rbegin(); it != namespaces.rend(); ++it) {
				if(it != namespaces.rbegin()) qualifiedNS += "::";
				qualifiedNS += (*it)->getNameAsString();
			}
			json["namespace"] = qualifiedNS;
		} else {
			json["namespace"] = "";
		}
	} else {
		json["namespace"] = "";
	}

	auto& arr = json["constants"];

	for(auto&& c : e->enumerators()) {
		if(hasIgnoreAttr(c)) {
			continue;
		}
		auto& item = arr.emplace_back();
		setName(&item, c);
	}

	context->result.emplace(std::move(name), std::move(json));
}

void JsonBuilder::addFunction(nlohmann::json* functions, const FunctionDecl* f, const std::string& className, bool inherited) {
	if(f->hasAttr<clang::AnnotateAttr>()) {
		return;
	}

	AccessSpecifier acc = f->getAccess();
	if(acc == clang::AS_private && inherited) {
		return;
	}

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
	if(hasIgnoreAttr(v)) {
		return;
	}

	AccessSpecifier acc = v->getAccess();
	if(acc == clang::AS_private && inherited) {
		return;
	}

	auto& field = fields->emplace_back();

	setName(&field, v);
	field["acc"] = accessArr(v);
	field["type"] = typeStr(v->getType());
}

std::string JsonBuilder::fileName(const NamedDecl* decl) const {
	llvm::StringRef llpath = srcMgr->getFilename(decl->getLocation());
	std::filesystem::path path(llpath.begin(), llpath.end());
	std::string rel = "../";
	rel += std::filesystem::relative(path, context->outputDir).string();
#ifdef _WIN32
	std::replace(rel.begin(), rel.end(), '\\', '/');
#endif
	return rel;
}

void JsonBuilder::setName(nlohmann::json* item, const NamedDecl* decl) {
	auto name = decl->getNameAsString();

	(*item)["name"] = name;
	(*item)["safe_name"] = name;

	if(hasAliasAttr(decl)) {
		(*item)["alias"] = getAttrAlias(decl);
	} else {
		(*item)["alias"] = name;
	}
}
