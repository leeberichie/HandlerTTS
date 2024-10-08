#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>
#include <cstdlib>

// Function to generate a temporary list file for FFmpeg
void generateFileList(const std::vector<std::string>& files, const std::string& listFile) {
    std::ofstream outfile(listFile);
    for (const auto& file : files) {
        outfile << "file '" << file << "'" << std::endl;
    }
    outfile.close();
}

// Main TTS function
void textToSpeech(const std::string& inputTextFile, const std::string& outputAudioFile) {
    // Open the text file
    std::ifstream infile(inputTextFile);
    if (!infile.is_open()) {
        std::cerr << "Error: Could not open input text file." << std::endl;
        return;
    }

    // Read the content of the text file
    std::string text;
    std::getline(infile, text);
    infile.close();

    // Convert text to uppercase and ignore non-alphabetical characters
    std::vector<std::string> audioFiles;
    for (size_t i = 0; i < text.size(); ++i) {
        char c = std::toupper(text[i]);
        if (c >= 'A' && c <= 'Z') {
            // Map each character to the corresponding audio file
            std::string filename = "Alphabet/" + std::string(1, c) + ".mp3";
            audioFiles.push_back(filename);
        } else if (c == ' ') {
            // Insert a pause for spaces between words
            audioFiles.push_back("Alphabet/pause.mp3");
        }
        else if (c == ',') {
            // Insert a pause for spaces between words
            audioFiles.push_back("Alphabet/coma.mp3");
        }
        else if (c == '.') {
            // Insert a pause for spaces between words
            audioFiles.push_back("Alphabet/dot.mp3");
        }
    }

    // Generate a temporary list file for FFmpeg
    std::string listFile = "file_list.txt";
    generateFileList(audioFiles, listFile);

    // Temporary concatenated file before speeding up
    std::string tempOutput = "temp_output.mp3";

    // Step 1: Concatenate audio files using FFmpeg
    std::ostringstream concatCommand;
    concatCommand << "ffmpeg -f concat -safe 0 -i " << listFile << " -c copy " << tempOutput;
    int concatResult = std::system(concatCommand.str().c_str());

    // Check if the concatenation step succeeded
    if (concatResult != 0) {
        std::cerr << "Error: Failed to concatenate audio files." << std::endl;
        return;
    }

    // Step 2: Apply speed-up by 800% using FFmpeg
    std::ostringstream speedUpCommand;
    speedUpCommand << "ffmpeg -i " << tempOutput << " -filter:a \"atempo=5.0\" -vn " << outputAudioFile;
    int speedUpResult = std::system(speedUpCommand.str().c_str());

    // Check if the speed-up step succeeded
    if (speedUpResult == 0) {
        std::cout << "Audio file created successfully with 500% speed-up: " << outputAudioFile << std::endl;
    } else {
        std::cerr << "Error: Failed to speed up audio file." << std::endl;
    }

    // Clean up temporary files
    std::remove(listFile.c_str());
    std::remove(tempOutput.c_str());
}

int main(int argc, char* argv[]) {
    // Check command-line arguments
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_text_file> <output_audio_file>" << std::endl;
        return 1;
    }

    std::string inputTextFile = argv[1];
    std::string outputAudioFile = argv[2];

    // Perform text-to-speech conversion with speed-up
    textToSpeech(inputTextFile, outputAudioFile);

    return 0;
}
