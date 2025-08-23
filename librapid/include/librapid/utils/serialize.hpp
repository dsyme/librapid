#ifndef LIBRAPID_UTILS_SERIALIZE_HPP
#define LIBRAPID_UTILS_SERIALIZE_HPP

namespace librapid {
	namespace detail {
		static std::ios_base::openmode getFileMode(const std::string &path) {
			// Check if the file is binary
			if (path.find(".bin") != std::string::npos) {
				return std::ios::out | std::ios::binary;
			} else {
				return std::ios::out;
			}
		}
	} // namespace detail

	template<typename T>
	struct SerializerImpl {
		// Optimized type hashing without runtime string operations
		LIBRAPID_NODISCARD static size_t hasher() {
			// Use type_index for consistent, fast type hashing (avoids string construction)
			static const size_t hash = std::hash<std::type_index>{}(std::type_index(typeid(T)));
			return hash;
		}

		LIBRAPID_NODISCARD static std::vector<uint8_t> serialize(const T &obj) {
			std::vector<uint8_t> data;
			data.resize(sizeof(T) + sizeof(size_t));
			size_t hashed = hasher();
			memcpy(data.data() + sizeof(T), &hashed, sizeof(size_t));
			memcpy(data.data(), &obj, sizeof(T));
			return data;
		}

		LIBRAPID_NODISCARD static T deserialize(const std::vector<uint8_t> &data) {
			size_t hashed;
			memcpy(&hashed, data.data() + sizeof(T), sizeof(size_t));
			LIBRAPID_ASSERT(
			  hasher() == hashed,
			  "Hash mismatch. Ensure the types are the same and the data is not corrupted.");

			T obj;
			memcpy(&obj, data.data(), sizeof(T));
			return obj;
		}

		LIBRAPID_NODISCARD static void serialize(const T &obj, const std::string &path) {
			auto data = serialize(obj);
			std::ofstream file(path, detail::getFileMode(path));
			file.write(reinterpret_cast<const char *>(data.data()), (int64_t)data.size());
			file.close();
		}

		LIBRAPID_NODISCARD static T deserialize(const std::string &path) {
			std::ifstream file(path, std::ios::in | std::ios::binary);
			file.seekg(0, std::ios::end);
			size_t fileSize = file.tellg();
			file.seekg(0, std::ios::beg);
			std::vector<uint8_t> data(fileSize);
			file.read(reinterpret_cast<char *>(data.data()), (int64_t)fileSize);
			file.close();
			return deserialize(data);
		}

		LIBRAPID_NODISCARD static void serialize(const T &obj, std::ofstream &file) {
			auto data = serialize(obj);
			file.write(reinterpret_cast<const char *>(data.data()), (int64_t)data.size());
		}

		LIBRAPID_NODISCARD static T deserialize(std::ifstream &file) {
			// Get the size of the type + hash
			size_t dataSize = sizeof(T) + sizeof(size_t);
			std::vector<uint8_t> data(dataSize);
			file.read(reinterpret_cast<char *>(data.data()), (int64_t)dataSize);
			return deserialize(data);
		}
	};

	template<typename T>
	std::vector<uint8_t> serialize(const T &obj) {
		return SerializerImpl<T>::serialize(obj);
	}

	template<typename T>
	T deserialize(const std::vector<uint8_t> &data) {
		return SerializerImpl<T>::deserialize(data);
	}

	template<typename T>
	void serialize(const T &obj, const std::string &path) {
		SerializerImpl<T>::serialize(obj, path);
	}

	template<typename T>
	T deserialize(const std::string &path) {
		return SerializerImpl<T>::deserialize(path);
	}

	template<typename T>
	void serialize(const T &obj, std::ofstream &file) {
		SerializerImpl<T>::serialize(obj, file);
	}

	template<typename T>
	T deserialize(std::ifstream &file) {
		return SerializerImpl<T>::deserialize(file);
	}
} // namespace librapid

#endif // LIBRAPID_UTILS_SERIALIZE_HPP