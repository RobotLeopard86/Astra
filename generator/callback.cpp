#include "callback.hpp"

#include "clang/AST/Type.h"
#include "clang/AST/Attr.h"
#include "clang/AST/Decl.h"
#include "llvm/Support/Casting.h"

#include <filesystem>
#include <vector>

using namespace clang;
using namespace clang::ast_matchers;

void JsonBuilder::handle_class(const CXXRecordDecl* c) {
	bool ok = false;
	for(clang::Attr* attr : c->getAttrs()) {
		if(clang::AnnotateAttr* annAttr = llvm::dyn_cast<clang::AnnotateAttr>(attr)) {
			llvm::StringRef annText = annAttr->getAnnotation();
			if(annText.compare("astra.reflect") == 0) {
				ok = true;
				break;
			}
		}
	}
	if(!ok) return;
	add_class(c);
}

void JsonBuilder::handle_enum(const EnumDecl* e) {
	bool ok = false;
	for(clang::Attr* attr : e->getAttrs()) {
		if(clang::AnnotateAttr* annAttr = llvm::dyn_cast<clang::AnnotateAttr>(attr)) {
			llvm::StringRef annText = annAttr->getAnnotation();
			if(annText.compare("astra.reflect") == 0) {
				ok = true;
				break;
			}
		}
	}
	if(!ok) return;

	add_enum(e);
}

void JsonBuilder::add_class(const CXXRecordDecl* c) {
	auto name = c->getQualifiedNameAsString();

	//check if this class is already handled
	if(_ctx->result.count(name) != 0) {
		return;
	}

	nlohmann::json json;

	json["kind"] = 0;
	json["name"] = name;
	json["origin"] = std::filesystem::path(file_name(c)).filename();

	std::vector<const clang::CXXRecordDecl*> decls;

	//if(_options.count(AstraReflectAttr::Option::Base) != 0) {
	{
		auto parents = nlohmann::json::array();

		for(auto&& b : c->bases()) {
			nlohmann::json item;

			item["acc"] = access_str(b.getAccessSpecifier());
			item["name"] = b.getType()->getAsRecordDecl()->getQualifiedNameAsString();

			parents.push_back(std::move(item));

			decls.push_back(b.getType()->getAsCXXRecordDecl());
		}
		json.emplace("parents", std::move(parents));
	}

	auto fields = nlohmann::json::array();
	auto func = nlohmann::json::array();

	std::vector<std::string> func_names;

	//Process primary class members
	for(auto&& d : c->getPrimaryContext()->decls()) {
		if(const auto* f = dyn_cast<FieldDecl>(d)) {
			add_field(&fields, f, false);
		} else if(const auto* v = dyn_cast<VarDecl>(d)) {
			add_field(&fields, v, false);
		} else if(const auto* f = dyn_cast<FunctionDecl>(d)) {
			add_function(&func, f, c->getNameAsString(), false);
			func_names.push_back(f->getNameAsString());
		} else if(const auto* nc = dyn_cast<CXXRecordDecl>(d)) {
			if(!nc->isThisDeclarationADefinition() ||//
				nc->hasAttr<clang::AnnotateAttr>()) {
				//skip nested classes with dedicated 'reflect' attribute,
				//handle them further as root declarations
				continue;
			}
			add_class(nc);
		} else if(const auto* ne = dyn_cast<EnumDecl>(d)) {
			if(ne->hasAttr<clang::AnnotateAttr>()) {
				//skip nested enums with dedicated 'reflect' attribute,
				//handle them further as root declarations
				continue;
			}
			add_enum(ne);
		}
	}

	//Process inherited members
	for(auto&& de : decls) {
		for(auto&& d : de->getPrimaryContext()->decls()) {
			if(const auto* f = dyn_cast<FieldDecl>(d)) {
				add_field(&fields, f, true);
			} else if(const auto* v = dyn_cast<VarDecl>(d)) {
				add_field(&fields, v, true);
			} else if(const auto* f = dyn_cast<FunctionDecl>(d)) {
				if(std::find(func_names.begin(), func_names.end(), f->getNameAsString()) != func_names.end()) continue;
				add_function(&func, f, de->getNameAsString(), true);
				func_names.push_back(f->getNameAsString());
			} else if(const auto* nc = dyn_cast<CXXRecordDecl>(d)) {
				if(!nc->isThisDeclarationADefinition() ||//
					nc->hasAttr<clang::AnnotateAttr>()) {
					//skip nested classes with dedicated 'reflect' attribute,
					//handle them further as root declarations
					continue;
				}
				add_class(nc);
			} else if(const auto* ne = dyn_cast<EnumDecl>(d)) {
				if(ne->hasAttr<clang::AnnotateAttr>()) {
					//skip nested enums with dedicated 'reflect' attribute,
					//handle them further as root declarations
					continue;
				}
				add_enum(ne);
			}
		}
	}

	json.emplace("fields", std::move(fields));
	json.emplace("methods", std::move(func));

	_ctx->result.emplace(std::move(name), std::move(json));
}

