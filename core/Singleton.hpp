//
// Created by Alex on 12/27/2020.
//

#pragma once

namespace RPG {
	template<typename T>
	struct Singleton {
		public:
			static T& GetInstance() {
				static T instance;
				return instance;
			}
			Singleton(Singleton const &) = delete;
			Singleton& operator=(Singleton const &) = delete;

		protected:
			Singleton() {}
			~Singleton() {}
	};
}
