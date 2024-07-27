#pragma once
#ifdef NIFO_GUI_USE_QT
#include <QVariant>
#endif
#include <entt/entt.hpp>
#include <typeinfo>
#include <typeindex>
#include "meta.h"

namespace nifo {

	struct bad_any_reference_cast : std::bad_cast {
		using std::bad_cast::bad_cast;
	};

	class any_ref {
	public:

		any_ref() = default;

		template<typename T> requires std::same_as<T, std::remove_cv_t<T>> && (not std::same_as<T, any_ref>)
		any_ref(T& p) noexcept : p(std::addressof(p)), type_(entt::type_id<T>()) {}

		template<typename T> requires std::same_as<T, std::remove_cv_t<T>>
		auto as() const ->T& {
			if (type_ != entt::type_id<T>()) throw bad_any_reference_cast{};
			return *static_cast<T*>(p);
		}

		template<typename T> requires (not std::same_as<std::remove_cv_t<T>, any_ref>) && (not nifo::meta::function<T>)
		operator T& () const noexcept { // if entt::type_id<T>() is not equal to type_index_, behavior is undefined
			return *static_cast<T*>(p);
		}

		friend auto operator==(const any_ref& lhs, const any_ref& rhs) ->bool = default;

#ifdef NIFO_GUI_USE_QT
		friend auto operator<< (QDataStream& out, const any_ref& wrapper) ->QDataStream& {
			QList<std::byte> bytes(sizeof(void*));
			std::uninitialized_copy_n(reinterpret_cast<const std::byte*>(&wrapper.p), sizeof(void*), bytes.begin());
			out << bytes;
			bytes.resize(sizeof(entt::type_info));
			std::copy_n(reinterpret_cast<const std::byte*>(&wrapper.type_), sizeof(entt::type_info), bytes.begin());
			out << bytes;
			return out;
		}

		friend auto operator>> (QDataStream& out, any_ref& wrapper) ->QDataStream& {
			QList<std::byte> bytes;
			out >> bytes;
			std::uninitialized_copy_n(bytes.begin(), bytes.size(), reinterpret_cast<std::byte*>(&wrapper.p));
			out >> bytes;
			std::uninitialized_copy_n(bytes.begin(), bytes.size(), reinterpret_cast<std::byte*>(&wrapper.type_));
			return out;
		}
#endif

	private:
		void* p = nullptr;
		entt::type_info type_ = entt::type_id<void>();
	};
}

#ifdef NIFO_GUI_USE_QT
Q_DECLARE_METATYPE(nifo::any_ref)
#endif

static_assert(std::is_trivially_copyable_v<nifo::any_ref>);
