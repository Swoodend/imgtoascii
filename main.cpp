#include "lodepng.h"
#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>
struct ImageMetadata
{
    std::vector<unsigned char> image;
    unsigned width;
    unsigned height;
};

ImageMetadata extractImageMetadata()
{
    std::vector<unsigned char> image; // the raw pixels
    unsigned int width;
    unsigned int height;

    const char *filename = "./saturn.png";
    unsigned error = lodepng::decode(image, width, height, filename, LCT_RGB, 8);

    if (error)
    {
        std::cout << "decoder error" << ": " << lodepng_error_text(error) << std::endl;
        throw std::runtime_error(lodepng_error_text(error));
    }
    ImageMetadata metadata = {image, width, height};
    return metadata;
}

std::vector<unsigned char> getAsciiChars(std::vector<unsigned char> rgbImage)
{
    const char *asciiRange = " .:-=+*#%@";
    std::vector<unsigned char> indices;

    for (size_t i = 0; i < rgbImage.size(); i += 3)
    {
        float grayscale = 0.21f * rgbImage[i] + 0.72f * rgbImage[i + 1] + 0.07f * rgbImage[i + 2];
        int asciiIndex = static_cast<int>(grayscale * 9 / 255);
        asciiIndex = std::min(asciiIndex, 9);
        indices.push_back(asciiRange[asciiIndex]);
    }
    return indices;
}

void printImageAsASCII(std::vector<unsigned char> asciiVec, unsigned int originalWidth, unsigned int originalHeight)
{
    // struct winsize w;
    // ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    // unsigned int terminalWidth = w.ws_col;
    // unsigned int terminalHeight = w.ws_row;
    //
    // // Calculate scaling factors
    // double rowScale = static_cast<double>(originalHeight) / terminalHeight;
    // double colScale = static_cast<double>(originalWidth) / terminalWidth;
    //
    // // Choose the larger scale to maintain aspect ratio
    // double scale = std::max(rowScale, colScale);

    // int rowSkip = std::max(1, static_cast<int>(scale));
    // int colSkip = std::max(1, static_cast<int>(scale));
    char rowSkip = 7;
    char colSkip = 7;

    for (unsigned int i = 0; i < originalHeight; i += rowSkip)
    {
        for (unsigned int j = 0; j < originalWidth; j += colSkip)
        {
            std::cout << asciiVec[(i * originalWidth) + j];
        }
        std::cout << '\n';
    }
}

int main()
{
    try
    {
        ImageMetadata imageMetadata = extractImageMetadata();
        std::vector<unsigned char> asciiChars = getAsciiChars(imageMetadata.image);

        printImageAsASCII(asciiChars, imageMetadata.width, imageMetadata.height);
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "Runtime error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
