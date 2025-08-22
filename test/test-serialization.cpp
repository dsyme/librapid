#include <librapid>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <filesystem>
#include <fstream>

namespace lrc = librapid;

// Test struct for serialization
struct TestData {
    int intValue;
    float floatValue;
    double doubleValue;
    
    bool operator==(const TestData& other) const {
        return intValue == other.intValue &&
               floatValue == other.floatValue &&
               doubleValue == other.doubleValue;
    }
};

TEST_CASE("Test Serialization Hash Function", "[serialization]") {
    using Serializer = lrc::serialize::SerializerImpl<int>;
    
    // Hash should be consistent
    auto hash1 = Serializer::hasher();
    auto hash2 = Serializer::hasher();
    REQUIRE(hash1 == hash2);
    
    // Different types should have different hashes
    using IntSerializer = lrc::serialize::SerializerImpl<int>;
    using FloatSerializer = lrc::serialize::SerializerImpl<float>;
    
    REQUIRE(IntSerializer::hasher() != FloatSerializer::hasher());
}

TEST_CASE("Test SerializerImpl Basic Operations", "[serialization]") {
    using Serializer = lrc::serialize::SerializerImpl<int>;
    
    SECTION("Serialize and deserialize integer") {
        int originalValue = 42;
        auto data = Serializer::serialize(originalValue);
        
        REQUIRE(data.size() == sizeof(int) + sizeof(size_t));
        
        int deserializedValue = Serializer::deserialize(data);
        REQUIRE(deserializedValue == originalValue);
    }
    
    SECTION("Serialize and deserialize various values") {
        std::vector<int> testValues = {0, -1, 1, 100, -100, INT_MAX, INT_MIN};
        
        for (int value : testValues) {
            auto data = Serializer::serialize(value);
            int result = Serializer::deserialize(data);
            REQUIRE(result == value);
        }
    }
}

TEST_CASE("Test SerializerImpl with Custom Struct", "[serialization]") {
    using Serializer = lrc::serialize::SerializerImpl<TestData>;
    
    TestData original{42, 3.14f, 2.71828};
    auto data = Serializer::serialize(original);
    
    REQUIRE(data.size() == sizeof(TestData) + sizeof(size_t));
    
    TestData deserialized = Serializer::deserialize(data);
    REQUIRE(deserialized == original);
}

TEST_CASE("Test Serializer Class", "[serialization]") {
    SECTION("Default constructor") {
        lrc::serialize::Serializer<int> serializer;
        // Should be able to create without issues
    }
    
    SECTION("Constructor with value") {
        int value = 123;
        lrc::serialize::Serializer<int> serializer(value);
        
        int result = serializer.deserialize();
        REQUIRE(result == value);
    }
    
    SECTION("Copy constructor and assignment") {
        int value = 456;
        lrc::serialize::Serializer<int> original(value);
        lrc::serialize::Serializer<int> copy = original;
        
        REQUIRE(copy.deserialize() == value);
        
        lrc::serialize::Serializer<int> assigned;
        assigned = original;
        REQUIRE(assigned.deserialize() == value);
    }
    
    SECTION("Move constructor and assignment") {
        int value = 789;
        lrc::serialize::Serializer<int> original(value);
        lrc::serialize::Serializer<int> moved = std::move(original);
        
        REQUIRE(moved.deserialize() == value);
    }
}

TEST_CASE("Test Serializer Data Access", "[serialization]") {
    int value = 999;
    lrc::serialize::Serializer<int> serializer(value);
    
    SECTION("Const data access") {
        const auto& constRef = serializer;
        const auto& data = constRef.data();
        REQUIRE(data.size() == sizeof(int) + sizeof(size_t));
    }
    
    SECTION("Non-const data access") {
        auto& data = serializer.data();
        REQUIRE(data.size() == sizeof(int) + sizeof(size_t));
        
        // Should still be able to deserialize after getting reference
        REQUIRE(serializer.deserialize() == value);
    }
    
    SECTION("Serialize method") {
        int newValue = 111;
        serializer.serialize(newValue);
        REQUIRE(serializer.deserialize() == newValue);
    }
}

TEST_CASE("Test File Operations", "[serialization]") {
    const std::string testPath = "test_serialization_file.bin";
    const std::string testPathText = "test_serialization_file.txt";
    
    // Cleanup function
    auto cleanup = [&]() {
        std::filesystem::remove(testPath);
        std::filesystem::remove(testPathText);
    };
    
    cleanup(); // Clean up any existing files
    
    SECTION("Write and read binary file by path") {
        int value = 2025;
        lrc::serialize::Serializer<int> serializer(value);
        
        // Write to file
        REQUIRE(serializer.write(testPath));
        
        // Read from file
        lrc::serialize::Serializer<int> reader;
        REQUIRE(reader.read(testPath));
        REQUIRE(reader.deserialize() == value);
    }
    
    SECTION("Write and read text file by path") {
        float value = 3.14159f;
        lrc::serialize::Serializer<float> serializer(value);
        
        // Write to text file
        REQUIRE(serializer.write(testPathText));
        
        // Read from text file
        lrc::serialize::Serializer<float> reader;
        REQUIRE(reader.read(testPathText));
        REQUIRE(reader.deserialize() == value);
    }
    
    SECTION("Write and read using file streams") {
        double value = 2.718281828;
        lrc::serialize::Serializer<double> serializer(value);
        
        // Write using file stream
        std::fstream writeFile(testPath, std::ios::out | std::ios::binary);
        REQUIRE(writeFile.is_open());
        REQUIRE(serializer.write(writeFile));
        writeFile.close();
        
        // Read using file stream
        lrc::serialize::Serializer<double> reader;
        std::fstream readFile(testPath, std::ios::in | std::ios::binary);
        REQUIRE(readFile.is_open());
        REQUIRE(reader.read(readFile));
        readFile.close();
        
        REQUIRE(reader.deserialize() == value);
    }
    
    SECTION("File mode detection") {
        // Test that fileBinMode correctly detects binary files
        auto binMode = lrc::serialize::detail::fileBinMode("test.bin");
        auto textMode = lrc::serialize::detail::fileBinMode("test.txt");
        
        REQUIRE((binMode & std::ios::binary) != 0);
        REQUIRE((textMode & std::ios::binary) == 0);
    }
    
    SECTION("Read from non-existent file") {
        lrc::serialize::Serializer<int> reader;
        REQUIRE_FALSE(reader.read("non_existent_file.bin"));
    }
    
    cleanup();
}

TEST_CASE("Test Serialization Edge Cases", "[serialization]") {
    SECTION("Large struct serialization") {
        struct LargeStruct {
            int array[1000];
            double values[500];
            
            bool operator==(const LargeStruct& other) const {
                return std::memcmp(this, &other, sizeof(LargeStruct)) == 0;
            }
        };
        
        LargeStruct large;
        // Initialize with some pattern
        for (int i = 0; i < 1000; ++i) {
            large.array[i] = i;
        }
        for (int i = 0; i < 500; ++i) {
            large.values[i] = i * 0.1;
        }
        
        lrc::serialize::Serializer<LargeStruct> serializer(large);
        auto deserialized = serializer.deserialize();
        
        REQUIRE(deserialized == large);
    }
    
    SECTION("Multiple serialization cycles") {
        int original = 12345;
        lrc::serialize::Serializer<int> serializer(original);
        
        // Perform multiple serialize/deserialize cycles
        for (int cycle = 0; cycle < 10; ++cycle) {
            int current = serializer.deserialize();
            REQUIRE(current == original);
            serializer.serialize(current);
        }
    }
}