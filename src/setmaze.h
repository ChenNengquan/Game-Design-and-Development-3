#pragma once
#include <iostream>
#include <fstream>
#include<vector>
class Maze {
public:
    Maze() {};
    void GenerateMazeOBJ(const std::string& filename) {
        //std::ofstream objFile(filename);
        std::ofstream objFile("./res/model/" + filename);  // 使用相对路径保存到目录的 res/model 文件夹下
        int width = maze[0].size();
        int height = maze.size();
        // Write vertices
        float y = 0.0f;
        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                if (maze[row][col] == 9) {
                    objFile << "v " << col * 20.0f << " " << y << " " << row * 20.0f << std::endl;
                    objFile << "v " << col * 20.0f + 20.0f << " " << y << " " << row * 20.0f << std::endl;
                    objFile << "v " << col * 20.0f + 20.0f << " " << y + 20.0f << " " << row * 20.0f << std::endl;
                    objFile << "v " << col * 20.0f << " " << y + 20.0f << " " << row * 20.0f << std::endl;
                    objFile << "v " << col * 20.0f << " " << y << " " << row * 20.0f + 20.0f << std::endl;
                    objFile << "v " << col * 20.0f + 20.0f << " " << y << " " << row * 20.0f + 20.0f << std::endl;
                    objFile << "v " << col * 20.0f + 20.0f << " " << y + 20.0f << " " << row * 20.0f + 20.0f << std::endl;
                    objFile << "v " << col * 20.0f << " " << y + 20.0f << " " << row * 20.0f + 20.0f << std::endl;
                }
            }
        }

        // Write texture coordinates
        objFile << "vt 0.0 0.0" << std::endl;
        objFile << "vt 1.0 0.0" << std::endl;
        objFile << "vt 1.0 1.0" << std::endl;
        objFile << "vt 0.0 1.0" << std::endl;

        // Write faces
        int vertexIndex = 1;
        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                if (maze[row][col] == 9) {
                    objFile << "f " << vertexIndex << "/1 " << vertexIndex + 1 << "/2 " << vertexIndex + 2 << "/3" << std::endl;
                    objFile << "f " << vertexIndex << "/1 " << vertexIndex + 2 << "/3 " << vertexIndex + 3 << "/4" << std::endl;
                    objFile << "f " << vertexIndex + 4 << "/1 " << vertexIndex + 5 << "/2 " << vertexIndex + 6 << "/3" << std::endl;
                    objFile << "f " << vertexIndex + 4 << "/1 " << vertexIndex + 6 << "/3 " << vertexIndex + 7 << "/4" << std::endl;
                    objFile << "f " << vertexIndex << "/1 " << vertexIndex + 3 << "/3 " << vertexIndex + 7 << "/4" << std::endl;
                    objFile << "f " << vertexIndex << "/1 " << vertexIndex + 7 << "/4 " << vertexIndex + 4 << "/2" << std::endl;
                    objFile << "f " << vertexIndex + 1 << "/1 " << vertexIndex + 5 << "/2 " << vertexIndex + 6 << "/3" << std::endl;
                    objFile << "f " << vertexIndex + 1 << "/1 " << vertexIndex + 6 << "/3 " << vertexIndex + 2 << "/4" << std::endl;
                    objFile << "f " << vertexIndex + 2 << "/1 " << vertexIndex + 6 << "/2 " << vertexIndex + 7 << "/3" << std::endl;
                    objFile << "f " << vertexIndex + 2 << "/1 " << vertexIndex + 7 << "/3 " << vertexIndex + 3 << "/4" << std::endl;
                    objFile << "f " << vertexIndex << "/1 " << vertexIndex + 4 << "/2 " << vertexIndex + 5 << "/3" << std::endl;
                    objFile << "f " << vertexIndex << "/1 " << vertexIndex + 5 << "/3 " << vertexIndex + 1 << "/4" << std::endl;

                    vertexIndex += 8;
                }
            }
        }

        objFile.close();
        std::cout << "maze.obj set up successfully!" << std::endl;
    };
    std::vector<std::vector<int>> GetMazeArray() {
        return maze;
    }

private :
    std::vector<std::vector<int>> maze = {

   {9, 9, 9, 9, 2, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
   {9, 9, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 9, 0},
   {0, 9, 0, 9, 9, 9, 0, 9, 9, 9, 9, 9, 0, 9, 0, 9, 9, 9, 0, 9},
   {0, 9, 0, 0, 0, 9, 0, 0, 0, 0, 9, 0, 0, 9, 0, 0, 9, 0, 0, 9},
   {0, 9, 9, 9, 0, 9, 0, 9, 9, 9, 9, 9, 0, 9, 0, 0, 9, 9, 9, 9},
   {0, 0, 0, 9, 0, 0, 0, 0, 9, 0, 0, 0, 0, 9, 9, 9, 0, 9, 0, 0},
   {9, 0, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 0, 9, 0, 0, 0, 9, 9, 9},
   {0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 9, 0, 0, 9},
   {9, 9, 9, 9, 0, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 0, 9, 9, 9, 9},
   {0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9},
   {9, 9, 9, 9, 9, 0, 9, 9, 9, 9, 9, 9, 9, 0, 9, 9, 9, 9, 9, 9},
   {0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9},
   {9, 9, 9, 9, 0, 9, 0, 9, 9, 9, 9, 9, 0, 9, 0, 0, 0, 9, 0, 9},
   {0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 9, 0, 0, 9, 0, 9, 9, 9, 0, 9},
   {9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 0, 9, 0, 0, 9, 9, 9, 9},
   {0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 9, 0, 0, 9},
   {0, 9, 0, 9, 0, 9, 9, 9, 0, 9, 9, 9, 0, 9, 0, 0, 9, 9, 9, 9},
   {0, 9, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 9, 0, 0},
   {0, 9, 9, 9, 9, 9, 9, 9, 9, 0, 9, 9, 9, 9, 0, 9, 0, 9, 9, 9},
   {9, 9, 9, 9, 9, 9, 9, 9, 9, 1, 9, 9, 9, 9, 9, 9, 9, 9, 9, 0}

    };

};