void JsonBuilder::add_enum(const EnumDecl* e) {
	auto name = e->getQualifiedNameAsString();

	//check if this enum is already handled
	if(_ctx->result.find(name) != _ctx->result.end()) {
		return;
	}

	nlohmann::json json;
	json["kind"] = 1;
	json["name"] = name;
	json["origin"] = std::filesystem::path(file_name(e)).filename();

	auto& arr = json["constants"];

	for(auto&& c : e->enumerators()) {
		if(c->hasAttr<clang::AnnotateAttr>()) {
			continue;
		}
		auto& item = arr.emplace_back();
		set_name(&item, c);
	}

	_ctx->result.emplace(std::move(name), std::move(json));
}

void JsonBuilder::add_function(nlohmann::json* functions, const FunctionDecl* f, const std::string& class_name, bool inherited) {
	if(f->hasAttr<clang::AnnotateAttr>()) {
		return;
	}

	auto acc = f->getAccess();
	/*if((acc != clang::AS_public && _options.count(AstraReflectAttr::Option::NonPublic) == 0) ||
		_options.count(AstraReflectAttr::Option::Func) == 0 || (acc == clang::AS_private && inherited)) {
		return;
	}*/

	auto name = f->getNameAsString();

	if(name == class_name ||								//constructor
		(name.find(class_name, 1) == 1 && name[0] == '~') ||//destructor
		name.find("operator") == 0) {
		return;
	}

	auto& func = functions->emplace_back();

	set_name(&func, f);
	func["acc"] = access_arr(f);
	auto ret = f->getDeclaredReturnType().getAsString();
	if(ret.compare("_Bool") == 0)
		func["return"] = "bool";
	else
		func["return"] = ret;

	auto params = nlohmann::json::array();
	for(auto&& p : f->parameters()) {
		params.emplace_back(type_str(p->getType(), true));
	}
	func.emplace("params", std::move(params));
}

void JsonBuilder::add_field(nlohmann::json* fields, const ValueDecl* v, bool inherited) {
	if(v->template hasAttr<clang::AnnotateAttr>()) {
		return;
	}

	/*auto acc = v->getAccess();
	if((acc != clang::AS_public && _options.count(AstraReflectAttr::Option::NonPublic) == 0) ||
		_options.count(AstraReflectAttr::Option::Data) == 0 || (acc == clang::AS_private && inherited)) {
		return;
	}*/

	auto& field = fields->emplace_back();

	set_name(&field, v);
	field["acc"] = access_arr(v);
	field["type"] = type_str(v->getType());
}

std::string JsonBuilder::file_name(const NamedDecl* decl) const {
	llvm::StringRef llpath = _sm->getFilename(decl->getLocation());
	std::filesystem::path path(llpath.begin(), llpath.end());
	std::string rel = "../";
	rel += std::filesystem::relative(path, _ctx->output_dir).string();
#ifdef _WIN32
	std::replace(rel.begin(), rel.end(), '\\', '/');
#endif
	return rel;
}

void JsonBuilder::set_name(nlohmann::json* item, const NamedDecl* decl) {
	auto name = decl->getNameAsString();

	(*item)["name"] = name;
	(*item)["safe_name"] = name;

	if(const auto* alias = decl->getAttr<clang::AnnotateAttr>()) {
		(*item)["alias"] = alias->getAnnotation().str();

		return;
	}
	(*item)["alias"] = name;
}
