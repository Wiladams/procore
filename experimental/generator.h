#pragma once

#include <coroutine>
#include <iterator>
#include <exception>
#include <utility>
#include <type_traits>
#include <ranges>

// This is a general generator that can be used
// to create any kind of iterator.
//
namespace pcore {
	// Reference
	// https://github.com/TartanLlama/generator/tree/main
	//
	template <class T>
	class [[nodiscard]] Generator {

		struct promise {
			using value_type = std::remove_reference_t<T>;
			using reference_type = std::conditional_t<std::is_pointer_v<value_type>, value_type, value_type&>;
			using pointer_type = std::conditional_t<std::is_pointer_v<value_type>, value_type, value_type*>;

			promise() = default;
			
			Generator get_return_object() noexcept {
				return Generator{ std::coroutine_handle<promise>::from_promise(*this) };
			}

			std::suspend_always initial_suspend() const { return {  }; }
			std::suspend_always final_suspend() const noexcept { return { }; }

			void return_void() const noexcept { return; }

			void unhandled_exception() noexcept {
				fException = std::current_exception();
			}
			
			void rethrow_if_exception() {
				if (fException) {
					std::rethrow_exception(fException);
				}
			}
			
			std::suspend_always yield_value(reference_type v) noexcept {
				if constexpr (std::is_pointer_v<value_type>) {
					fValue = v;
				}
				else {
					fValue = std::addressof(v);
				}
				return {};
			}

			std::exception_ptr fException;
			pointer_type fValue{};
		};
		
	public:
		using promise_type = promise;
		class sentinel {};
		
		class iterator {
			using handle_type = std::coroutine_handle<promise_type>;

		public:
			using value_type = typename promise_type::value_type;
			using reference_type = typename promise_type::reference_type;
			using pointer_type = typename promise_type::pointer_type;
			using difference_type = std::ptrdiff_t;
			
			//using iterator_category = std::input_iterator_tag;

			// Remove copy constructor and copy assignment
			iterator(const iterator&) = delete;
			iterator(iterator&& rhs) noexcept : fHandle(std::exchange(rhs.fHandle, nullptr)) {}
			iterator& operator=(const iterator&) = delete;
			iterator& operator=(iterator&& rhs) noexcept {
				fHandle = std::exchange(rhs.fHandle, nullptr);
				return *this;
			}

			friend bool operator==(iterator const& it, sentinel) noexcept {
				return (!it.fHandle || it.fHandle.done());
			}

			iterator& operator++() {
				fHandle.resume();
				if (fHandle.done()) {
					fHandle.promise().rethrow_if_exception();
				}
				return *this;
			}
			
			void operator++(int) {
				(void)this->operator++();
			}

			reference_type operator*() const
				noexcept(noexcept(std::is_nothrow_copy_constructible_v<reference_type>))
			{
				if constexpr (std::is_pointer_v<value_type>)
					return fHandle.promise().fValue;
				else
					return *fHandle.promise().fValue;
			}
			
		private:
			friend class Generator;
			iterator(handle_type handle) : fHandle(handle) {}

			handle_type fHandle;
		};


		using handle_type = std::coroutine_handle<promise_type>;

		Generator() noexcept = default;
		~Generator() {
			if (fHandle)
				fHandle.destroy();
		}
		
		Generator(Generator const&) = delete;
		Generator(Generator&& rhs) noexcept : fHandle(std::exchange(rhs.fHandle, nullptr)) {}
		Generator& operator=(Generator const&) = delete;
		Generator& operator=(Generator&& rhs) noexcept {
			swap(rhs);
			return *this;
		}

		void swap(Generator& rhs) noexcept {
			std::swap(fHandle, rhs.fHandle);
		}
		
		iterator begin()
		{
			fHandle.resume();
			if (fHandle.done()) {
				fHandle.promise().rethrow_if_exception();
			}
			return { std::exchange(fHandle, nullptr) };
		}

		sentinel end() const noexcept {
			return {};
		}

		private:
			friend class iterator;
			explicit Generator(handle_type handle) noexcept : fHandle(handle) {}

			handle_type fHandle{ nullptr };

	};


}

template <class T>
inline constexpr bool std::ranges::enable_view<pcore::Generator<T>> = true;
