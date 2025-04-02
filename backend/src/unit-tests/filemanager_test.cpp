#include "../utils/file_manager.hpp"
#include <fstream>
#include <filesystem>
#include <chrono>
#include <userver/utest/utest.hpp>

namespace nl::utils {

    class FileManagerTest : public ::testing::Test {
    protected:
        std::string test_dir_;
        void SetUp() override {
            // Создаем уникальную временную директорию для тестов
            test_dir_ = "test_attachments_" + std::to_string(
                std::chrono::system_clock::now().time_since_epoch().count());
            
            // Создаем директорию
            std::filesystem::create_directory(test_dir_);
            
            // Сохраняем оригинальную директорию attachments (если существует)
            if (std::filesystem::exists("attachments")) {
                std::filesystem::rename("attachments", "attachments_backup");
            }
            
            // Переименовываем нашу тестовую директорию в attachments
            std::filesystem::rename(test_dir_, "attachments");
        }
    
        void TearDown() override {
            // Удаляем тестовую директорию attachments
            std::filesystem::remove_all("attachments");
            
            // Восстанавливаем оригинальную директорию (если была)
            if (std::filesystem::exists("attachments_backup")) {
                std::filesystem::rename("attachments_backup", "attachments");
            }
        }
    };
    
    TEST_F(FileManagerTest, SaveFile_WithValidFile) {
        FileManager file_manager;
        userver::server::http::FormDataArg file;
        file.filename = "test.txt";
        file.value = "Hello, world!";
    
        std::string saved_filename = file_manager.SaveFile(file);
        
        // Проверяем, что файл создан
        auto file_path = std::filesystem::path("attachments") / saved_filename;
        EXPECT_TRUE(std::filesystem::exists(file_path));
        
        // Проверяем содержимое файла
        std::ifstream in_file(file_path);
        std::string content((std::istreambuf_iterator<char>(in_file)), 
                          std::istreambuf_iterator<char>());
        EXPECT_EQ(content, "Hello, world!");
    }
    
    TEST_F(FileManagerTest, ReadFile_ReturnsCorrectContent) {
        FileManager file_manager;
        // Создаем тестовый файл
        std::string test_filename = "test_read.txt";
        std::string test_content = "Test file content";
        
        std::ofstream(std::filesystem::path("attachments") / test_filename) << test_content;
    
        // Читаем через FileManager
        std::string content = file_manager.ReadFile(test_filename);
        EXPECT_EQ(content, test_content);
    }
    
    TEST_F(FileManagerTest, DeleteFile_RemovesFileSuccessfully) {
        FileManager file_manager;
        // Создаем тестовый файл
        std::string test_filename = "test_delete.txt";
        std::ofstream(std::filesystem::path("attachments") / test_filename) << "To be deleted";
    
        ASSERT_TRUE(std::filesystem::exists(std::filesystem::path("attachments") / test_filename));
        
        file_manager.DeleteFile(test_filename);
        
        EXPECT_FALSE(std::filesystem::exists(std::filesystem::path("attachments") / test_filename));
    }
    
    } // namespace nl::utils