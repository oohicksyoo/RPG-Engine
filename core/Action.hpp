//
// Created by Alex on 12/20/2020.
//

#pragma once

#include <functional>
#include <unordered_map>

namespace RPG {

	using ListenerID = uint64_t;

	template<class... Args>
	struct Action {
		public:
			//Helpers
			using Callback = std::function<void(Args...)>;

			//Functions
			ListenerID AddListener(Callback callback);
			bool RemoveListener(ListenerID listenerID);
			bool RemoveListener(Callback callback);
			void RemoveAllListeners();
			uint64_t GetListenerCount();
			void Invoke(Args... args);

			//Operator Overrides
			ListenerID operator +=(Callback callback);
			bool operator -=(ListenerID listenerID);
			bool operator -=(Callback callback);

		private:
			std::unordered_map<ListenerID, Callback> callbacks;
			ListenerID currentListenerID = 0;

	};

	template<class... Args>
	ListenerID Action<Args...>::AddListener(Callback callback) {
		ListenerID listenerID = currentListenerID++;
		callbacks.emplace(listenerID, callback);
		return listenerID;
	}

	template<class... Args>
	bool Action<Args...>::RemoveListener(ListenerID listenerID) {
		return callbacks.erase(listenerID) != 0;
	}

	template<class... Args>
	bool Action<Args...>::RemoveListener(Callback callback) {
		for (auto const& [key, value] : callbacks) {
			RPG::Log("Action", std::to_string(value.target_type().hash_code()) + "|" + std::to_string(callback.target_type().hash_code()));
			if (value.target_type() == callback.target_type()) {
				RPG::Log("Action", "Found Callback");
				callbacks.erase(key);
				return true;
			}
		}
		return false;
	}

	template<class... Args>
	void Action<Args...>::RemoveAllListeners() {
		callbacks.clear();
	}

	template<class... Args>
	uint64_t Action<Args...>::GetListenerCount() {
		return callbacks.size();
	}

	template<class... Args>
	void Action<Args...>::Invoke(Args... args) {
		for (auto const& [key, value] : callbacks) {
			value(args...);
		}
	}

	template<class... Args>
	ListenerID Action<Args...>::operator+=(Callback callback) {
		return AddListener(callback);
	}


	template<class... Args>
	bool Action<Args...>::operator-=(ListenerID listenerID) {
		return RemoveListener(listenerID);
	}

	template<class... Args>
	bool Action<Args...>::operator-=(Callback callback) {
		return RemoveListener(callback);
	}
}


