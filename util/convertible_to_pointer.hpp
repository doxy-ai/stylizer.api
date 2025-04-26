#include <concepts>
#include <memory>

/**
 * @brief Concept that defines a type that can be converted to a pointer or a smart pointer type.
 *
 * A type `Tfrom` satisfies this concept if it can be converted to a raw pointer of type `Tto*`,
 * or a `std::unique_ptr<Tto>`, or a `std::shared_ptr<Tto>`.
 *
 * @tparam Tfrom The type to be converted.
 * @tparam Tto The type to which `Tfrom` can be converted.
 */
template<typename Tfrom, typename Tto>
concept convertible_to_pointer = std::convertible_to<Tfrom, Tto*>
    || std::convertible_to<Tfrom, std::unique_ptr<Tto>>
    || std::convertible_to<Tfrom, std::shared_ptr<Tto>>;
