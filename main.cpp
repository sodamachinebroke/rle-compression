#include <cstdint>
#include <utility>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <random>
#include <algorithm>
#include <iomanip>
#include <iostream>

std::vector<std::pair<uint8_t, uint8_t> > rle_compress(const std::vector<uint8_t> &data) {
    std::vector<std::pair<uint8_t, uint8_t> > compressed;
    if (data.empty())throw std::runtime_error("Data empty");

    uint8_t current_byte = data[0];
    uint8_t count = 1;

    for (size_t i = 1; i < data.size(); i++) {
        if (data[i] == current_byte) {
            count++;
            if (count == 255) {
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
    for (const auto &pair: compressed) decompressed.insert(decompressed.end(), pair.second, pair.first);

    return decompressed;
}

void generate_random_file(const std::string &filename) {
    std::random_device rd;
    std::mt19937 gen(rd());
    if (std::ofstream outfile(filename, std::ios::binary); outfile.is_open()) {
        std::uniform_int_distribution<uint8_t> dist(0, 255);
        std::vector<uint8_t> sample_data;
        for (int i = 0; i < 1000; ++i) {
            sample_data.push_back(dist(gen));
        }
        outfile.write(reinterpret_cast<char *>(sample_data.data()), sample_data.size());
        outfile.close();
    } else throw std::runtime_error("Unable to open file");
}

void write_compressed(const std::string &filename, std::vector<std::pair<uint8_t, uint8_t> > &compressed) {
    std::ofstream outfile(filename, std::ios::out | std::ios::binary | std::ios::trunc);
    if (outfile.is_open()) {
        for (const auto &pair: compressed) {
            outfile.put(pair.first);
            outfile.put(pair.second);
        }
    } else throw std::runtime_error("Unable to open file");
}

int main() {
    std::string filename = "./assets/input2.bin";

    //generate_random_file("./assets/generated_input.bin");

    std::ifstream infile(filename, std::ios::binary | std::ios::ate);
    if (infile.is_open()) {
        std::streamsize size = infile.tellg();
        infile.seekg(0, std::ios::beg);

        std::vector<uint8_t> file_data(size);
        infile.read(reinterpret_cast<char *>(file_data.data()), size);
        infile.close();

        std::vector<std::pair<uint8_t, uint8_t> > compressed_data = rle_compress(file_data);
        write_compressed("./assets/compressed/output2.bin", compressed_data);

        std::vector<uint8_t> decompressed = rle_decompress(compressed_data);

        if (file_data.size() == decompressed.size() && std::equal(file_data.begin(), file_data.end(),
                                                                  decompressed.begin())) {
            std::cout << "compression & decompression successful" << std::endl;
            std::cout << "Original size: " << file_data.size() << std::endl;
            std::cout << "Compressed size: " << compressed_data.size() << std::endl;
            for (const auto &pair: compressed_data) {
                std::cout << pair.first << " " << pair.second << std::endl;
            }
        } else std::cerr << "Decompression failed" << std::endl;
    } else throw std::runtime_error("Unable to open file");

    return 0;
}
