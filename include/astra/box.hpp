#pragma once

#include <memory_resource>
#include <cstdint>

#include "sizeof.hpp"
#include "var.hpp"
#include "dll.hpp"

namespace astra {
	///@cond
	static inline std::pmr::polymorphic_allocator<uint8_t> defaultAlloc;
	///@endcond

	/**
	 * @brief Wrapper for allocating objects by type ID and safely storing them type-erased
	 *
	 * <b>This type is move-only!</b>
	 */
	struct ASTRA_API Box {
		/**
		 * @brief Create an empty Box
		 *
		 * @param alloc Optional override for a custom allocator
		 */
		Box(std::pmr::polymorphic_allocator<uint8_t>* alloc = &defaultAlloc)
		  : alloc(alloc) {};

		///@cond
		Box(const Box& other) = delete;
		Box& operator=(const Box& other) = delete;
		~Box();
		///@endcond

		Box(Box&& other) noexcept;
		Box& operator=(Box&& other) noexcept;

		/**
		 * @brief Create a new Box storing an object of a given TypeId
		 *
		 * @param id TypeId for the type of object to store
		 * @param alloc Optional override for a custom allocator
		 */
		explicit Box(TypeId id, std::pmr::polymorphic_allocator<uint8_t>* alloc = &defaultAlloc);

		/**
		 * @brief Access the contents of the Box
		 *
		 * @return A Var that manages the contained object
		 */
		Var var();

		/**
		 * @brief Copy the contents of one Box to another
		 *
		 * @return A new Box containing a clone of the stored object
		 *
		 * @throws std::runtime_error If the contained type is not copy-constructible
		 */
		Box clone();

	  private:
		Var inner;
		std::pmr::polymorphic_allocator<uint8_t>* alloc;
	};

}
