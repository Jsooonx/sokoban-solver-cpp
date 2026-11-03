#include "game.h"
#include <exception>
#include <iostream>
#include <string>
#include <streambuf>
#include <SFML/System/Err.hpp>

bool g_audioDeviceAvailable = true;

class AudioErrRedirector : public std::streambuf {
public:
    AudioErrRedirector(std::streambuf* original) : original(original) {}

protected:
    int overflow(int c) override {
        if (c == EOF) {
            return EOF;
        }
        char ch = static_cast<char>(c);
        buffer += ch;
        if (ch == '\n') {
            processBuffer();
        }
        return c;
    }

private:
    void processBuffer() {
        if (buffer.find("Failed to open the audio device") != std::string::npos) {
            g_audioDeviceAvailable = false;
        }
        bool suppress = false;
        if (!g_audioDeviceAvailable) {
            if (buffer.find("OpenAL") != std::string::npos || 
                buffer.find("audio device") != std::string::npos ||
                buffer.find("SoundStream") != std::string::npos ||
                buffer.find("Expression:") != std::string::npos ||
                buffer.find("Error description:") != std::string::npos ||
                buffer.find("AL_INVALID_OPERATION") != std::string::npos ||
                buffer.find("AL_") != std::string::npos ||
                buffer.find("The specified operation is not allowed") != std::string::npos) {
                suppress = true;
            } else {
                // Check for "al" followed by an uppercase letter (e.g. alGenSources, alSourcei)
                size_t pos = buffer.find("al");
                while (pos != std::string::npos) {
                    if (pos + 2 < buffer.length() && std::isupper(static_cast<unsigned char>(buffer[pos + 2]))) {
                        suppress = true;
                        break;
                    }
                    pos = buffer.find("al", pos + 1);
                }
            }
        }
        
        if (!suppress) {
            for (char c : buffer) {
                original->sputc(c);
            }
        }
        buffer.clear();
    }

    std::streambuf* original;
    std::string buffer;
};

struct StreamRedirectorGuard {
    std::streambuf* original;
    StreamRedirectorGuard(std::streambuf* orig) : original(orig) {}
    ~StreamRedirectorGuard() {
        sf::err().rdbuf(original);
    }
};

int main(int argc, char* argv[]) {
    std::streambuf* originalErrBuf = sf::err().rdbuf();
    AudioErrRedirector redirector(originalErrBuf);
    sf::err().rdbuf(&redirector);
    StreamRedirectorGuard guard(originalErrBuf);

    if (argc > 1 && std::string(argv[1]) == "--test") {
        try {
            Game game;
            if (game.runVerificationTests()) {
                std::cout << "All gameplay mechanics verification tests PASSED!\n";
                return 0;
            } else {
                std::cerr << "Some gameplay mechanics verification tests FAILED.\n";
                return 1;
            }
        } catch (const std::exception& error) {
            std::cerr << "Sokoban test error: " << error.what() << '\n';
            return 1;
        }
    }

    try {
        Game game;
        game.run();
    } catch (const std::exception& error) {
        std::cerr << "Sokoban error: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
