//
// Created by Alex on 12/13/2020.
//

#pragma once

#include <memory>

namespace RPG {
	namespace InternalPointerDeleter {
		template<class T>
		void Deleter(T* victim) {
			delete victim;
		}
	}

	template<class T, class Deleter = void (*)(T*)>
	using InternalPointer = std::unique_ptr<T, Deleter>;

	template<class T, class... Args>
	inline InternalPointer<T> MakeInternalPointer(Args&&... args) {
		return InternalPointer<T>(new T(std::forward<Args>(args)...), &InternalPointerDeleter::Deleter<T>);
	}
}
