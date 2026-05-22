#pragma once

#include <string>

#include "astra/reflection.hpp"
#include "astra/stringify.hpp"
#include "astra/type_info.hpp"

namespace astra {

	template<typename T>
	inline void printSequence(const T& sequence, std::string* result, int indention);

	inline void sprint(const TypeInfo& info, std::string* result, int indention) {
		info.match(
			[result, indention](const Object& o) {
				if(!result->empty()) {
					*result += '\n';
				}
				for(auto&& record : o.getFields(Access::kAll, true)) {
					//indent a row
					*result += std::string(indention, ' ');
					//add the field name and trailing whitespace
					*result += record.first;
					*result += ": ";

					auto fieldInfo = reflect(record.second.var());
					sprint(fieldInfo, result, indention + 2);

					if(result->back() != '\n') {
						*result += '\n';
					}
				}
			},
			[result](const Bool& b) { *result += toString(b.get()); },//
			[result](const Integer& i) {
				if(i.isSigned()) {
					*result += toString(i.asSigned());
				} else {
					*result += toString(i.asUnsigned());
				}
			},
			[result](const Floating& f) { *result += toString(f.get(), 2); },
			[result](const String& s) {
				*result += "'";
				*result += s.get();
				*result += "'";
			},
			[result](const Enum& e) { *result += e.toString(); },
			[result, indention](const Map& m) {
				if(m.size() == 0) {
					*result += "[]\n";
					return;
				}

				*result += "[";
				m.forEach([result, indention](Var key, Var value) {
					auto keyInfo = reflect(key);
					sprint(keyInfo, result, indention);

					if(result->back() == '\n') {
						*result += std::string(indention, ' ');
					}

					*result += ": ";

					auto valueInfo = reflect(value);
					sprint(valueInfo, result, indention);

					*result += ", ";
				});
				result->resize(result->size() - 2);
				*result += "]";
			},
			[result, indention](const Array& a) { printSequence(a, result, indention); },
			[result, indention](const Sequence& s) { printSequence(s, result, indention); },
			[result, indention](const Pointer& p) {
				try {
					auto nestedPtr = p.getNested();
					auto info = reflect(nestedPtr);
					sprint(info, result, indention);
				} catch(...) {
					*result += "nullptr";
				}
			});
	}

	template<typename T>
	inline void printSequence(const T& sequence, std::string* result, int indention) {
		if(sequence.size() == 0) {
			*result += "[]\n";
			return;
		}

		*result += "[";
		sequence.forEach([result, indention](Var entry) {
			auto entryInfo = reflect(entry);

			sprint(entryInfo, result, indention);
			*result += ", ";
		});
		result->resize(result->size() - 2);
		*result += "]";
	}
}
