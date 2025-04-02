#include <userver/utest/utest.hpp>
#include "../utils/filename_generator.hpp"

namespace nl::utils {

class FilenameGeneratorTestClass : public FilenameGenerator {
public:
    using FilenameGenerator::GetFileExtension; 
};

class FilenameGeneratorTest : public ::testing::Test {
protected:
    FilenameGeneratorTestClass generator;
};

TEST_F(FilenameGeneratorTest, GetFileExtension_WithValidExtension) {
    EXPECT_EQ(generator.GetFileExtension("file.txt"), "txt");
    EXPECT_EQ(generator.GetFileExtension("archive.tar.gz"), "gz");
    EXPECT_EQ(generator.GetFileExtension("image.jpeg"), "jpeg");
    EXPECT_EQ(generator.GetFileExtension("document.with.many.dots.pdf"), "pdf");
}

TEST_F(FilenameGeneratorTest, GetFileExtension_WithNoExtension) {
    EXPECT_EQ(generator.GetFileExtension("file"), "");
    EXPECT_EQ(generator.GetFileExtension(""), "");
}

TEST_F(FilenameGeneratorTest, GetFileExtension_WithDotAtStart) {
    EXPECT_EQ(generator.GetFileExtension(".hiddenfile"), "");
}

TEST_F(FilenameGeneratorTest, GenerateRandomFilename_GeneratesDifferentNames) {
    std::string filename1 = generator.GenerateRandomFilename("test.txt");
    std::string filename2 = generator.GenerateRandomFilename("test.txt");
    
    EXPECT_NE(filename1, filename2);
}

TEST_F(FilenameGeneratorTest, GenerateRandomFilename_PreservesExtension) {
    std::string filename = generator.GenerateRandomFilename("document.pdf");
    size_t ext_pos = filename.rfind(".pdf");
    EXPECT_NE(ext_pos, std::string::npos);
    EXPECT_EQ(ext_pos, filename.length() - 4);
    
    filename = generator.GenerateRandomFilename("image.png");
    ext_pos = filename.rfind(".png");
    EXPECT_NE(ext_pos, std::string::npos);
    EXPECT_EQ(ext_pos, filename.length() - 4);
    
    filename = generator.GenerateRandomFilename("noextension");
    EXPECT_EQ(filename.substr(filename.length() - 1), ".");
}

TEST_F(FilenameGeneratorTest, GenerateRandomFilename_Format) {
    std::string filename = generator.GenerateRandomFilename("test.txt");
    
    size_t dot_pos = filename.rfind('.');
    ASSERT_NE(dot_pos, std::string::npos);
    
    std::string random_part = filename.substr(0, dot_pos);
    std::string extension = filename.substr(dot_pos + 1);
    
    for (char c : random_part) {
        EXPECT_TRUE(isxdigit(c)) << "Character '" << c << "' is not hexadecimal";
    }
    
    EXPECT_EQ(extension, "txt");
}

TEST_F(FilenameGeneratorTest, GenerateRandomFilename_WithEmptyFilename) {
    std::string filename = generator.GenerateRandomFilename("");
    
    EXPECT_TRUE(filename.size() > 0);
    EXPECT_EQ(filename.substr(filename.length() - 1), ".");
}

} // namespace nl::utils