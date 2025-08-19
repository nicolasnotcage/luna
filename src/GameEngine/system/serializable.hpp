#ifndef SERIALIZATION_SERIALIZABLE_HPP
#define SERIALIZATION_SERIALIZABLE_HPP

#include <string>
#include <vector>

namespace cge
{

class Serializer;

// Interface for objects that can be serialized
class Serializable
{
public:
	virtual ~Serializable() = default;

	// Serialize object state to the provided serializer
	virtual void serialize(Serializer& serializer) const = 0;

	// Deserialize object state from the provided serializer
	virtual void deserialize(Serializer& serializer) = 0;
};

} // namespace cge

#endif // SERIALIZATION_SERIALIZABLE_HPP
