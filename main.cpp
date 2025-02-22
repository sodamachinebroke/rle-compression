#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

std::vector<std::pair<uint8_t, uint8_t> > rle_compress(const std::vector<uint8_t> &data) {
    if (data.empty()) throw std::runtime_error("Data is empty");

    std::vector<std::pair<uint8_t, uint8_t> > compressed;
    uint8_t current_byte = data[0];
    uint8_t count = 1;

    for (size_t i = 1; i < data.size(); ++i) {
        if (data[i] == current_byte) {
            if (++count == 255) {
                compressed.emplace_back(current_byte, count);
                count = 0;
            }
        } else {
            compressed.emplace_back(current_byte, count);
            current_byte = data[i];
            count = 1;
        }
    }
    compressed.emplace_back(current_byte, count);
    return compressed;
}

std::vector<uint8_t> rle_decompress(const std::vector<std::pair<uint8_t, uint8_t> > &compressed) {
    std::vector<uint8_t> decompressed;
    for (const auto &[byte, count]: compressed) {
        decompressed.insert(decompressed.end(), count, byte);
    }
    return decompressed;
}

void generate_random_file(const fs::path &filename, size_t size = 1000) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint8_t> dist(0, 255);

    std::vector<uint8_t> sample_data(size);
    std::generate(sample_data.begin(), sample_data.end(), [&]() { return dist(gen); });

    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile.is_open()) throw std::runtime_error("Unable to open file for writing: " + filename.string());

    outfile.write(reinterpret_cast<const char *>(sample_data.data()), sample_data.size());
}

void write_compressed(const fs::path &filename, const std::vector<std::pair<uint8_t, uint8_t> > &compressed) {
    std::ofstream outfile(filename, std::ios::binary | std::ios::trunc);
    if (!outfile.is_open()) throw std::runtime_error("Unable to open file for writing: " + filename.string());
    for (const auto &[byte, count]: compressed) {
        outfile.put(byte);
        outfile.put(count);
    }
}

int main() {
    fs::path input_filename = "./assets/input3.bin";
    fs::path compressed_filename = "./assets/compressed/output3.bin";

    //generate_random_file("./assets/generated_input.bin");

    std::ifstream infile(input_filename, std::ios::binary | std::ios::ate);
    if (!infile.is_open()) throw std::runtime_error("Unable to open file for reading: " + input_filename.string());

    std::streamsize size = infile.tellg();
    infile.seekg(0, std::ios::beg);

    std::vector<uint8_t> file_data(size);
    infile.read(reinterpret_cast<char *>(file_data.data()), size);

    std::vector<std::pair<uint8_t, uint8_t> > compressed_data = rle_compress(file_data);
    write_compressed(compressed_filename, compressed_data);

    if (std::vector<uint8_t> decompressed = rle_decompress(compressed_data); file_data == decompressed) {
        std::cout << "Compression & decompression successful" << std::endl;
        std::cout << "Original size: " << file_data.size() << std::endl;
        std::cout << "Compressed size: " << compressed_data.size() << std::endl;
    } else std::cerr << "Decompression failed" << std::endl;

    return 0;
}
